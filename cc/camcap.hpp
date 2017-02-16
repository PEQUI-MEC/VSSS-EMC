/*
 * camcap.hpp
 *
 *  Created on: Feb 18, 2014
 *      Author: gustavo
 */

#ifndef CAMCAP_HPP_
#define CAMCAP_HPP_
#define PI 3.14159265453

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/tracking.hpp>

#include "strategyGUI.hpp"
#include "controlGUI.hpp"
#include <capture-gui/V4LInterface.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <stdlib.h>
#include <tgmath.h>
#include <gtkmm.h>
#include <math.h>
#include <fstream>
#include "CPUTimer.cpp"

boost::mutex io_mutex;
boost::thread_group threshold_threads;
struct ImageGray;

class CamCap:


    public Gtk::HBox {
    public:

        cv::Mat image_copy;
        cv::Point Ball_Est;

        StrategyGUI strategy;
        ControlGUI control;
        capture::V4LInterface v;
        unsigned char * d;
        unsigned char **threshold = NULL;
        Gtk::Notebook notebook;
        int w, h;
        CPUTimer timer;
        // Team_Main[INDEX] - Vector de cv::Point
        //   GUARDA A POSIÇÃO DAS TAGS PRIMÁRIAS DO TIME(.x e .y acessam a posição)
        vector< cv::Point > Team_Main;
        // Team_Sec[COLOR_INDEX][INDEX] - Vector de Vector de cv::Point
        //	 CADA POSIÇÃO GUARDA UM VECTOR DE cv::Point PARA CADA COR SECUNDÁRIA DO TIME
        vector<vector< cv::Point >> Team_Sec;
        vector<vector< double >> Team_Sec_area;

        // ADV_Main[INDEX] - Vector de cv::Point
        //   GUARDA A POSIÇÃO DAS TAGS PRIMÁRIAS DO ADVERSÁRIO(.x e .y acessam a posição)
        vector< cv::Point > Adv_Main;
        // Ball - cv::Point
        //   GUARDA A POSIÇÃO DA BOLA
        cv::Point Ball;



        Gtk::Frame fm;
        Gtk::Frame info_fm;
        Gtk::VBox camera_vbox;
        sigc::connection con;
        unsigned char * data;
        int width, height;
        int Selec_index=-1;
        bool fixed_ball[3];
        double threshouldAuto[3][2];

        // VARIÁVEIS PARA A FRAME INFO
        Gtk::Label *label;





        std::string function[3];

        // KALMAN FILTER
        int stateSize = 6;
        int measSize = 4;
        int contrSize = 0;

        unsigned int type = CV_32F;

        cv::KalmanFilter kfBall;

        cv::Mat state;// [x,y,v_x,v_y,w,h]
        cv::Mat meas;    // [z_x,z_y,z_w,z_h]

        double ticks = 0;
        bool found = false;
        int notFoundCount = 0;


        // Função para retornar a posição de um robo
        cv::Point getRobotPosition(int robot_list_index) {
            return v.robot_list[robot_list_index].position;
        }

        double meanCalc(vector<int> v) {
            int sum = std::accumulate(v.begin(), v.end(), 0.0);
            double mean = sum / v.size();
            //cout<<"Media = "<<mean<<endl;

            return mean;
        }

        double stdevCalc(vector<int> v) {
            int sum = std::accumulate(v.begin(), v.end(), 0.0);
            double mean = sum / v.size();
            std::vector<double> diff(v.size());
            std::transform(v.begin(), v.end(), diff.begin(),
                           std::bind2nd(std::minus<double>(), mean));
            double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
            double stdev = std::sqrt(sq_sum / v.size());

            return stdev;
        }

        void autoThreshold(cv::Mat img, cv::Point p) {

            vector<int> H, S, V;

            cv::Mat imagem = img.clone();

            cv::cvtColor(imagem, imagem, cv::COLOR_RGB2HSV);
            int alpha_slider = 6;
            int horizontalOffset = alpha_slider, verticalOffset = alpha_slider, Hue, Sat, Val;

            //int threshould[3][2];

            for ( int i = (p.x-horizontalOffset); i < (p.x+horizontalOffset); i++ ) {
                for ( int j = (p.y-verticalOffset); j < (p.y+verticalOffset); j++ ) {

                    //pixel = image.at<uchar>(j,i);
                    //cout<<(int)pixel.val[0]<<"|"<<(int)pixel.val[1]<<"|"<<(int)pixel.val[2]<<endl;
                    Hue = (int)imagem.at<cv::Vec3b>(j,i)[0];
                    Sat = (int)imagem.at<cv::Vec3b>(j,i)[1];
                    Val = (int)imagem.at<cv::Vec3b>(j,i)[2];
                    //cout<< Hue <<"|"<< Sat <<"|"<< Val <<endl;
                    H.push_back(Hue);
                    S.push_back(Sat);
                    V.push_back(Val);
                }
            }

            threshouldAuto[0][0] = meanCalc(H);
            threshouldAuto[0][1] = stdevCalc(H);
            threshouldAuto[1][0] = meanCalc(S);
            threshouldAuto[1][1] = stdevCalc(S);
            threshouldAuto[2][0] = meanCalc(V);
            threshouldAuto[2][1] = stdevCalc(V);

            //cout<<"H = "<<Hue<<endl;
            //cout<<"S = "<<Sat<<endl;
            //cout<<"V = "<<Val<<endl;


            //cout<<"HMIN = "<<threshouldAuto[0][0]-threshouldAuto[0][1]<<"| HMAX = "<<threshouldAuto[0][0]+threshouldAuto[0][1]<<endl;
            //cout<<"SMIN = "<<threshouldAuto[1][0]-threshouldAuto[1][1]<<"| SMAX = "<<threshouldAuto[1][0]+threshouldAuto[1][1]<<endl;
            //cout<<"VMIN = "<<threshouldAuto[2][0]-threshouldAuto[2][1]<<"| VMAX = "<<threshouldAuto[2][0]+threshouldAuto[2][1]<<endl;

        }

        bool start_signal(bool b) {
            if (b) {

                cout << "Start Clicked!" << endl;

                if (data) {
                    v.iv.disable_image_show();
                    free(data);
                    data = 0;
                }
                Ball.x=0;
                Ball.y=0;

                /*GdkScreen* screen = gdk_screen_get_default();
                if (v.vcap.format_dest.fmt.pix.width > gdk_screen_get_width(screen)/2 || v.vcap.format_dest.fmt.pix.height > gdk_screen_get_height(screen)/2)
                {
                  width = gdk_screen_get_width(screen)/2;
                  height = gdk_screen_get_height(screen)/2;
                  strategy.strats.set_constants(width,height);
                }
                else
                {*/
                  width = v.vcap.format_dest.fmt.pix.width;
                  height = v.vcap.format_dest.fmt.pix.height;
                  strategy.strats.set_constants(width,height);
                //}


                // Liberar os botões de edit
                v.robots_id_edit_bt.set_state(Gtk::STATE_NORMAL);
                v.robots_speed_edit_bt.set_state(Gtk::STATE_NORMAL);
                v.robots_function_edit_bt.set_state(Gtk::STATE_NORMAL);
                v.robots_save_bt.set_state(Gtk::STATE_NORMAL);
                v.robots_load_bt.set_state(Gtk::STATE_NORMAL);

                threshold = (unsigned char**) malloc(6 * sizeof(unsigned char *));
                for(int i = 0; i < 6; i++)
                {
                    threshold[i] =  (unsigned char*) malloc((3*(width*height + width) +3) * sizeof(unsigned char));
                    if(threshold[i] == NULL)
                    {
                        cout<<"out of memory\n"<<endl;
                    }
                }
                data = (unsigned char *) calloc(v.vcap.format_dest.fmt.pix.sizeimage, sizeof(unsigned char));

                v.iv.set_size_request(width, height);
                con = Glib::signal_idle().connect(sigc::mem_fun(*this, &CamCap::capture_and_show));

                cout << "Start Clicked! 1" << endl;
            } else {
                cout << "Stop Button Clicked!" << endl;
                con.disconnect();

                // Travar os botões de edit
                v.robots_id_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
                v.robots_speed_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
                v.robots_function_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
                v.robots_save_bt.set_state(Gtk::STATE_INSENSITIVE);
                v.robots_load_bt.set_state(Gtk::STATE_INSENSITIVE);

            }

            v.__event_bt_quick_load_clicked();


        //KALMAN FILTER INIT
        cv::Mat stateAux(stateSize, 1, type);
        cv::Mat measAux(measSize, 1, type);
		state = stateAux;  // [x,y,v_x,v_y,w,h]
        meas = measAux;    // [z_x,z_y,z_w,z_h]

        kfBall.init(stateSize, measSize, contrSize, type);
        /*//cv::Mat procNoise(stateSize, 1, type)
        // [E_x,E_y,E_v_x,E_v_y,E_w,E_h]

        // Transition State Matrix A
        // Note: set dT at each processing step!
        // [ 1 0 dT 0  0 0 ]
        // [ 0 1 0  dT 0 0 ]
        // [ 0 0 1  0  0 0 ]
        // [ 0 0 0  1  0 0 ]
        // [ 0 0 0  0  1 0 ]
        // [ 0 0 0  0  0 1 ]*/

        cv::setIdentity(kfBall.transitionMatrix);

        /* // Measure Matrix H
        // [ 1 0 0 0 0 0 ]
        // [ 0 1 0 0 0 0 ]
        // [ 0 0 0 0 1 0 ]
        // [ 0 0 0 0 0 1 ]*/

        kfBall.measurementMatrix = cv::Mat::zeros(measSize, stateSize, type);
        kfBall.measurementMatrix.at<float>(0) = 1.0f;
        kfBall.measurementMatrix.at<float>(7) = 1.0f;
        kfBall.measurementMatrix.at<float>(16) = 1.0f;
        kfBall.measurementMatrix.at<float>(23) = 1.0f;

        /* // Process Noise Covariance Matrix Q
           // [ Ex 0  0    0 0    0 ]
           // [ 0  Ey 0    0 0    0 ]
           // [ 0  0  Ev_x 0 0    0 ]
           // [ 0  0  0    1 Ev_y 0 ]
           // [ 0  0  0    0 1    Ew ]
           // [ 0  0  0    0 0    Eh ]
           //cv::setIdentity(kfBall.processNoiseCov, cv::Scalar(1e-2));
           * */

        kfBall.processNoiseCov.at<float>(0) = 1e-2;
        kfBall.processNoiseCov.at<float>(7) = 1e-2;
        kfBall.processNoiseCov.at<float>(14) = 2.0f;
        kfBall.processNoiseCov.at<float>(21) = 1.0f;
        kfBall.processNoiseCov.at<float>(28) = 1e-2;
        kfBall.processNoiseCov.at<float>(35) = 1e-2;

        // Measures Noise Covariance Matrix R
        cv::setIdentity(kfBall.measurementNoiseCov, cv::Scalar(1e-1));

            return true;
        }

        bool capture_and_show() {
            if (!data) return false;

            //timer.start();
            v.vcap.grab_rgb(data);
            v.iv.set_data(data, width, height);


            v.iv.refresh();
            d = v.iv.get_data();

            w = v.iv.get_width();
            h = v.iv.get_height();

            cv::Mat image(h,w,CV_8UC3,d);

            if(v.iv.hold_warp) {
                v.warped = true;
                v.bt_adjust.set_state(Gtk::STATE_NORMAL);
                v.iv.warp_event_flag = false;
                v.iv.warp_event_flag = false;
                v.iv.hold_warp=false;
            }

            v.iv.auto_calib_flag = v.auto_calib_flag; // resolver

            v.iv.PID_test_flag = control.PID_test_flag;
            v.iv.adjust_event_flag = v.adjust_event_flag;

            if(v.warped) {
                v.bt_warp.set_active(false);
                v.bt_warp.set_state(Gtk::STATE_INSENSITIVE);
                warp_transform(image);
                v.iv.warp_event_flag=false;

                if(v.invert_image_flag)
                {
                    cv::flip(image,image, -1);
                }

            }

            // RESOLVER

            if(v.auto_calib_flag) {
                //cout<<"----------------------------------------"<<endl;
                cv::Point pt = v.iv.pointClicked;
                cv::Mat imgAux = image.clone();
                autoThreshold(imgAux,pt);

                double H_MIN = threshouldAuto[0][0]-threshouldAuto[0][1];
                double H_MAX = threshouldAuto[0][0]+threshouldAuto[0][1];
                double S_MIN = threshouldAuto[1][0]-threshouldAuto[1][1];
                double S_MAX = threshouldAuto[1][0]+threshouldAuto[1][1];
                double V_MIN = threshouldAuto[2][0]-threshouldAuto[2][1];
                double V_MAX = threshouldAuto[2][0]+threshouldAuto[2][1];

                int color_id = v.Img_id;
                switch(color_id) {

                case 0:// TEAM MAIN COLOR
                    v.HScale_Hmin.set_value(H_MIN);
                    v.HScale_Hmax.set_value(H_MAX);
                    v.HScale_Smin.set_value(S_MIN); //v.HScale_Smax.set_value(S_MAX);
                    v.HScale_Vmin.set_value(V_MIN); //v.HScale_Vmax.set_value(V_MAX);

                    v.H[color_id][0] = H_MIN;
                    v.H[color_id][1] = H_MAX;
                    v.S[color_id][0] = S_MIN; //v.S[color_id][1] = S_MAX;
                    v.V[color_id][0] = V_MIN; //v.V[color_id][1] = V_MAX;
                    break;

                case 1:// TEAM FIRST SECUNDARY COLOR
                    v.HScale_Hmin.set_value(H_MIN);
                    v.HScale_Hmax.set_value(H_MAX);
                    v.HScale_Smin.set_value(S_MIN); //v.HScale_Smax.set_value(S_MAX);
                    v.HScale_Vmin.set_value(V_MIN); //v.HScale_Vmax.set_value(V_MAX);

                    v.H[color_id][0] = H_MIN;
                    v.H[color_id][1] = H_MAX;
                    v.S[color_id][0] = S_MIN; //v.S[color_id][1] = S_MAX;
                    v.V[color_id][0] = V_MIN; //v.V[color_id][1] = V_MAX;
                    break;

                case 2:// TEAM SECOND SECUNDARY COLOR
                    v.HScale_Hmin.set_value(H_MIN);
                    v.HScale_Hmax.set_value(H_MAX);
                    v.HScale_Smin.set_value(S_MIN); //v.HScale_Smax.set_value(S_MAX);
                    v.HScale_Vmin.set_value(V_MIN); //v.HScale_Vmax.set_value(V_MAX);

                    v.H[color_id][0] = H_MIN;
                    v.H[color_id][1] = H_MAX;
                    v.S[color_id][0] = S_MIN; //v.S[color_id][1] = S_MAX;
                    v.V[color_id][0] = V_MIN; //v.V[color_id][1] = V_MAX;
                    break;

                case 3:// TEAM THIRD SECUNDARY COLOR
                    v.HScale_Hmin.set_value(H_MIN);
                    v.HScale_Hmax.set_value(H_MAX);
                    v.HScale_Smin.set_value(S_MIN); //v.HScale_Smax.set_value(S_MAX);
                    v.HScale_Vmin.set_value(V_MIN); //v.HScale_Vmax.set_value(V_MAX);

                    v.H[color_id][0] = H_MIN;
                    v.H[color_id][1] = H_MAX;
                    v.S[color_id][0] = S_MIN; //v.S[color_id][1] = S_MAX;
                    v.V[color_id][0] = V_MIN; //v.V[color_id][1] = V_MAX;
                    break;

                case 4:// BALL
                    v.HScale_Hmin.set_value(H_MIN);
                    v.HScale_Hmax.set_value(H_MAX);
                    v.HScale_Smin.set_value(S_MIN); //v.HScale_Smax.set_value(S_MAX);
                    v.HScale_Vmin.set_value(V_MIN); //v.HScale_Vmax.set_value(V_MAX);

                    v.H[color_id][0] = H_MIN;
                    v.H[color_id][1] = H_MAX;
                    v.S[color_id][0] = S_MIN; // v.S[color_id][1] = S_MAX;
                    v.V[color_id][0] = V_MIN; // v.V[color_id][1] = V_MAX;
                    break;

                case 5:// ADVERSARY MAIN COLOR
                    v.HScale_Hmin.set_value(H_MIN);
                    v.HScale_Hmax.set_value(H_MAX);
                    v.HScale_Smin.set_value(S_MIN);
                    v.HScale_Smax.set_value(S_MAX);
                    v.HScale_Vmin.set_value(V_MIN);
                    v.HScale_Vmax.set_value(V_MAX);

                    v.H[color_id][0] = H_MIN; // v.H[color_id][1] = H_MAX;
                    v.S[color_id][0] = S_MIN; // v.S[color_id][1] = S_MAX;
                    v.V[color_id][0] = V_MIN; // v.V[color_id][1] = V_MAX;
                    break;
                }
                //v.auto_calib_flag = false;
            }

            //cout<<"AQUI"<<endl;

            //TRACKING CAMERA
            parallel_tracking(image);

            if(!v.HSV_calib_event_flag) {
                robot_creation_unitag();
                //robot_creation();

                drawStrategyConstants(image, w, h);

                if (!v.draw_info_flag)
                {
                    circle(image,v.robot_list[0].position, 15, cv::Scalar(255,255,0), 2);
                    line(image,v.robot_list[0].position,v.robot_list[0].secundary,cv::Scalar(255,255,0), 2);
                    //line(image,v.robot_list[0].position,v.robot_list[0].ternary,cv::Scalar(100,255,0), 2);
                    putText(image,"1",cv::Point(v.robot_list[0].position.x-5,v.robot_list[0].position.y-17),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0),2);
                    circle(image,v.robot_list[1].position, 15, cv::Scalar(255,255,0), 2);
                    line(image,v.robot_list[1].position,v.robot_list[1].secundary,cv::Scalar(255,255,0), 2);
                    //line(image,v.robot_list[1].position,v.robot_list[1].ternary,cv::Scalar(100,255,0), 2);
                    putText(image,"2",cv::Point(v.robot_list[1].position.x-5,v.robot_list[1].position.y-17),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0),2);
                    circle(image,v.robot_list[2].position, 15, cv::Scalar(255,255,0), 2);
                    line(image,v.robot_list[2].position,v.robot_list[2].secundary,cv::Scalar(255,255,0), 2);
                    //line(image,v.robot_list[2].position,v.robot_list[2].ternary,cv::Scalar(100,255,0), 2);
                    putText(image,"3",cv::Point(v.robot_list[2].position.x-5,v.robot_list[2].position.y-17),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0),2);
                    circle(image,Ball, 7, cv::Scalar(255,255,255), 2);

                    //PREDIÇÃO DA BOLA
                  /*  double precTick = ticks;
					ticks = (double) cv::getTickCount();
					double dT = (ticks - precTick) / cv::getTickFrequency(); //seconds
					kfBall.transitionMatrix.at<float>(2) = dT;
					kfBall.transitionMatrix.at<float>(9) = dT;
					state = kfBall.predict();
					cv::Point center;
					center.x = state.at<float>(0);
					center.y = state.at<float>(1);
					circle(image,center, 5, cv::Scalar(0,0,255), 2);    */
                   // strategy.strats.set_Ball(Ball);
					//strategy.strats.set_Ball_Est(center);
                    for(int i=0; i<Adv_Main.size(); i++)
                        circle(image,Adv_Main[i], 15, cv::Scalar(0,0,255), 2);
                }


            }

            if(v.iv.PID_test_flag)	 PID_test();
            else {
                for(int i=0; i<v.robot_list.size(); i++) {
                    v.robot_list[i].target=cv::Point(-1,-1);
                }
                Selec_index=-1;
            }

            if(Selec_index!=-1) {
                circle(image,v.robot_list[Selec_index].position, 17, cv::Scalar(255,255,255), 2);
            }

            for(int i=0; i<v.robot_list.size(); i++) {
                if(v.robot_list[i].target.x!=-1&&v.robot_list[i].target.y!=-1)
                    line(image, v.robot_list[i].position,v.robot_list[i].target, cv::Scalar(255,255,255),2);
                circle(image,v.robot_list[i].target, 7, cv::Scalar(255,255,255), 2);
            }

            // ----------- ESTRATEGIA -----------------//
			strategy.strats.set_Ball(Ball);

            /*
            line(image, cv::Point(strategy.strats.LIMITE_AREA_X,strategy.strats.LARGURA_CAMPO/2-strategy.strats.TAMANHO_AREA/2),cv::Point(strategy.strats.LIMITE_AREA_X,strategy.strats.LARGURA_CAMPO/2+strategy.strats.TAMANHO_AREA/2), cv::Scalar(255,255,255),2);
            line(image, cv::Point(strategy.strats.LIMITE_AREA_X,strategy.strats.LARGURA_CAMPO/2-strategy.strats.TAMANHO_AREA/2),cv::Point(0,strategy.strats.LARGURA_CAMPO/2-strategy.strats.TAMANHO_AREA/2), cv::Scalar(255,255,255),2);
            line(image, cv::Point(strategy.strats.LIMITE_AREA_X,strategy.strats.LARGURA_CAMPO/2+strategy.strats.TAMANHO_AREA/2),cv::Point(0,strategy.strats.LARGURA_CAMPO/2+strategy.strats.TAMANHO_AREA/2), cv::Scalar(255,255,255),2);
            line(image, cv::Point(strategy.strats.COMPRIMENTO_CAMPO - round(0.2*float(width)/1.70),0),cv::Point(strategy.strats.COMPRIMENTO_CAMPO - round(0.2*float(width)/1.70),height), cv::Scalar(255,255,255),2);
            line(image, cv::Point(strategy.strats.COMPRIMENTO_CAMPO - round(0.2*float(width)/1.70),strategy.strats.MAX_GOL_Y),cv::Point(width,strategy.strats.MAX_GOL_Y), cv::Scalar(255,255,255),2);
            line(image, cv::Point(strategy.strats.COMPRIMENTO_CAMPO - round(0.2*float(width)/1.70),strategy.strats.MIN_GOL_Y),cv::Point(width,strategy.strats.MIN_GOL_Y), cv::Scalar(255,255,255),2);
            */
            if(v.start_game_flag) {
                Ball_Est=strategy.strats.get_Ball_Est();
                line(image,Ball,Ball_Est,cv::Scalar(255,140,0), 2);
                circle(image,Ball_Est, 7, cv::Scalar(255,140,0), 2);
                char buffer[3];
                for(int i =0; i<3; i++) {
                    switch (v.robot_list[i].role)	{
                    case 0:
                        v.robot_list[i].target = strategy.strats.get_gk_target(Adv_Main);
                        v.robot_list[i].fixedPos = strategy.strats.Goalkeeper.fixedPos;
                        if(strategy.strats.GOAL_DANGER_ZONE) {
                            //	cout<<"hist_wipe"<<endl;
                            v.robot_list[i].histWipe();
                        }
                        break;
                    case 2:
                        v.robot_list[i].target = strategy.strats.get_atk_target(v.robot_list[i].position, v.robot_list[i].orientation);
                        v.robot_list[i].fixedPos = strategy.strats.Attack.fixedPos;
                        v.robot_list[i].status = strategy.strats.Attack.status;
                        /*	for(int j=0;j<Adv_Main.size();j++){
                        		if ( sqrt(pow(Adv_Main[j].x - v.robot_list[i].position.x, 2) + pow(Adv_Main[j].y - v.robot_list[i].position.y, 2)) < 50) {
                        			v.robot_list[i].histWipe();
                        		}
                        	}*/
                        break;
                    case 1:
                        v.robot_list[i].target = strategy.strats.get_def_target(v.robot_list[i].position);
                        v.robot_list[i].fixedPos = strategy.strats.Defense.fixedPos;
                        v.robot_list[i].status = strategy.strats.Defense.status;
                        /*	for(int j=0;j<Adv_Main.size();j++){
                        		if ( sqrt(pow(Adv_Main[j].x - v.robot_list[i].position.x, 2) + pow(Adv_Main[j].y - v.robot_list[i].position.y, 2)) < 50) {
                        			v.robot_list[i].spin = true;
                        		}
                        	}*/
                        break;
                     case 3:
						v.robot_list[i].target = strategy.strats.get_opp_target(v.robot_list[i].position, v.robot_list[i].orientation);
                        v.robot_list[i].fixedPos = strategy.strats.Opponent.fixedPos;
                        v.robot_list[i].status = strategy.strats.Opponent.status;

                     break;
                    }
                    //cout<<v.robot_list[0].target.x<<" - "<<v.robot_list[0].target.y<<endl;
                    circle(image,v.robot_list[i].target, 7, cv::Scalar(127,255,127), 2);

                    putText(image,std::to_string(i+1),cv::Point(v.robot_list[i].target.x-5,v.robot_list[i].target.y-17),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(127,255,127),2);
                }
            }
            //cout<<v.robot_list[2].status<<" | "<<v.robot_list[2].V<<endl;

            v.update_speed_progressBars();
            send_vel_to_robots();
            // ----------------------------------------//


            //PRINT RAW POSITIONS
            /*	 cout<<"--------------------------------------------------------------- "<<endl;
            	 cout<<"Team Size "<<Team_Main.size()<<endl;
            	 for(int i=0;i<Team_Main.size();i++){
            		cout<<i<<" "<<Team_Main[i].x<<" "<<Team_Main[i].y<<" ";
            }
            cout<<endl;
            	 cout<<"Sec Size "<<Team_Sec[0].size()<<" "<<Team_Sec[1].size()<<" "<<Team_Sec[2].size()<<endl;
            	for(int j=0;j<Team_Sec.size();j++){
            		cout<<"TS "<<j<<" ";

            	for(int i=0;i<Team_Sec[j].size();i++)
            		cout<<i<<" "<<Team_Sec[j][i].x<<" "<<Team_Sec[j][i].y<<" ";

            		cout<<endl;
            }
            		 cout<<"Advs Size "<<Adv_Main.size()<<endl;
            	 for(int i=0;i<Adv_Main.size();i++){
            		cout<<i<<" "<<Adv_Main[i].x<<" "<<Adv_Main[i].y<<" ";
            }
            	cout<<endl;
            		cout<<"BALL "<<Ball.x<<" "<<Ball.y<<endl;
            cout<<"--------------------------------------------------------------- "<<endl;

            		//timer.stop();
            		//cout<<"Time: "<<timer.getCPUTotalSecs()<<"	FPS: "<<1/timer.getCPUTotalSecs()<<endl;
            		//timer.reset();
            		* */

            if(v.HSV_calib_event_flag) {
                for(int i=0; i<3*(width*height + width) +2; i++)
                    d[i]=threshold[v.Img_id][i];
            }
            return true;
        }

        void send_vel_to_robots() {
            for(int i=0; i<v.robot_list.size(); i++) {
                if(v.robot_list[i].target.x!=-1&&v.robot_list[i].target.y!=-1) {
                    v.robot_list[i].goTo(v.robot_list[i].target,Ball);
                } else {
                    v.robot_list[i].Vr = 0 ;
                    v.robot_list[i].Vl = 0 ;
                }
            }
            control.s.sendToThree(v.robot_list[0],v.robot_list[1],v.robot_list[2]);
        }

        void PID_test() {
            double dist;
            int old_Selec_index;
            old_Selec_index = Selec_index;
            for(int i=0; i<v.robot_list.size(); i++) {
                dist = sqrt(pow((v.iv.robot_pos[0]-v.robot_list[i].position.x),2)+pow((v.iv.robot_pos[1]-v.robot_list[i].position.y),2));
                if(dist<=17) {
                    Selec_index=i;
                    v.iv.tar_pos[0] = -1;
                    v.iv.tar_pos[1] = -1;
                    v.robot_list[Selec_index].target=cv::Point(-1,-1);
                    fixed_ball[Selec_index]=false;
                }
            }
            if(Selec_index>-1) {
                v.robot_list[Selec_index].histWipe();
                if(sqrt(pow((Ball.x-v.robot_list[Selec_index].target.x),2)+pow((Ball.y-v.robot_list[Selec_index].target.y),2))<=7)
                    fixed_ball[Selec_index]=true;


                if(fixed_ball[Selec_index])
                    v.robot_list[Selec_index].target=Ball;
                else
                    v.robot_list[Selec_index].target = cv::Point(v.iv.tar_pos[0],v.iv.tar_pos[1]);
            }


            for(int i=0; i<v.robot_list.size(); i++) {
                if(fixed_ball[i])
                    v.robot_list[i].target=Ball;
                else {
                    if(sqrt(pow((v.robot_list[i].position.x-v.robot_list[i].target.x),2)+
                            pow((v.robot_list[i].position.y-v.robot_list[i].target.y),2))<15) {

                        v.robot_list[i].target = cv::Point(-1,-1);
                        v.iv.tar_pos[0]=-1;
                        v.iv.tar_pos[1]=-1;
                        v.robot_list[i].Vr = 0 ;
                        v.robot_list[i].Vl = 0 ;
                    }
                    if(v.robot_list[i].target.x!=-1&&v.robot_list[i].target.y!=-1) {
                        v.robot_list[i].goTo(v.robot_list[i].target,Ball);
                    } else {
                        v.robot_list[i].Vr = 0 ;
                        v.robot_list[i].Vl = 0 ;
                    }

                }

            }


        }

        void drawStrategyConstants(cv::Mat image, int w, int h)
        {
          if (strategy.get_deslocamentoZagaAtaque_flag())
            line(image,cv::Point(strategy.strats.DESLOCAMENTO_ZAGA_ATAQUE, 0),cv::Point(strategy.strats.DESLOCAMENTO_ZAGA_ATAQUE, h),cv::Scalar(255,255,0), 2);

          if (strategy.get_goalSize_flag())
            line(image,cv::Point(strategy.strats.TAMANHO_GOL, h/2-strategy.strats.TAMANHO_GOL/2),cv::Point(strategy.strats.TAMANHO_GOL, h/2+strategy.strats.TAMANHO_GOL/2),cv::Scalar(255,255,0), 2);

          if (strategy.get_fieldWwidth_flag())
            line(image,cv::Point(0, h/2),cv::Point(strategy.strats.LARGURA_CAMPO, h/2),cv::Scalar(255,255,0), 2);

          if (strategy.get_totalFieldLength_flag())
            line(image,cv::Point(w/2, 0),cv::Point(w/2, strategy.strats.COMPRIMENTO_CAMPO_TOTAL),cv::Scalar(255,255,0), 2);

          if (strategy.get_fieldLength_flag())
            line(image,cv::Point(round(0.10*float(h)/1.70), h/2),cv::Point(strategy.strats.COMPRIMENTO_PISTA, h/2),cv::Scalar(255,255,0), 2);

          if (strategy.get_coneRatio_flag())
            line(image,cv::Point(strategy.strats.CONE_RATIO, 0),cv::Point(strategy.strats.CONE_RATIO, h),cv::Scalar(255,255,0), 2);

          if (strategy.get_offsetRatio_flag())
            line(image,cv::Point(strategy.strats.OFFSET_RATIO, 0),cv::Point(strategy.strats.OFFSET_RATIO, h),cv::Scalar(255,255,0), 2);

          if (strategy.get_defenseLine_flag())
            line(image,cv::Point(strategy.strats.LINHA_ZAGA, 0),cv::Point(strategy.strats.LINHA_ZAGA, h),cv::Scalar(255,255,0), 2);

          if (strategy.get_goalMax_flag())
            putText(image, "X", cv::Point(w-strategy.strats.COMPRIMENTO_PISTA, strategy.strats.MEIO_GOL_Y+strategy.strats.TAMANHO_GOL/2),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0), 2);

          if (strategy.get_goalMin_flag())
            putText(image, "X", cv::Point(w-strategy.strats.COMPRIMENTO_PISTA, strategy.strats.MEIO_GOL_Y-strategy.strats.TAMANHO_GOL/2),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0), 2);

          if (strategy.get_goalCenter_flag())
            putText(image, "X", cv::Point(strategy.strats.MEIO_GOL_X, strategy.strats.MEIO_GOL_Y),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0), 2);

          if (strategy.get_banheira_flag())
            line(image,cv::Point(strategy.strats.BANHEIRA, 0),cv::Point(strategy.strats.BANHEIRA, h),cv::Scalar(255,255,0), 2);

          if (strategy.get_banheiraOffset_flag())
            line(image,cv::Point(strategy.strats.OFFSET_BANHEIRA, 0),cv::Point(strategy.strats.OFFSET_BANHEIRA, h),cv::Scalar(255,255,0), 2);

          if (strategy.get_areasDivision_flag())
            line(image,cv::Point(strategy.strats.DIVISAO_AREAS, 0),cv::Point(strategy.strats.DIVISAO_AREAS, h),cv::Scalar(255,255,0), 2);

          if (strategy.get_areaSize_flag())
            line(image,cv::Point(strategy.strats.TAMANHO_AREA, 0),cv::Point(strategy.strats.TAMANHO_AREA, h),cv::Scalar(255,255,0), 2);

          if (strategy.get_areaLimitX_flag())
            line(image,cv::Point(strategy.strats.LIMITE_AREA_X, 0),cv::Point(strategy.strats.LIMITE_AREA_X, h),cv::Scalar(255,255,0), 2);
        }

        void robot_creation_unitag() {

            vector <Robot> robot;
            Robot r;
            double omax = -99999;
            double omin = 99999;
            //cout<<"1"<<endl;
            cv::Point secundary;
            int index1[2] = {0,0};
            int index2[2] = {0,0};
            float sx,sy,px,py,tx,ty;
            int l=0;
            float distanceRef1 = 999999999.0;
            float distanceRef2 = 999999999.0;
            float distance = 0;
            //cout<<"2"<<endl;
            for(int j = 0; j < Team_Main.size()&&j<=3; j++) {
                robot.push_back(r);
                distanceRef1 = 999999999.0;
                distanceRef2 = 999999999.0;
                for(int i = 0; i < 1; i++) {
                    for(int k = 0; k < Team_Sec[i].size(); k++) {

                        distance = calcDistance(Team_Main[j],Team_Sec[i][k]);

                        if(distance <= distanceRef1) {
                            distanceRef2 = distanceRef1;
                            index2[0] = index1[0];
                            index2[1] = index1[1];

                            distanceRef1 = distance;
                            index1[0] = i;
                            index1[1] = k;

                        } else if(distance < distanceRef2) {
                            distanceRef2 = distance;
                            index2[0] = i;
                            index2[1] = k;
                        }
                    }
                }
                //cout<<"3"<<endl;
                robot[l].position = Team_Main[j];
                //cout<<"3.1"<<endl;
                //cout<<"AMARELO: "<<j<<"| AREA 1: "<<Team_Sec_area[index1[0]][index1[1]]<<"| AREA 2: "<<Team_Sec_area[index2[0]][index2[1]]<<endl;
                if (!Team_Sec_area[0].empty())
                {
                  if(Team_Sec_area[index1[0]][index1[1]]>Team_Sec_area[index2[0]][index2[1]]) {
                      robot[l].secundary = Team_Sec[index1[0]][index1[1]];
                      robot[l].ternary =  Team_Sec[index2[0]][index2[1]];

                      //cout<<"Area 1 = Secundary"<<"	Area 2 = Ternary"<<endl;
                  } else {
                      robot[l].secundary = Team_Sec[index2[0]][index2[1]];
                      robot[l].ternary = Team_Sec[index1[0]][index1[1]];
                      //cout<<"Area 2 = Secundary"<<"	Area 1 = Ternary"<<endl;
                  }
                }


                //cout<<"3.2"<<endl;


                sx = robot[l].secundary.x;
                sy =  robot[l].secundary.y;

                px = robot[l].position.x;
                py = robot[l].position.y;

                tx = robot[l].ternary.x;
                ty =  robot[l].ternary.y;


                robot[l].orientation = atan2((sy-py)*1.3/480,(sx-px)*1.5/640);
                robot[l].orientation2 = atan2((ty-py)*1.3/480,(tx-px)*1.5/640);
                l++;

            }
            //cout<<"4"<<endl;
            double o = 0;
            int max;
            int min;
            int medio;
            for(int i=0; i<l; i++) {
                o = atan2(sin(robot[i].orientation2-robot[i].orientation+3.1415),cos(robot[i].orientation2-robot[i].orientation+3.1415));

                if(omax<o) {
                    omax = o;
                    v.robot_list[0].position = robot[i].position; // colocar em um vetor

                    v.robot_list[0].secundary = robot[i].secundary; // colocar em um vetor
                    v.robot_list[0].orientation =  robot[i].orientation;

                    max=i;
                }
            }
            for(int i=0; i<l; i++) {
                o = atan2(sin(robot[i].orientation2-robot[i].orientation+3.1415),cos(robot[i].orientation2-robot[i].orientation+3.1415));

                if(omin>o) {
                    omin = o;
                    v.robot_list[1].position = robot[i].position; // colocar em um vetor

                    v.robot_list[1].secundary = robot[i].secundary; // colocar em um vetor
                    v.robot_list[1].orientation =  robot[i].orientation;


                    min=i;
                }
            }
            for(int i=0; i<l; i++) {
                if(i!=min&&i!=max) {
                    v.robot_list[2].position = robot[i].position; // colocar em um vetor

                    v.robot_list[2].secundary = robot[i].secundary; // colocar em um vetor
                    v.robot_list[2].orientation =  robot[i].orientation;
                    medio=i;
                }
            }
            //cout<<endl;
            //cout<<"robo1  "<<min<<" robo2 "<<max<<" robo3 "<<medio<<endl;




            //cout<<"5"<<endl;
            v.robot_list[0].feedHist(v.robot_list[0].position);
            v.robot_list[1].feedHist(v.robot_list[1].position);
            v.robot_list[2].feedHist(v.robot_list[2].position);
            //cout<<"6"<<endl;
            // Atualizar as labels de posição dos robos

            stringstream aux1;
            aux1 << "(" << round((v.robot_list[0].position.x))<< "," << round((v.robot_list[0].position.y))<< "," << round(v.robot_list[0].orientation*(180/PI)) << ")";
            v.robot1_pos_lb->set_text(aux1.str());

            stringstream aux2;
            aux2 << "(" << round((v.robot_list[1].position.x))<< "," << round((v.robot_list[1].position.y))<< "," << round((v.robot_list[1].orientation*(180/PI))) << ")";
            v.robot2_pos_lb->set_text(aux2.str());

            stringstream aux3;
            aux3 << "(" << round((v.robot_list[2].position.x))<< "," << round((v.robot_list[2].position.y)) << "," <<  round((v.robot_list[2].orientation*(180/PI))) << ")";

            v.robot3_pos_lb->set_text(aux3.str());
            stringstream aux4;
            aux4 << "(" << round((Ball.x))<< "," << round((Ball.y)) << ")";
            v.ball_pos_lb->set_text(aux4.str());
        }

        void robot_creation() {
            Robot robot;
            cv::Point secundary;
            int index[2];


            float distanceRef = 999999999.0;
            float distance = 0;
            for(int j = 0; j < Team_Main.size()&&j<=3; j++) {
                for(int i = 0; i < 3; i++) {
                    for(int k = 0; k < Team_Sec[i].size(); k++) {

                        distance = calcDistance(Team_Main[j],Team_Sec[i][k]);
                        if(distance < distanceRef) {
                            distanceRef = distance;
                            index[0] = i;
                            index[1] = k;
                        }
                    }
                }

                robot.position = Team_Main[j];
                robot.secundary = Team_Sec[index[0]][index[1]];
                distanceRef = 999999999.0;
                v.robot_list[index[0]].position = robot.position; // colocar em um vetor
                v.robot_list[index[0]].feedHist(robot.position);
                v.robot_list[index[0]].secundary = robot.secundary; // colocar em um vetor
                calcOrientation(index[0]);


            }
            // Atualizar as labels de posição dos robos

            stringstream aux1;
            aux1 << "(" << round((v.robot_list[0].position.x))<< "," << round((v.robot_list[0].position.y))<< "," << round(v.robot_list[0].orientation*(180/PI)) << ")";
            v.robot1_pos_lb->set_text(aux1.str());

            stringstream aux2;
            aux2 << "(" << round((v.robot_list[1].position.x))<< "," << round((v.robot_list[1].position.y))<< "," << round((v.robot_list[1].orientation*(180/PI))) << ")";
            v.robot2_pos_lb->set_text(aux2.str());

            stringstream aux3;
            aux3 << "(" << round((v.robot_list[2].position.x))<< "," << round((v.robot_list[2].position.y)) << "," <<  round((v.robot_list[2].orientation*(180/PI))) << ")";

            v.robot3_pos_lb->set_text(aux3.str());
            stringstream aux4;
            aux4 << "(" << round((Ball.x))<< "," << round((Ball.y)) << ")";
            v.ball_pos_lb->set_text(aux4.str());
        }

        void calcOrientation(int tag_id) { //Define a orientação da tag em analise;
            float sx,sy,px,py;

            sx =  v.robot_list[tag_id].secundary.x;
            sy =  v.robot_list[tag_id].secundary.y;

            px = v.robot_list[tag_id].position.x;
            py = v.robot_list[tag_id].position.y;

            v.robot_list[tag_id].orientation = atan2((sy-py)*1.3/480,(sx-px)*1.5/640);
            v.robot_list[tag_id].position.x = v.robot_list[tag_id].position.x;
            v.robot_list[tag_id].position.y = v.robot_list[tag_id].position.y;
        }

        float calcDistance(cv::Point position, cv::Point secundary) {
            return sqrt(pow(position.x-secundary.x,2) + pow(position.y-secundary.y,2));
        }

        void parallel_tracking(cv::Mat im) {
            cv::Mat image_copy = im.clone();
            cv::cvtColor(image_copy,image_copy,cv::COLOR_RGB2HSV);
            cv::medianBlur(image_copy, image_copy, 5);
            Team_Sec_area.clear();
            vector< double > a;
            Team_Sec_area.push_back(a);
            Team_Sec_area.push_back(a);
            Team_Sec_area.push_back(a);

            for(int i =0; i<6; i++) {

                threshold_threads.add_thread(new boost::thread(&CamCap::img_tracking,this, boost::ref(image_copy), (i)));
            }

            threshold_threads.join_all();
            image_copy.release();
        }

        void img_tracking(cv::Mat image,int color_id) {
            int ec,e3c,H,S,V;
            vector< vector<cv::Point> > contours;
            vector<cv::Vec4i> hierarchy;


            for(int i =0; i<image.rows; i++) { //Threshold calculations
                for(int j =0; j<image.cols; j++) {
                    ec = image.cols*i + j;
                    e3c = ec*3;

                    H = image.data[e3c];
                    S = image.data[e3c + 1];
                    V = image.data[e3c + 2];

                    if((H>=v.H[color_id][0]&&H<=v.H[color_id][1])&&
                            (S>=v.S[color_id][0]&&S<=v.S[color_id][1])&&
                            (V>=v.V[color_id][0]&&V<=v.V[color_id][1])) {

                        threshold[color_id][e3c] = 255;
                        threshold[color_id][e3c+1] = 255;
                        threshold[color_id][e3c+2] = 255;
                    } else {
                        threshold[color_id][e3c] = 0;
                        threshold[color_id][e3c+1] = 0;
                        threshold[color_id][e3c+2] = 0;
                    }
                }
            }
            cv::Mat temp(height,width,CV_8UC3,threshold[color_id]);
            cv::cvtColor(temp,temp,cv::COLOR_RGB2GRAY);
            cv::findContours(temp,contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);


            switch(color_id) {

            case 0:// TEAM MAIN COLOR

                if (hierarchy.size() > 0) {
                    Team_Main.clear();
                    int index = 0;
                    while(index >= 0) {
                        cv::Moments moment = moments((cv::Mat)contours[index]);
                        double area = contourArea(contours[index]);
                        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
                        if(area >= v.Amin[color_id]/100) {
                            Team_Main.push_back(cv::Point(moment.m10/area,moment.m01/area));
                        }
                        index = hierarchy[index][0];
                    }

                }

                break;


            case 1:// TEAM FIRST SECUNDARY COLOR
                if (hierarchy.size() > 0) {
                    Team_Sec[0].clear();
                    int index = 0;
                    while(index >= 0) {
                        cv::Moments moment = moments((cv::Mat)contours[index]);
                        double area = contourArea(contours[index]);
                        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
                        if(area >= v.Amin[color_id]/100) {
                            Team_Sec[0].push_back(cv::Point(moment.m10/area,moment.m01/area));
                            Team_Sec_area[0].push_back(area);
                        }
                        index = hierarchy[index][0];
                    }
                }


                break;

            case 2:// TEAM SECOND SECUNDARY COLOR
                if (hierarchy.size() > 0) {
                    Team_Sec[1].clear();
                    int index = 0;
                    while(index >= 0) {
                        cv::Moments moment = moments((cv::Mat)contours[index]);
                        double area = contourArea(contours[index]);
                        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
                        if(area >= v.Amin[color_id]/100) {
                            Team_Sec[1].push_back(cv::Point(moment.m10/area,moment.m01/area));
                            Team_Sec_area[1].push_back(area);

                        }
                        index = hierarchy[index][0];
                    }
                }
                break;
            case 3:// TEAM THIRD SECUNDARY COLOR
                if (hierarchy.size() > 0) {
                    Team_Sec[2].clear();
                    int index = 0;
                    while(index >= 0) {
                        cv::Moments moment = moments((cv::Mat)contours[index]);
                        double area = contourArea(contours[index]);
                        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
                        if(area >= v.Amin[color_id]/100) {
                            Team_Sec[2].push_back(cv::Point(moment.m10/area,moment.m01/area));
                            Team_Sec_area[2].push_back(area);
                        }
                        index = hierarchy[index][0];
                    }
                }
                break;

            case 5:// ADVERSARY MAIN COLOR

                if (hierarchy.size() > 0) {
                    Adv_Main.clear();
                    int j =0;
                    int index = 0;
                    while(index >= 0) {
                        cv::Moments moment = moments((cv::Mat)contours[index]);
                        double area = contourArea(contours[index]);
                        //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
                        if(area >= v.Amin[color_id]/100) {
                            Adv_Main.push_back(cv::Point(moment.m10/area,moment.m01/area));
                        }
                        index = hierarchy[index][0];
                    }

                }

                break;

            case 4:// BALL
                if (hierarchy.size() > 0) {
                    cv::Moments moment = moments((cv::Mat)contours[0]);
                    double area = contourArea(contours[0]);
                    //Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.


                    if(area >= v.Amin[color_id]/100) {
                        Ball = cv::Point(moment.m10/area,moment.m01/area);

                       /* kfBall.statePost = state;

                        notFoundCount = 0;

						meas.at<float>(0) = Ball.x;
						meas.at<float>(1) = Ball.y;
						meas.at<float>(2) = (float) 5;
						meas.at<float>(3) = (float) 5;

						if (!found) // First detection!
						{
							// >>>> Initialization
							kfBall.errorCovPre.at<float>(0) = 1; // px
							kfBall.errorCovPre.at<float>(7) = 1; // px
							kfBall.errorCovPre.at<float>(14) = 1;
							kfBall.errorCovPre.at<float>(21) = 1;
							kfBall.errorCovPre.at<float>(28) = 1; // px
							kfBall.errorCovPre.at<float>(35) = 1; // px

							state.at<float>(0) = meas.at<float>(0);
							state.at<float>(1) = meas.at<float>(1);
							state.at<float>(2) = 0;
							state.at<float>(3) = 0;
							state.at<float>(4) = meas.at<float>(2);
							state.at<float>(5) = meas.at<float>(3);
							// <<<< Initialization

							found = true;
						}
						else
							kfBall.correct(meas); // Kalman Correction

                    //}else{
						//notFoundCount++;
						//cout << "notFoundCount:" << notFoundCount << endl;
						//if( notFoundCount >= 10 ){
						//	found = false;
						//}
					*/}
                }
                break;

            }
        }

        void warp_transform(cv::Mat image){
            cv::Point2f inputQuad[4];
            cv::Point2f outputQuad[4];
            cv::Mat lambda = cv::Mat::zeros( image.rows, image.cols, image.type() );

            inputQuad[0] = cv::Point2f( v.iv.warp_mat[0][0]-v.offsetL,v.iv.warp_mat[0][1]);
            inputQuad[1] = cv::Point2f( v.iv.warp_mat[1][0]+v.offsetR,v.iv.warp_mat[1][1]);
            inputQuad[2] = cv::Point2f( v.iv.warp_mat[2][0]+v.offsetR,v.iv.warp_mat[2][1]);
            inputQuad[3] = cv::Point2f( v.iv.warp_mat[3][0]-v.offsetL,v.iv.warp_mat[3][1]);

            outputQuad[0] = cv::Point2f( 0,0 );
            outputQuad[1] = cv::Point2f( w-1,0);
            outputQuad[2] = cv::Point2f( w-1,h-1);
            outputQuad[3] = cv::Point2f( 0,h-1  );
            lambda = getPerspectiveTransform( inputQuad, outputQuad );
            warpPerspective(image,image,lambda,image.size());
            if(v.iv.adjust_rdy) {
                v.bt_adjust.set_active(false);
                v.bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
                v.adjust_event_flag = false;
                v.iv.adjust_event_flag = false;
                for(int i =0; i<v.iv.adjust_mat[0][1]; i++) {
                    for(int j =0; j<3*v.iv.adjust_mat[0][0]; j++) {
                        image.at<uchar>(i, j) =0;
                    }
                }

                for(int i = height; i>v.iv.adjust_mat[1][1]; i--) {
                    for(int j =0; j<3*v.iv.adjust_mat[1][0]; j++) {
                        image.at<uchar>(i, j) =0;
                    }
                }

                for(int i =0; i<v.iv.adjust_mat[2][1]; i++) {
                    for(int j =3*width; j>3*v.iv.adjust_mat[2][0]; j--) {
                        image.at<uchar>(i, j) =0;
                    }
                }

                for(int i =height; i>v.iv.adjust_mat[3][1]; i--) {
                    for(int j =3*width; j>3*v.iv.adjust_mat[3][0]; j--) {
                        image.at<uchar>(i, j) =0;
                    }
                }

            }
        }

        CamCap() : data(0), width(0), height(0){

            fixed_ball[0]=false;
            fixed_ball[1]=false;
            fixed_ball[2]=false;
            fm.set_label("Image");
            fm.add(v.iv);
            notebook.append_page(v, "Vision");
            notebook.append_page(control, "Control");
            notebook.append_page(strategy, "Strategy");

            vector< double > a;
            Team_Sec_area.push_back(a);
            Team_Sec_area.push_back(a);
            Team_Sec_area.push_back(a);

            vector< cv::Point > p;


            p.push_back(cv::Point(0,0));
            Team_Sec.push_back(p);
            Team_Sec.push_back(p);
            Team_Sec.push_back(p);

            Team_Main.push_back(cv::Point(0,0));
            Team_Main.push_back(cv::Point(0,0));
            Team_Main.push_back(cv::Point(0,0));
            for(int i=0; i<4; i++) {
                v.iv.adjust_mat[i][0] = -1;
                v.iv.adjust_mat[i][1] = -1;
            }




            camera_vbox.pack_start(fm, false, true, 10);
            camera_vbox.pack_start(info_fm, false, true, 10);
            info_fm.add(v.info_hbox);

            pack_start(camera_vbox, true, true, 10);
            pack_start(notebook, false, false, 10);

            v.signal_start().connect(sigc::mem_fun(*this, &CamCap::start_signal));
        }

        ~CamCap(){

            con.disconnect();
            v.iv.disable_image_show();
            free(data);
            if (threshold != NULL)
            {
                for(int i = 0; i < 6; i++)
                    free(threshold[i]);
                free(threshold);
            }

            data = 0;
        }

};

#endif /* CAMCAP_HPP_ */
