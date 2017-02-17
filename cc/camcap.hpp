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
#include "vision.hpp"
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

Vision *vision;

class CamCap:


    public Gtk::HBox {
    public:

        cv::Mat image_copy;
        cv::Point Ball_Est;

        StrategyGUI strategy;
        ControlGUI control;
        capture::V4LInterface v;

        unsigned char * d;

        Gtk::Notebook notebook;
        int w, h;
        CPUTimer timer;




        Gtk::Frame fm;
        Gtk::Frame info_fm;
        Gtk::VBox camera_vbox;
        sigc::connection con;
        unsigned char * data;
        int width, height;
        int Selec_index=-1;
        bool fixed_ball[3];

        void updateAllPositions()
        {
          //  cout<<"AQUI"<<endl;
          vision->robot_creation_unitag();
          //vision->robot_creation();
        //    cout<<"AQUI"<<endl;
          for (int i = 0; i < vision->getRobotListSize(); i++)
          {
            v.robot_list[i].position = vision->getRobotFromList(i).position;
            v.robot_list[i].orientation = vision->getRobotFromList(i).orientation;
              v.robot_list[i].secundary = vision->getRobotFromList(i).secundary;
          }
      //    cout<<"AQUI"<<endl;
          v.ballX = vision->getBallPosition().x;
          v.ballY = vision->getBallPosition().y;

          v.robot_list[0].feedHist(v.robot_list[0].position);
          v.robot_list[1].feedHist(v.robot_list[1].position);
          v.robot_list[2].feedHist(v.robot_list[2].position);

          v.updateRobotLabels();
        }

        bool start_signal(bool b) {

            if (b) {

                cout << "Start Clicked!" << endl;

                if (data) {
                    v.iv.disable_image_show();
                    free(data);
                    data = 0;
                }


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

                vision = new Vision(width, height);

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




            vision->setHSV(v.H,v.S,v.V,v.Amin);
            //TRACKING CAMERA


            vision->parallel_tracking(image);



            if(!v.HSV_calib_event_flag) {
              updateAllPositions();

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
                    circle(image,vision->getBallPosition(), 7, cv::Scalar(255,255,255), 2);

                    for(int i=0; i<vision->Adv_Main.size(); i++)
                        circle(image,vision->Adv_Main[i], 15, cv::Scalar(0,0,255), 2);
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
		       	strategy.strats.set_Ball(vision->getBallPosition());

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
                line(image,vision->getBallPosition(),Ball_Est,cv::Scalar(255,140,0), 2);
                circle(image,Ball_Est, 7, cv::Scalar(255,140,0), 2);
                char buffer[3];
                for(int i =0; i<3; i++) {
                    switch (v.robot_list[i].role)	{
                    case 0:
                        v.robot_list[i].target = strategy.strats.get_gk_target(vision->Adv_Main);
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
                        /*	for(int j=0;j<vision->Adv_Main.size();j++){
                        		if ( sqrt(pow(vision->Adv_Main[j].x - v.robot_list[i].position.x, 2) + pow(vision->Adv_Main[j].y - v.robot_list[i].position.y, 2)) < 50) {
                        			v.robot_list[i].histWipe();
                        		}
                        	}*/
                        break;
                    case 1:
                        v.robot_list[i].target = strategy.strats.get_def_target(v.robot_list[i].position);
                        v.robot_list[i].fixedPos = strategy.strats.Defense.fixedPos;
                        v.robot_list[i].status = strategy.strats.Defense.status;
                        /*	for(int j=0;j<vision->Adv_Main.size();j++){
                        		if ( sqrt(pow(vision->Adv_Main[j].x - v.robot_list[i].position.x, 2) + pow(vision->Adv_Main[j].y - v.robot_list[i].position.y, 2)) < 50) {
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
            		 cout<<"Advs Size "<<vision->Adv_Main.size()<<endl;
            	 for(int i=0;i<vision->Adv_Main.size();i++){
            		cout<<i<<" "<<vision->Adv_Main[i].x<<" "<<vision->Adv_Main[i].y<<" ";
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
                    d[i]=vision->threshold[v.Img_id][i];
            }
            return true;
        }

        void send_vel_to_robots() {
            for(int i=0; i<v.robot_list.size(); i++) {
                if(v.robot_list[i].target.x!=-1&&v.robot_list[i].target.y!=-1) {
                    v.robot_list[i].goTo(v.robot_list[i].target,vision->getBallPosition());
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
                if(sqrt(pow((vision->getBallPosition().x-v.robot_list[Selec_index].target.x),2)+pow((vision->getBallPosition().y-v.robot_list[Selec_index].target.y),2))<=7)
                    fixed_ball[Selec_index]=true;


                if(fixed_ball[Selec_index])
                    v.robot_list[Selec_index].target=vision->getBallPosition();
                else
                    v.robot_list[Selec_index].target = cv::Point(v.iv.tar_pos[0],v.iv.tar_pos[1]);
            }


            for(int i=0; i<v.robot_list.size(); i++) {
                if(fixed_ball[i])
                    v.robot_list[i].target=vision->getBallPosition();
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
                        v.robot_list[i].goTo(v.robot_list[i].target,vision->getBallPosition());
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
          {
          line(image, cv::Point(strategy.strats.LIMITE_AREA_X,strategy.strats.LARGURA_CAMPO/2-strategy.strats.TAMANHO_AREA/2),cv::Point(strategy.strats.LIMITE_AREA_X,strategy.strats.LARGURA_CAMPO/2+strategy.strats.TAMANHO_AREA/2), cv::Scalar(255,255,255),2);
        line(image, cv::Point(strategy.strats.LIMITE_AREA_X,strategy.strats.LARGURA_CAMPO/2-strategy.strats.TAMANHO_AREA/2),cv::Point(0,strategy.strats.LARGURA_CAMPO/2-strategy.strats.TAMANHO_AREA/2), cv::Scalar(255,255,255),2);
         line(image, cv::Point(strategy.strats.LIMITE_AREA_X,strategy.strats.LARGURA_CAMPO/2+strategy.strats.TAMANHO_AREA/2),cv::Point(0,strategy.strats.LARGURA_CAMPO/2+strategy.strats.TAMANHO_AREA/2), cv::Scalar(255,255,255),2);

          line(image, cv::Point(strategy.strats.Ball.x, strategy.strats.Ball.y - 100),cv::Point(strategy.strats.Ball.x, strategy.strats.Ball.y + 100), cv::Scalar(255,0,0),2);
            line(image, cv::Point(strategy.strats.Ball),cv::Point(strategy.strats.Ball.x - 100, strategy.strats.Ball.y), cv::Scalar(255,0,0),2);

              line(image, cv::Point(strategy.strats.COMPRIMENTO_CAMPO_TOTAL - strategy.strats.LIMITE_AREA_X,0),cv::Point(strategy.strats.COMPRIMENTO_CAMPO_TOTAL - strategy.strats.LIMITE_AREA_X,strategy.strats.MIN_GOL_Y), cv::Scalar(255,255,255),2);
              line(image, cv::Point(strategy.strats.COMPRIMENTO_CAMPO_TOTAL - strategy.strats.LIMITE_AREA_X,height),cv::Point(strategy.strats.COMPRIMENTO_CAMPO_TOTAL - strategy.strats.LIMITE_AREA_X,strategy.strats.MAX_GOL_Y), cv::Scalar(255,255,255),2);
              line(image, cv::Point(strategy.strats.COMPRIMENTO_CAMPO_TOTAL - strategy.strats.LIMITE_AREA_X,strategy.strats.MAX_GOL_Y),cv::Point(width,strategy.strats.MAX_GOL_Y), cv::Scalar(255,255,255),2);
              line(image, cv::Point(strategy.strats.COMPRIMENTO_CAMPO_TOTAL - strategy.strats.LIMITE_AREA_X,strategy.strats.MIN_GOL_Y),cv::Point(width,strategy.strats.MIN_GOL_Y), cv::Scalar(255,255,255),2);

              line(image,cv::Point(strategy.strats.DESLOCAMENTO_ZAGA_ATAQUE, 0),cv::Point(strategy.strats.DESLOCAMENTO_ZAGA_ATAQUE, h),cv::Scalar(255,255,0), 2);
          }

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

            data = 0;
        }

};

#endif /* CAMCAP_HPP_ */
