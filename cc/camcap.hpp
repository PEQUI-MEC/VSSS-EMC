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
#include "vision/vision.hpp"
#include "Kalman_Filter.hpp"
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



class CamCap:


public Gtk::HBox {
public:
    int width, height;
    int Selec_index=-1;
    int fps_average = 0;
    int timerCounter = 0;
    CPUTimer timer;

    bool fixed_ball[3];
    bool KF_FIRST = true;

    unsigned char * data;

    int frameCounter;
    double ticks = 0;
    vector<cv::Point> robot_kf_est;
    vector< KalmanFilter > KF_RobotBall;

    cv::Point Ball_Est;
    cv::Point Ball_kf_est;

    cv::Point virtual_robots_positions[3];
    float virtual_robots_orientations[3];
    int virtual_robot_selected = -1;

    StrategyGUI strategyGUI;
    ControlGUI control;
    capture::V4LInterface interface;

    Gtk::Frame fm;
    Gtk::Frame info_fm;
    Gtk::VBox camera_vbox;
    Gtk::Notebook notebook;

    boost::thread_group threshold_threads;
    sigc::connection con;

    void updateAllPositions()
    {
        Robot robot;
        cv::Point ballPosition;

        for (int i = 0; i < interface.visionGUI.vision->getRobotListSize(); i++)
        {
            robot = interface.visionGUI.vision->getRobot(i);
            interface.robot_list[i].position = robot.position;
            interface.robot_list[i].orientation = robot.orientation;
            interface.robot_list[i].secundary = robot.secundary;
        }

        ballPosition = interface.visionGUI.vision->getBall();
        interface.ballX = ballPosition.x;
        interface.ballY = ballPosition.y;

        interface.robot_list[0].feedHist(interface.robot_list[0].position);
        interface.robot_list[1].feedHist(interface.robot_list[1].position);
        interface.robot_list[2].feedHist(interface.robot_list[2].position);

        interface.updateRobotLabels();
        interface.updateFPS(fps_average);


        // KALMAN FILTER
        if(KF_FIRST) {
            //KALMAN FILTER INIT
            for(int i=0; i<3; i++) {
                KF_RobotBall[i].KF_init(interface.visionGUI.vision->getRobotPos(i));
            }
            KF_RobotBall[3].KF_init(interface.visionGUI.vision->getBall());
            KF_FIRST = false;
        }

        robot_kf_est[0] = KF_RobotBall[0].KF_Prediction(interface.visionGUI.vision->getRobotPos(0));
        robot_kf_est[1] = KF_RobotBall[1].KF_Prediction(interface.visionGUI.vision->getRobotPos(1));
        robot_kf_est[2] = KF_RobotBall[2].KF_Prediction(interface.visionGUI.vision->getRobotPos(2));
        Ball_kf_est = KF_RobotBall[3].KF_Prediction(ballPosition);

    } // updateAllPositions

    bool start_signal(bool b) {
        if (b) {
            cout << "Start Clicked!" << endl;

            if (data) {
                interface.imageView.disable_image_show();
                free(data);
                data = 0;
            }
            /*GdkScreen* screen = gdk_screen_get_default();
            if (interface.vcap.format_dest.fmt.pix.width > gdk_screen_get_width(screen)/2 || interface.vcap.format_dest.fmt.pix.height > gdk_screen_get_height(screen)/2)
            {
            width = gdk_screen_get_width(screen)/2;
            height = gdk_screen_get_height(screen)/2;
            strategyGUI.strategy.set_constants(width,height);
            }
            else
            {*/
            width = interface.vcap.format_dest.fmt.pix.width;
            height = interface.vcap.format_dest.fmt.pix.height;
            strategyGUI.strategy.set_constants(width,height);
            //}

            interface.visionGUI.setFrameSize(width, height);

            // Liberar os botões de edit
            interface.robots_id_edit_bt.set_state(Gtk::STATE_NORMAL);
            interface.robots_speed_edit_bt.set_state(Gtk::STATE_NORMAL);
            interface.robots_function_edit_bt.set_state(Gtk::STATE_NORMAL);
            // strategyGUI.formation_box.set_sensitive(true);
            // strategyGUI.bt_createFormation.set_sensitive(true);

            data = (unsigned char *) calloc(interface.vcap.format_dest.fmt.pix.sizeimage, sizeof(unsigned char));

            interface.imageView.set_size_request(width, height);
            con = Glib::signal_idle().connect(sigc::mem_fun(*this, &CamCap::capture_and_show));

            cout << "Start Clicked! 1" << endl;
        }
        else {
            cout << "Stop Button Clicked!" << endl;
            con.disconnect();

            // Travar os botões de edit
            interface.robots_id_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
            interface.robots_speed_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
            interface.robots_function_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
            // strategyGUI.formation_box.set_sensitive(false);
            // strategyGUI.bt_createFormation.set_sensitive(false);
        }

        interface.__event_bt_quick_load_clicked();

        return true;
    } // start_signal

    bool capture_and_show() {
        if (!data) return false;

        if (frameCounter == 0) {
          timer.start();
        }
        frameCounter++;


        interface.vcap.grab_rgb(data);
        interface.imageView.set_data(data, width, height);
        interface.imageView.refresh();

        cv::Mat imageView(height, width, CV_8UC3, data);

        if(interface.imageView.hold_warp) {
            interface.warped = true;
            interface.bt_adjust.set_state(Gtk::STATE_NORMAL);
            interface.imageView.warp_event_flag = false;
            interface.imageView.warp_event_flag = false;
            interface.imageView.hold_warp=false;
        }

        interface.imageView.PID_test_flag = control.PID_test_flag;
        // interface.imageView.formation_flag = strategyGUI.formation_flag;
        interface.imageView.adjust_event_flag = interface.adjust_event_flag;

        if(interface.warped) {
            interface.bt_warp.set_active(false);
            interface.bt_warp.set_state(Gtk::STATE_INSENSITIVE);
            warp_transform(imageView);
            interface.imageView.warp_event_flag=false;

            if(interface.invert_image_flag)
            {
                cv::flip(imageView,imageView, -1);
            }
        }


        interface.visionGUI.vision->run(imageView);


        if(!interface.visionGUI.HSV_calib_event_flag) {
            if (!interface.draw_info_flag)
            {
                drawStrategyConstants(imageView, width, height);

                circle(imageView,interface.visionGUI.vision->getBall(), 7, cv::Scalar(255,255,255), 2);

                for (int i = 0; i < interface.visionGUI.vision->getRobotListSize(); i++)
                {
                    // robo 1
                    line(imageView, interface.visionGUI.vision->getRobot(i).position, interface.visionGUI.vision->getRobot(i).secundary,cv::Scalar(255,255,0), 2);
                    //line(imageView,interface.robot_list[0].position,interface.robot_list[0].ternary,cv::Scalar(100,255,0), 2);
                    putText(imageView, std::to_string(i+1),cv::Point(interface.visionGUI.vision->getRobot(i).position.x-5,interface.visionGUI.vision->getRobot(i).position.y-17),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0),2);
                    circle(imageView, interface.visionGUI.vision->getRobot(i).position, 15, cv::Scalar(255,255,0), 2);
                    // linha da pick-a
                    if(interface.visionGUI.vision->getRobot(i).rearPoint != cv::Point(-1,-1))
                        line(imageView, interface.visionGUI.vision->getRobot(i).secundary,interface.visionGUI.vision->getRobot(i).rearPoint,cv::Scalar(255,0,0), 2);
                }

                cv::Point aux_point;

                for(int i=0;i<5;i++){
                    aux_point.x = round(100*cos(strategyGUI.strategy.pot_angle[i]));
                    aux_point.y = - round(100*sin(strategyGUI.strategy.pot_angle[i]));
                    aux_point += interface.robot_list[2].position;
                    if(strategyGUI.strategy.pot_magnitude[i]!=0){
                        arrowedLine(imageView,interface.robot_list[2].position, aux_point, cv::Scalar(0,255,0));
                    }
                }

                aux_point.x = round(100*cos(strategyGUI.strategy.pot_goalTheta));
                aux_point.y = - round(100*sin(strategyGUI.strategy.pot_goalTheta));
                aux_point += interface.robot_list[2].position;
                arrowedLine(imageView,interface.robot_list[2].position, aux_point, cv::Scalar(255,255,0));
                aux_point.x = round(100*cos(interface.robot_list[2].transAngle));
                aux_point.y = - round(100*sin(interface.robot_list[2].transAngle));
                aux_point += interface.robot_list[2].position;
                arrowedLine(imageView,interface.robot_list[2].position, aux_point,cv::Scalar(255,0,0),2);

                for(int i=0; i<interface.visionGUI.vision->getAdvListSize(); i++)
                    circle(imageView,interface.visionGUI.vision->getAdvRobot(i), 15, cv::Scalar(0,0,255), 2);
            } // if !interface.draw_info_flag
        } // if !draw_info_flag
        else
        {
            interface.imageView.set_data(interface.visionGUI.vision->getThreshold(interface.visionGUI.Img_id).data, width, height);
            interface.imageView.refresh();
        }

        updateAllPositions();

        if(interface.imageView.PID_test_flag && !interface.get_start_game_flag())
        {
            control.button_PID_Test.set_active(true);
            PID_test();
        }
        else if(/*strategyGUI.formation_flag && */!interface.get_start_game_flag()) {
            formation_creation();

            // exibe os robos virtual
            for(int i = 0; i < 3; i++) {
                if(virtual_robot_selected == i) {
                    circle(imageView,virtual_robots_positions[i], 20, cv::Scalar(0,255,100), 3);
                }
                // posição
                circle(imageView,virtual_robots_positions[i], 17, cv::Scalar(0,255,0), 2);
                // orientação
                cv::Point aux_point = cv::Point(virtual_robots_positions[i].x + 30*cos(virtual_robots_orientations[i]), virtual_robots_positions[i].y + 30*sin(virtual_robots_orientations[i]));
                arrowedLine(imageView,virtual_robots_positions[i], aux_point,cv::Scalar(0,255,0),2);
                // identificação
                putText(imageView, std::to_string(i+1),virtual_robots_positions[i] + cv::Point(-14,10),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(0,255,0),2);
            }
        }
        else {
            for(int i=0; i<interface.robot_list.size(); i++) {
                interface.robot_list[i].target=cv::Point(-1,-1);
            }
            Selec_index=-1;
        }

        if (interface.imageView.PID_test_flag && interface.get_start_game_flag())
            control.button_PID_Test.set_active(false);

        if(Selec_index!=-1) {
            circle(imageView,interface.robot_list[Selec_index].position, 17, cv::Scalar(255,255,255), 2);
        }

        for(int i=0; i<interface.robot_list.size(); i++) {
            if(interface.robot_list[i].target.x!=-1&&interface.robot_list[i].target.y!=-1)
                line(imageView, interface.robot_list[i].position,interface.robot_list[i].target, cv::Scalar(255,255,255),2);
            circle(imageView,interface.robot_list[i].target, 7, cv::Scalar(255,255,255), 2);
        }

        // ----------- ESTRATEGIA -----------------//
        strategyGUI.strategy.set_Ball(interface.visionGUI.vision->getBall());

        if(interface.start_game_flag) {
            Ball_Est=strategyGUI.strategy.get_Ball_Est();
            line(imageView,interface.visionGUI.vision->getBall(),Ball_Est,cv::Scalar(255,140,0), 2);
            circle(imageView,Ball_Est, 7, cv::Scalar(255,140,0), 2);
            //char buffer[3]; -> não é utilizado
            strategyGUI.strategy.get_targets(&(interface.robot_list), (interface.visionGUI.vision->getAllAdvRobots()));
            for(int i =0; i<3; i++) {
                circle(imageView,interface.robot_list[i].target, 7, cv::Scalar(127,255,127), 2);
                putText(imageView,std::to_string(i+1),cv::Point(interface.robot_list[i].target.x-5,interface.robot_list[i].target.y-17),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(127,255,127),2);
            } // for
        } // if start_game_flag

        interface.update_speed_progressBars();
        // ----------------------------------------//


        if (frameCounter == 30)
        {
            timer.stop();
            fps_average = 30 / timer.getCronoTotalSecs();
            // cout<<"CPU Time: "<<timer.getCPUTotalSecs()<<",	\"CPU FPS\": "<<30/timer.getCPUTotalSecs()<<endl;
            // cout<<"FPS Time: "<<timer.getCronoTotalSecs()<<", FPS: "<<30/timer.getCronoTotalSecs()<<endl;
            timer.reset();
            frameCounter = 0;
        }

        return true;
    } // capture_and_show

    void arrowedLine(cv::Mat img, cv::Point pt1, cv::Point pt2, const cv::Scalar& color,
        int thickness=1, int line_type=8, int shift=0, double tipLength=0.1){
       const double tipSize = norm(pt1-pt2)*tipLength;
       line(img, pt1, pt2, color, thickness, line_type, shift);
       const double angle = atan2( (double) pt1.y - pt2.y, (double) pt1.x - pt2.x );
       cv::Point p(cvRound(pt2.x + tipSize * cos(angle + CV_PI / 4)),
       cvRound(pt2.y + tipSize * sin(angle + CV_PI / 4)));
       line(img, p, pt2, color, thickness, line_type, shift);
       p.x = cvRound(pt2.x + tipSize * cos(angle - CV_PI / 4));
       p.y = cvRound(pt2.y + tipSize * sin(angle - CV_PI / 4));
       line(img, p, pt2, color, thickness, line_type, shift);
   }

    void sendCmdToRobots(std::vector<Robot>&robot_list, bool &xbeeIsConnected){
        while (1) {
            if (interface.start_game_flag || interface.imageView.PID_test_flag) {
                // transformTargets(robot_list);
                control.s.sendCmdToRobots(robot_list);
            }
            boost::this_thread::sleep(boost::posix_time::milliseconds(300));
        }
    }

    void formation_creation() {
        if (interface.get_start_game_flag()) return;

        // if(strategyGUI.update_interface_flag) {
        //     for(int i = 0; i < 3; i++) {
        //         virtual_robots_positions[i] = strategyGUI.formation_positions[i];
        //         virtual_robots_orientations[i] = strategyGUI.formation_orientations[i];
        //     }
        //     strategyGUI.update_interface_flag = false;
        // }

        // marca o robô mais próximo
        for(int i = 0; i < 3; i++) {
            float dist = sqrt(pow((interface.imageView.robot_pos[0]-virtual_robots_positions[i].x),2)+pow((interface.imageView.robot_pos[1]-virtual_robots_positions[i].y),2));
            if(dist<=17) {
                virtual_robot_selected = i;
                interface.imageView.tar_pos[0] = -1;
                interface.imageView.tar_pos[1] = -1;
                interface.imageView.look_pos[0] = -1;
                interface.imageView.look_pos[1] = -1;
            }
        }

        // segundo clique
        if(virtual_robot_selected > -1) {
            if(interface.imageView.look_pos[0] >= 0) {
                float x1, x2, y1, y2;
                x1 = virtual_robots_positions[virtual_robot_selected].x;
                y1 = virtual_robots_positions[virtual_robot_selected].y;
                x2 = interface.imageView.look_pos[0];
                y2 = interface.imageView.look_pos[1];
                virtual_robots_orientations[virtual_robot_selected] = atan2((y2-y1)*1.3/height,(x2-x1)*1.5/width);
                update_formation_information();
            }
            else if(interface.imageView.tar_pos[0] >= 0) {
                virtual_robots_positions[virtual_robot_selected] = cv::Point(interface.imageView.tar_pos[0], interface.imageView.tar_pos[1]);
                update_formation_information();
            }
        }
    }
    void update_formation_information() {
        // reseta robo selecionado
        virtual_robot_selected = -1;
        // copia os dados pra estratégia
        for(int i = 0; i < 3; i++) {
            // strategyGUI.formation_positions[i] = virtual_robots_positions[i];
            // strategyGUI.formation_orientations[i] = virtual_robots_orientations[i];
        }
    }

/*    void transformTargets (std::vector<Robot>&robot_list){
        double tmp[2];
        for (int i = 0; i < 3; i++) {
            if(robot_list[i].target.x!=-1&&robot_list[i].target.y!=-1) {
                // tmp[0] = double(robot_list[i].target.x - robot_kf_est[i].x);
                // tmp[1] = double(robot_list[i].target.y - robot_kf_est[i].y);
                tmp[0] = double(robot_list[i].target.x - robot_list[i].position.x);
                tmp[1] = double(robot_list[i].target.y - robot_list[i].position.y);
                // cout << "tmp[0] " << tmp[0] << " tmp[1] " << tmp[1] << endl;
                robot_list[i].transTarget.x = round(cos(robot_list[i].orientation)*tmp[0] + sin(robot_list[i].orientation)*tmp[1]);
                robot_list[i].transTarget.y = round(-(-sin(robot_list[i].orientation)*tmp[0] + cos(robot_list[i].orientation)*tmp[1]));
                // cout << "robot_list[i].transTarget.x " << robot_list[i].transTarget.x << " robot_list[i].transTarget.y " << robot_list[i].transTarget.y << endl;
                // robot_list[i].transAngle = atan2(robot_list[i].transTarget.y, robot_list[i].transTarget.x);
                // cout << "transAngle " << robot_list[i].transAngle << endl;

            }else{
                robot_list[i].transTarget.x = NULL;
                robot_list[i].transTarget.y = NULL;
            }
        }
    } */

    void PID_test() {
        if (interface.get_start_game_flag()) return;

        double dist;
        int old_Selec_index;
        old_Selec_index = Selec_index;

        for(int i=0; i<interface.robot_list.size() && i<3; i++) {
            // interface.robot_list[i].cmdType = 0; // position cmd
            dist = sqrt(pow((interface.imageView.robot_pos[0]-interface.robot_list[i].position.x),2)+pow((interface.imageView.robot_pos[1]-interface.robot_list[i].position.y),2));
            if(dist<=17) {
                Selec_index=i;
                interface.imageView.tar_pos[0] = -1;
                interface.imageView.tar_pos[1] = -1;
                interface.robot_list[Selec_index].target=cv::Point(-1,-1);
                fixed_ball[Selec_index]=false;
            }
        }
        if(Selec_index>-1) {
            interface.robot_list[Selec_index].histWipe();
            if(sqrt(pow((interface.visionGUI.vision->getBall().x-interface.robot_list[Selec_index].target.x),2)+pow((interface.visionGUI.vision->getBall().y-interface.robot_list[Selec_index].target.y),2))<=7)
                fixed_ball[Selec_index]=true;

            if(fixed_ball[Selec_index])
                interface.robot_list[Selec_index].target=interface.visionGUI.vision->getBall();
            else
                interface.robot_list[Selec_index].target = cv::Point(interface.imageView.tar_pos[0],interface.imageView.tar_pos[1]);
        }


        for(int i=0; i<interface.robot_list.size() && i<3; i++) {
            if(fixed_ball[i])
                interface.robot_list[i].target=interface.visionGUI.vision->getBall();
            else {
                if(sqrt(pow((interface.robot_list[i].position.x-interface.robot_list[i].target.x),2)+
                pow((interface.robot_list[i].position.y-interface.robot_list[i].target.y),2))<15) {
                    interface.robot_list[i].target = cv::Point(-1,-1);
                    interface.imageView.tar_pos[0]=-1;
                    interface.imageView.tar_pos[1]=-1;
                    interface.robot_list[i].Vr = 0 ;
                    interface.robot_list[i].Vl = 0 ;
                }
                if(interface.robot_list[i].target.x!=-1&&interface.robot_list[i].target.y!=-1) {
                    //interface.robot_list[i].goTo(interface.robot_list[i].target,interface.visionGUI.vision->getBall());
                } else {
                    interface.robot_list[i].Vr = 0 ;
                    interface.robot_list[i].Vl = 0 ;
                }
            }
        }
    } // PID_test

    void drawStrategyConstants(cv::Mat imageView, int width, int height)
    {
        if (strategyGUI.get_deslocamentoZagaAtaque_flag())
            line(imageView,cv::Point(strategyGUI.strategy.DESLOCAMENTO_ZAGA_ATAQUE, 0),cv::Point(strategyGUI.strategy.DESLOCAMENTO_ZAGA_ATAQUE, height),cv::Scalar(255,255,0), 2);

        if (strategyGUI.get_goalArea_flag())
        {
            line(imageView, cv::Point(strategyGUI.strategy.LIMITE_AREA_X,strategyGUI.strategy.LARGURA_CAMPO/2-strategyGUI.strategy.TAMANHO_AREA/2),cv::Point(strategyGUI.strategy.LIMITE_AREA_X,strategyGUI.strategy.LARGURA_CAMPO/2+strategyGUI.strategy.TAMANHO_AREA/2), cv::Scalar(255,255,255),2);
            line(imageView, cv::Point(strategyGUI.strategy.LIMITE_AREA_X,strategyGUI.strategy.LARGURA_CAMPO/2-strategyGUI.strategy.TAMANHO_AREA/2),cv::Point(0,strategyGUI.strategy.LARGURA_CAMPO/2-strategyGUI.strategy.TAMANHO_AREA/2), cv::Scalar(255,255,255),2);
            line(imageView, cv::Point(strategyGUI.strategy.LIMITE_AREA_X,strategyGUI.strategy.LARGURA_CAMPO/2+strategyGUI.strategy.TAMANHO_AREA/2),cv::Point(0,strategyGUI.strategy.LARGURA_CAMPO/2+strategyGUI.strategy.TAMANHO_AREA/2), cv::Scalar(255,255,255),2);
        }

        if (strategyGUI.get_sideRectangles_flag())
        {
            line(imageView, cv::Point(strategyGUI.strategy.COMPRIMENTO_CAMPO_TOTAL - strategyGUI.strategy.LIMITE_AREA_X,0),cv::Point(strategyGUI.strategy.COMPRIMENTO_CAMPO_TOTAL - strategyGUI.strategy.LIMITE_AREA_X,strategyGUI.strategy.MIN_GOL_Y), cv::Scalar(255,255,255),2);
            line(imageView, cv::Point(strategyGUI.strategy.COMPRIMENTO_CAMPO_TOTAL - strategyGUI.strategy.LIMITE_AREA_X,height),cv::Point(strategyGUI.strategy.COMPRIMENTO_CAMPO_TOTAL - strategyGUI.strategy.LIMITE_AREA_X,strategyGUI.strategy.MAX_GOL_Y), cv::Scalar(255,255,255),2);
            line(imageView, cv::Point(strategyGUI.strategy.COMPRIMENTO_CAMPO_TOTAL - strategyGUI.strategy.LIMITE_AREA_X,strategyGUI.strategy.MAX_GOL_Y),cv::Point(width,strategyGUI.strategy.MAX_GOL_Y), cv::Scalar(255,255,255),2);
            line(imageView, cv::Point(strategyGUI.strategy.COMPRIMENTO_CAMPO_TOTAL - strategyGUI.strategy.LIMITE_AREA_X,strategyGUI.strategy.MIN_GOL_Y),cv::Point(width,strategyGUI.strategy.MIN_GOL_Y), cv::Scalar(255,255,255),2);
        }

        if (strategyGUI.get_ballRadius_flag())
        {
            ellipse(imageView, strategyGUI.strategy.Ball, cv::Size(100,100), 90, 0, 180, cv::Scalar(255,0,0), 2);
            line(imageView, cv::Point(strategyGUI.strategy.Ball.x, strategyGUI.strategy.Ball.y+100), cv::Point(strategyGUI.strategy.Ball.x, strategyGUI.strategy.Ball.y-100), cv::Scalar(255,0,0), 2);
        }

        if (strategyGUI.get_defenseLine_flag())
            line(imageView,cv::Point(strategyGUI.strategy.LINHA_ZAGA, 0),cv::Point(strategyGUI.strategy.LINHA_ZAGA, height),cv::Scalar(255,255,0), 2);

        if (strategyGUI.get_goalMax_flag())
            putText(imageView, "X", cv::Point(width-strategyGUI.strategy.COMPRIMENTO_PISTA, strategyGUI.strategy.MEIO_GOL_Y+strategyGUI.strategy.TAMANHO_GOL/2),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0), 2);

        if (strategyGUI.get_goalMin_flag())
            putText(imageView, "X", cv::Point(width-strategyGUI.strategy.COMPRIMENTO_PISTA, strategyGUI.strategy.MEIO_GOL_Y-strategyGUI.strategy.TAMANHO_GOL/2),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0), 2);

        if (strategyGUI.get_goalCenter_flag())
            putText(imageView, "X", cv::Point(strategyGUI.strategy.COORD_GOAL_ATK_FRONT_X, strategyGUI.strategy.COORD_GOAL_MID_Y),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0), 2);

        if (strategyGUI.get_banheira_flag())
            line(imageView,cv::Point(strategyGUI.strategy.BANHEIRA, 0),cv::Point(strategyGUI.strategy.BANHEIRA, height),cv::Scalar(255,255,0), 2);

        if (strategyGUI.get_areasDivision_flag())
            line(imageView,cv::Point(strategyGUI.strategy.corner_atk_limit, 0),cv::Point(strategyGUI.strategy.corner_atk_limit, height),cv::Scalar(255,255,0), 2);

        if (strategyGUI.get_areaLimitX_flag())
            line(imageView,cv::Point(strategyGUI.strategy.COORD_GOAL_DEF_FRONT_X, 0),cv::Point(strategyGUI.strategy.COORD_GOAL_DEF_FRONT_X, height),cv::Scalar(255,255,0), 2);
    } // drawStrategyConstants

    void warp_transform(cv::Mat imageView){
        cv::Point2f inputQuad[4];
        cv::Point2f outputQuad[4];
        cv::Mat lambda = cv::Mat::zeros( imageView.rows, imageView.cols, imageView.type() );

        inputQuad[0] = cv::Point2f( interface.imageView.warp_mat[0][0]-interface.offsetL,interface.imageView.warp_mat[0][1]);
        inputQuad[1] = cv::Point2f( interface.imageView.warp_mat[1][0]+interface.offsetR,interface.imageView.warp_mat[1][1]);
        inputQuad[2] = cv::Point2f( interface.imageView.warp_mat[2][0]+interface.offsetR,interface.imageView.warp_mat[2][1]);
        inputQuad[3] = cv::Point2f( interface.imageView.warp_mat[3][0]-interface.offsetL,interface.imageView.warp_mat[3][1]);

        outputQuad[0] = cv::Point2f( 0,0 );
        outputQuad[1] = cv::Point2f( width-1,0);
        outputQuad[2] = cv::Point2f( width-1,height-1);
        outputQuad[3] = cv::Point2f( 0,height-1  );
        lambda = getPerspectiveTransform( inputQuad, outputQuad );
        warpPerspective(imageView,imageView,lambda,imageView.size());

        if(interface.imageView.adjust_rdy) {
            interface.bt_adjust.set_active(false);
            interface.bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
            interface.adjust_event_flag = false;
            interface.imageView.adjust_event_flag = false;

            for(int i =0; i<interface.imageView.adjust_mat[0][1]; i++) {
                for(int j =0; j<3*interface.imageView.adjust_mat[0][0]; j++) {
                    imageView.at<uchar>(i, j) =0;
                }
            }

            for(int i = height; i>interface.imageView.adjust_mat[1][1]; i--) {
                for(int j =0; j<3*interface.imageView.adjust_mat[1][0]; j++) {
                    imageView.at<uchar>(i, j) =0;
                }
            }

            for(int i =0; i<interface.imageView.adjust_mat[2][1]; i++) {
                for(int j =3*width; j>3*interface.imageView.adjust_mat[2][0]; j--) {
                    imageView.at<uchar>(i, j) =0;
                }
            }

            for(int i =height; i>interface.imageView.adjust_mat[3][1]; i--) {
                for(int j =3*width; j>3*interface.imageView.adjust_mat[3][0]; j--) {
                    imageView.at<uchar>(i, j) =0;
                }
            }
        }
    } // warp_transform

    CamCap() : data(0), width(0), height(0), frameCounter(0) {
        fixed_ball[0]=false;
        fixed_ball[1]=false;
        fixed_ball[2]=false;
        fm.set_label("imageView");
        fm.add(interface.imageView);

        notebook.append_page(interface, "Capture");
        notebook.append_page(interface.visionGUI, "Vision");
        notebook.append_page(control, "Control");
        notebook.append_page(strategyGUI, "Strategy");

        robot_kf_est.push_back(Ball_Est);
        robot_kf_est.push_back(Ball_Est);
        robot_kf_est.push_back(Ball_Est);
        robot_kf_est.push_back(Ball_Est);

        KalmanFilter kf;
        KF_RobotBall.push_back(kf);
        KF_RobotBall.push_back(kf);
        KF_RobotBall.push_back(kf);
        KF_RobotBall.push_back(kf);

        for(int i = 0; i < 3; i++) {
            virtual_robots_orientations[i] = 0;
            virtual_robots_positions[i] = cv::Point(200, 480 / 6 + (i + 1) * 480 / 6); // !TODO hardcoded, usar variáveis quando possível
        }

        for(int i=0; i<4; i++) {
            interface.imageView.adjust_mat[i][0] = -1;
            interface.imageView.adjust_mat[i][1] = -1;
        }

        camera_vbox.pack_start(fm, false, true, 10);
        camera_vbox.pack_start(info_fm, false, true, 10);
        info_fm.add(interface.info_hbox);

        pack_start(camera_vbox, true, true, 10);
        pack_start(notebook, false, false, 10);

        // Thread que envia comandos para o robo
        threshold_threads.add_thread(new boost::thread(&CamCap::sendCmdToRobots,this,
        boost::ref(interface.robot_list), boost::ref(control.s.Serial_Enabled)));


        interface.signal_start().connect(sigc::mem_fun(*this, &CamCap::start_signal));
    }

    ~CamCap(){
        con.disconnect();
        interface.imageView.disable_image_show();
        free(data);

        data = 0;
    }

};

#endif /* CAMCAP_HPP_ */
