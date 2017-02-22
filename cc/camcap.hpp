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

        StrategyGUI strategyGUI;
        ControlGUI control;
        capture::V4LInterface interface;

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
          Robot robot;
          cv::Point ballPosition;
          //  cout<<"AQUI"<<endl;
          vision->robot_creation_unitag();
          //vision->robot_creation();
        //    cout<<"AQUI"<<endl;
          for (int i = 0; i < vision->get_robot_list_size(); i++)
          {
            robot = vision->get_robot_from_list(i);
            interface.robot_list[i].position = robot.position;
            interface.robot_list[i].orientation = robot.orientation;
            interface.robot_list[i].secundary = robot.secundary;
          }

          ballPosition = vision->get_ball_position();
          interface.ballX = ballPosition.x;
          interface.ballY = ballPosition.y;

          interface.robot_list[0].feedHist(interface.robot_list[0].position);
          interface.robot_list[1].feedHist(interface.robot_list[1].position);
          interface.robot_list[2].feedHist(interface.robot_list[2].position);

          interface.updateRobotLabels();
        }

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


                // Liberar os botões de edit
                interface.robots_id_edit_bt.set_state(Gtk::STATE_NORMAL);
                interface.robots_speed_edit_bt.set_state(Gtk::STATE_NORMAL);
                interface.robots_function_edit_bt.set_state(Gtk::STATE_NORMAL);
                interface.robots_save_bt.set_state(Gtk::STATE_NORMAL);
                interface.robots_load_bt.set_state(Gtk::STATE_NORMAL);

                vision = new Vision(width, height);

                data = (unsigned char *) calloc(interface.vcap.format_dest.fmt.pix.sizeimage, sizeof(unsigned char));

                interface.imageView.set_size_request(width, height);
                con = Glib::signal_idle().connect(sigc::mem_fun(*this, &CamCap::capture_and_show));

                cout << "Start Clicked! 1" << endl;
            } else {
                cout << "Stop Button Clicked!" << endl;
                con.disconnect();

                // Travar os botões de edit
                interface.robots_id_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
                interface.robots_speed_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
                interface.robots_function_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
                interface.robots_save_bt.set_state(Gtk::STATE_INSENSITIVE);
                interface.robots_load_bt.set_state(Gtk::STATE_INSENSITIVE);

            }

            //interface.__event_bt_quick_load_clicked();

            return true;
        }

        bool capture_and_show() {
            if (!data) return false;

            //timer.start();
            interface.vcap.grab_rgb(data);
            interface.imageView.set_data(data, width, height);


            interface.imageView.refresh();
            d = interface.imageView.get_data();

            w = interface.imageView.get_width();
            h = interface.imageView.get_height();



            cv::Mat imageView(h,w,CV_8UC3,d);

            if(interface.imageView.hold_warp) {
                interface.warped = true;
                interface.bt_adjust.set_state(Gtk::STATE_NORMAL);
                interface.imageView.warp_event_flag = false;
                interface.imageView.warp_event_flag = false;
                interface.imageView.hold_warp=false;
            }

            interface.imageView.PID_test_flag = control.PID_test_flag;
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

            vision->setHSV(interface.H,interface.S,interface.V,interface.Amin);
            //TRACKING CAMERA


            vision->parallel_tracking(imageView);



            if(!interface.HSV_calib_event_flag) {
              updateAllPositions();

                drawStrategyConstants(imageView, w, h);

                if (!interface.draw_info_flag)
                {
                    circle(imageView,interface.robot_list[0].position, 15, cv::Scalar(255,255,0), 2);
                    line(imageView,interface.robot_list[0].position,interface.robot_list[0].secundary,cv::Scalar(255,255,0), 2);
                    //line(imageView,interface.robot_list[0].position,interface.robot_list[0].ternary,cv::Scalar(100,255,0), 2);
                    putText(imageView,"1",cv::Point(interface.robot_list[0].position.x-5,interface.robot_list[0].position.y-17),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0),2);
                    circle(imageView,interface.robot_list[1].position, 15, cv::Scalar(255,255,0), 2);
                    line(imageView,interface.robot_list[1].position,interface.robot_list[1].secundary,cv::Scalar(255,255,0), 2);
                    //line(imageView,interface.robot_list[1].position,interface.robot_list[1].ternary,cv::Scalar(100,255,0), 2);
                    putText(imageView,"2",cv::Point(interface.robot_list[1].position.x-5,interface.robot_list[1].position.y-17),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0),2);
                    circle(imageView,interface.robot_list[2].position, 15, cv::Scalar(255,255,0), 2);
                    line(imageView,interface.robot_list[2].position,interface.robot_list[2].secundary,cv::Scalar(255,255,0), 2);
                    //line(imageView,interface.robot_list[2].position,interface.robot_list[2].ternary,cv::Scalar(100,255,0), 2);
                    putText(imageView,"3",cv::Point(interface.robot_list[2].position.x-5,interface.robot_list[2].position.y-17),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0),2);
                    circle(imageView,vision->get_ball_position(), 7, cv::Scalar(255,255,255), 2);

                    //std::cout << "Robot 0 position: (" << interface.robot_list[0].position.x << "," << interface.robot_list[0].position.y << ")" << std::endl;
                    //std::cout << "Robot 1 position: (" << interface.robot_list[1].position.x << "," << interface.robot_list[1].position.y << ")" << std::endl;
                    //std::cout << "Robot 2 position: (" << interface.robot_list[2].position.x << "," << interface.robot_list[2].position.y << ")" << std::endl;

                    for(int i=0; i<vision->Adv_Main.size(); i++)
                        circle(imageView,vision->Adv_Main[i], 15, cv::Scalar(0,0,255), 2);
                }


            }

            if(interface.imageView.PID_test_flag)	 PID_test();
            else {
                for(int i=0; i<interface.robot_list.size(); i++) {
                    interface.robot_list[i].target=cv::Point(-1,-1);
                }
                Selec_index=-1;
            }

            if(Selec_index!=-1) {
                circle(imageView,interface.robot_list[Selec_index].position, 17, cv::Scalar(255,255,255), 2);
            }

            for(int i=0; i<interface.robot_list.size(); i++) {
                if(interface.robot_list[i].target.x!=-1&&interface.robot_list[i].target.y!=-1)
                    line(imageView, interface.robot_list[i].position,interface.robot_list[i].target, cv::Scalar(255,255,255),2);
                   circle(imageView,interface.robot_list[i].target, 7, cv::Scalar(255,255,255), 2);
            }

            // ----------- ESTRATEGIA -----------------//
		       	strategyGUI.strategy.set_Ball(vision->get_ball_position());

            if(interface.start_game_flag) {
                Ball_Est=strategyGUI.strategy.get_Ball_Est();
                line(imageView,vision->get_ball_position(),Ball_Est,cv::Scalar(255,140,0), 2);
                circle(imageView,Ball_Est, 7, cv::Scalar(255,140,0), 2);
                char buffer[3];
                for(int i =0; i<3; i++) {
                    switch (interface.robot_list[i].role)	{
                    case 0:
                        interface.robot_list[i].target = strategyGUI.strategy.get_gk_target(vision->Adv_Main);
                        interface.robot_list[i].fixedPos = strategyGUI.strategy.Goalkeeper.fixedPos;
                        if(strategyGUI.strategy.GOAL_DANGER_ZONE) {
                            interface.robot_list[i].histWipe();
                        }
                        break;
                    case 2:
                        interface.robot_list[i].target = strategyGUI.strategy.get_atk_target(interface.robot_list[i].position, interface.robot_list[i].orientation);
                        interface.robot_list[i].fixedPos = strategyGUI.strategy.Attack.fixedPos;
                        interface.robot_list[i].status = strategyGUI.strategy.Attack.status;
                        /*	for(int j=0;j<vision->Adv_Main.size();j++){
                        		if ( sqrt(pow(vision->Adv_Main[j].x - interface.robot_list[i].position.x, 2) + pow(vision->Adv_Main[j].y - interface.robot_list[i].position.y, 2)) < 50) {
                        			interface.robot_list[i].histWipe();
                        		}
                        	}*/
                        break;
                    case 1:
                        interface.robot_list[i].target = strategyGUI.strategy.get_def_target(interface.robot_list[i].position);
                        interface.robot_list[i].fixedPos = strategyGUI.strategy.Defense.fixedPos;
                        interface.robot_list[i].status = strategyGUI.strategy.Defense.status;
                        /*	for(int j=0;j<vision->Adv_Main.size();j++){
                        		if ( sqrt(pow(vision->Adv_Main[j].x - interface.robot_list[i].position.x, 2) + pow(vision->Adv_Main[j].y - interface.robot_list[i].position.y, 2)) < 50) {
                        			interface.robot_list[i].spin = true;
                        		}
                        	}*/
                        break;
                     case 3:
						            interface.robot_list[i].target = strategyGUI.strategy.get_opp_target(interface.robot_list[i].position, interface.robot_list[i].orientation);
                        interface.robot_list[i].fixedPos = strategyGUI.strategy.Opponent.fixedPos;
                        interface.robot_list[i].status = strategyGUI.strategy.Opponent.status;

                     break;
                    }
                    //cout<<interface.robot_list[0].target.x<<" - "<<interface.robot_list[0].target.y<<endl;
                    circle(imageView,interface.robot_list[i].target, 7, cv::Scalar(127,255,127), 2);

                    putText(imageView,std::to_string(i+1),cv::Point(interface.robot_list[i].target.x-5,interface.robot_list[i].target.y-17),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(127,255,127),2);
                }
            }
            //cout<<interface.robot_list[2].status<<" | "<<interface.robot_list[2].interface<<endl;

            interface.update_speed_progressBars();
            send_vel_to_robots();
            // ----------------------------------------//




            		//timer.stop();
            		//cout<<"Time: "<<timer.getCPUTotalSecs()<<"	FPS: "<<1/timer.getCPUTotalSecs()<<endl;
            		//timer.reset();

            if(interface.HSV_calib_event_flag) {
                for(int i=0; i<3*(width*height + width) +2; i++)
                    d[i]=vision->threshold[interface.Img_id][i];
            }
            return true;
        }

        void send_vel_to_robots() {
            for(int i=0; i<interface.robot_list.size(); i++) {
                if(interface.robot_list[i].target.x!=-1&&interface.robot_list[i].target.y!=-1) {
                    interface.robot_list[i].goTo(interface.robot_list[i].target,vision->get_ball_position());
                } else {
                    interface.robot_list[i].Vr = 0 ;
                    interface.robot_list[i].Vl = 0 ;
                }
            }
            control.s.sendToThree(interface.robot_list[0],interface.robot_list[1],interface.robot_list[2]);
        }

        void PID_test() {
            double dist;
            int old_Selec_index;
            old_Selec_index = Selec_index;
            for(int i=0; i<interface.robot_list.size(); i++) {
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
                if(sqrt(pow((vision->get_ball_position().x-interface.robot_list[Selec_index].target.x),2)+pow((vision->get_ball_position().y-interface.robot_list[Selec_index].target.y),2))<=7)
                    fixed_ball[Selec_index]=true;


                if(fixed_ball[Selec_index])
                    interface.robot_list[Selec_index].target=vision->get_ball_position();
                else
                    interface.robot_list[Selec_index].target = cv::Point(interface.imageView.tar_pos[0],interface.imageView.tar_pos[1]);
            }


            for(int i=0; i<interface.robot_list.size(); i++) {
                if(fixed_ball[i])
                    interface.robot_list[i].target=vision->get_ball_position();
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
                        interface.robot_list[i].goTo(interface.robot_list[i].target,vision->get_ball_position());
                    } else {
                        interface.robot_list[i].Vr = 0 ;
                        interface.robot_list[i].Vl = 0 ;
                    }

                }

            }


        }

        void drawStrategyConstants(cv::Mat imageView, int w, int h)
        {

          if (strategyGUI.get_deslocamentoZagaAtaque_flag())
            line(imageView,cv::Point(strategyGUI.strategy.DESLOCAMENTO_ZAGA_ATAQUE, 0),cv::Point(strategyGUI.strategy.DESLOCAMENTO_ZAGA_ATAQUE, h),cv::Scalar(255,255,0), 2);

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
            line(imageView,cv::Point(strategyGUI.strategy.LINHA_ZAGA, 0),cv::Point(strategyGUI.strategy.LINHA_ZAGA, h),cv::Scalar(255,255,0), 2);

          if (strategyGUI.get_goalMax_flag())
            putText(imageView, "X", cv::Point(w-strategyGUI.strategy.COMPRIMENTO_PISTA, strategyGUI.strategy.MEIO_GOL_Y+strategyGUI.strategy.TAMANHO_GOL/2),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0), 2);

          if (strategyGUI.get_goalMin_flag())
            putText(imageView, "X", cv::Point(w-strategyGUI.strategy.COMPRIMENTO_PISTA, strategyGUI.strategy.MEIO_GOL_Y-strategyGUI.strategy.TAMANHO_GOL/2),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0), 2);

          if (strategyGUI.get_goalCenter_flag())
            putText(imageView, "X", cv::Point(strategyGUI.strategy.MEIO_GOL_X, strategyGUI.strategy.MEIO_GOL_Y),cv::FONT_HERSHEY_PLAIN,1,cv::Scalar(255,255,0), 2);

          if (strategyGUI.get_banheira_flag())
            line(imageView,cv::Point(strategyGUI.strategy.BANHEIRA, 0),cv::Point(strategyGUI.strategy.BANHEIRA, h),cv::Scalar(255,255,0), 2);

          if (strategyGUI.get_areasDivision_flag())
            line(imageView,cv::Point(strategyGUI.strategy.DIVISAO_AREAS, 0),cv::Point(strategyGUI.strategy.DIVISAO_AREAS, h),cv::Scalar(255,255,0), 2);

          if (strategyGUI.get_areaLimitX_flag())
            line(imageView,cv::Point(strategyGUI.strategy.LIMITE_AREA_X, 0),cv::Point(strategyGUI.strategy.LIMITE_AREA_X, h),cv::Scalar(255,255,0), 2);
        }

        void warp_transform(cv::Mat imageView){
            cv::Point2f inputQuad[4];
            cv::Point2f outputQuad[4];
            cv::Mat lambda = cv::Mat::zeros( imageView.rows, imageView.cols, imageView.type() );

            inputQuad[0] = cv::Point2f( interface.imageView.warp_mat[0][0]-interface.offsetL,interface.imageView.warp_mat[0][1]);
            inputQuad[1] = cv::Point2f( interface.imageView.warp_mat[1][0]+interface.offsetR,interface.imageView.warp_mat[1][1]);
            inputQuad[2] = cv::Point2f( interface.imageView.warp_mat[2][0]+interface.offsetR,interface.imageView.warp_mat[2][1]);
            inputQuad[3] = cv::Point2f( interface.imageView.warp_mat[3][0]-interface.offsetL,interface.imageView.warp_mat[3][1]);

            outputQuad[0] = cv::Point2f( 0,0 );
            outputQuad[1] = cv::Point2f( w-1,0);
            outputQuad[2] = cv::Point2f( w-1,h-1);
            outputQuad[3] = cv::Point2f( 0,h-1  );
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
        }

        CamCap() : data(0), width(0), height(0){

            fixed_ball[0]=false;
            fixed_ball[1]=false;
            fixed_ball[2]=false;
            fm.set_label("imageView");
            fm.add(interface.imageView);
            notebook.append_page(interface, "Vision");
            notebook.append_page(control, "Control");
            notebook.append_page(strategyGUI, "strategyGUI");


            for(int i=0; i<4; i++) {
                interface.imageView.adjust_mat[i][0] = -1;
                interface.imageView.adjust_mat[i][1] = -1;
            }




            camera_vbox.pack_start(fm, false, true, 10);
            camera_vbox.pack_start(info_fm, false, true, 10);
            info_fm.add(interface.info_hbox);

            pack_start(camera_vbox, true, true, 10);
            pack_start(notebook, false, false, 10);

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
