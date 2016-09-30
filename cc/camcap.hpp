/*
 * camcap.hpp
 *
 *  Created on: Feb 18, 2014
 *      Author: gustavo
 */

#ifndef CAMCAP_HPP_
#define CAMCAP_HPP_
#define PI 3.14159265453

//#include <capture/v4l_device.hpp>
#include "opencv2/opencv.hpp"
#include "strategyGUI.hpp"
#include "controlGUI.hpp"
//#include "filechooser.cpp"
#include <capture-gui/V4LInterface.hpp>
#include <capture-gui/ImageView.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <tgmath.h>
#include <gtkmm.h>
#include <math.h>
#include <fstream>
#include "Robot.hpp"
#include "CPUTimer.cpp"
#include "Strategy.hpp"
boost::mutex io_mutex;
boost::thread_group threshold_threads;
struct ImageGray;

class CamCap: 	


public Gtk::HBox {
	public:
		Strategy strats;
		cv::Mat image_copy;
		cv::Point Ball_Est;
		bool warped = false;
		bool start_game_flag = false;
		StrategyGUI strategy;
		ControlGUI control;
		capture::V4LInterface v;
		capture::ImageView iv;
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
		// ADV_Main[INDEX] - Vector de cv::Point 
		//   GUARDA A POSIÇÃO DAS TAGS PRIMÁRIAS DO ADVERSÁRIO(.x e .y acessam a posição)
		vector< cv::Point > Adv_Main; 
		// Ball - cv::Point 
		//   GUARDA A POSIÇÃO DA BOLA	
		cv::Point Ball;		
	
	
		vector<Robot> robot_list;
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
		Gtk::HBox info_hbox;
		Gtk::VBox robots_pos_vbox;
		Gtk::HBox robots_pos_hbox[7];
		Gtk::HBox start_game_hbox;
		Gtk::VBox buttons_vbox;
		Gtk::Label *robot1_pos_lb, *robot2_pos_lb, *robot3_pos_lb;
		Gtk::Label *ball_pos_lb;
		vector<string> robot_pos;
		Gtk::Button start_game_bt;
		Gtk::Frame robots_pos_fm;

		Gtk::Image red_button_released;
		Gtk::Image red_button_pressed;

		Gtk::Frame robots_id_fm;

		Gtk::VBox robots_id_vbox;
		Gtk::HBox robots_id_hbox[4];
		Gtk::ToggleButton robots_id_edit_bt;
		Gtk::Button robots_id_done_bt;
		Gtk::Entry robots_id_box[3];
		bool robots_id_edit_flag = false;

		Gtk::Frame robots_speed_fm;
		Gtk::VBox robots_speed_vbox[4];
		Gtk::VScale robots_speed_vscale[3];
		Gtk::HBox robots_speed_hbox[2];
		Gtk::ToggleButton robots_speed_edit_bt;
		Gtk::Button robots_speed_done_bt;
		bool robots_speed_edit_flag = false;



	/*	
		virtual bool on_key_release_event(GdkEventKey *event)
		{

            cerr << event->keyval << endl;

			return true;

			}
			*/

		// Função para retornar a posição de um robo
		cv::Point getRobotPosition(int robot_list_index)
		{
			return robot_list[robot_list_index].position;
		}
		
		double meanCalc(vector<int> v){
			int sum = std::accumulate(v.begin(), v.end(), 0.0);
			double mean = sum / v.size();
			//cout<<"Media = "<<mean<<endl;
			
			return mean;
		}

		double stdevCalc(vector<int> v){
			int sum = std::accumulate(v.begin(), v.end(), 0.0);
			double mean = sum / v.size();
			std::vector<double> diff(v.size());
			std::transform(v.begin(), v.end(), diff.begin(),
			std::bind2nd(std::minus<double>(), mean));
			double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
			double stdev = std::sqrt(sq_sum / v.size());
			
			return stdev;
		}
		
		void autoThreshold(cv::Mat img, cv::Point p){

			vector<int> H, S, V;
			
			cv::Mat imagem = img.clone();
			
			cv::cvtColor(imagem, imagem, cv::COLOR_RGB2HSV);
			int alpha_slider = 6;
			int horizontalOffset = alpha_slider, verticalOffset = alpha_slider, Hue, Sat, Val;
			
			//int threshould[3][2];
			
			for ( int i = (p.x-horizontalOffset); i < (p.x+horizontalOffset); i++ ){
				for ( int j = (p.y-verticalOffset); j < (p.y+verticalOffset); j++ ){
					
					//pixel = image.at<uchar>(j,i);
					//cout<<(int)pixel.val[0]<<"|"<<(int)pixel.val[1]<<"|"<<(int)pixel.val[2]<<endl;
					Hue = (int)imagem.at<cv::Vec3b>(j,i)[0]; Sat = (int)imagem.at<cv::Vec3b>(j,i)[1]; Val = (int)imagem.at<cv::Vec3b>(j,i)[2];
					//cout<< Hue <<"|"<< Sat <<"|"<< Val <<endl;
					H.push_back(Hue);
					S.push_back(Sat);
					V.push_back(Val);
				}
			}
			
			threshouldAuto[0][0] = meanCalc(H); threshouldAuto[0][1] = stdevCalc(H);
			threshouldAuto[1][0] = meanCalc(S); threshouldAuto[1][1] = stdevCalc(S);
			threshouldAuto[2][0] = meanCalc(V); threshouldAuto[2][1] = stdevCalc(V);	
			
			cout<<"H = "<<Hue<<endl;
			cout<<"S = "<<Sat<<endl;
			cout<<"V = "<<Val<<endl;
			
			
			//cout<<"HMIN = "<<threshouldAuto[0][0]-threshouldAuto[0][1]<<"| HMAX = "<<threshouldAuto[0][0]+threshouldAuto[0][1]<<endl;
			//cout<<"SMIN = "<<threshouldAuto[1][0]-threshouldAuto[1][1]<<"| SMAX = "<<threshouldAuto[1][0]+threshouldAuto[1][1]<<endl;
			//cout<<"VMIN = "<<threshouldAuto[2][0]-threshouldAuto[2][1]<<"| VMAX = "<<threshouldAuto[2][0]+threshouldAuto[2][1]<<endl;
			
		}

		bool start_signal(bool b) {
			if (b) {
				
				cout << "Start Clicked!" << endl;

				if (data) {
					iv.disable_image_show();
					free(data);
					data = 0;
				}
				Ball.x=0;
				Ball.y=0;
				width = v.vcap.format_dest.fmt.pix.width;
				height = v.vcap.format_dest.fmt.pix.height;
				strats.width = width;
				strats.height = height;
				
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
				
				iv.set_size_request(width, height);
				con = Glib::signal_idle().connect(sigc::mem_fun(*this, &CamCap::capture_and_show));
				

			} else {
				cout << "Stop Button Clicked!" << endl;
				con.disconnect();

			}

			return true;
		}

		bool capture_and_show() {
			if (!data) return false;
			
			//timer.start();
			v.vcap.grab_rgb(data);
			iv.set_data(data, width, height);
		

			iv.refresh();
			d = iv.get_data();
			
			w = iv.get_width();
			h = iv.get_height();
			
			cv::Mat image(h,w,CV_8UC3,d);

			if(iv.hold_warp){
				warped = true;
				v.bt_adjust.set_state(Gtk::STATE_NORMAL);
				v.warp_event_flag = false;
				iv.warp_event_flag = false;
				iv.hold_warp=false;			
			 }
			 else			 
			 iv.warp_event_flag = v.warp_event_flag;
			 
			 iv.auto_calib_flag = v.auto_calib_flag; // resolver

			 iv.PID_test_flag = control.PID_test_flag;
			 iv.adjust_event_flag = v.adjust_event_flag;
			
			 
			 if(v.save_warp_flag)	 save_warp();
			 if(v.load_warp_flag)	 load_warp();
			 if(v.reset_warp_flag)	 reset_warp();
			
			 if(v.save_HSV_calib_flag)	 save_HSV();
			 if(v.load_HSV_calib_flag) 	 load_HSV();
			 
			 if(warped){
				v.bt_warp.set_active(false);
				v.bt_warp.set_state(Gtk::STATE_INSENSITIVE);		
				warp_transform(image);
				iv.warp_event_flag=false;
				
				if(v.invert_image_flag)
				{
					cv::flip(image,image, -1);
				}
						
			 }
			  
			  // RESOLVER
			  
			  if(v.auto_calib_flag){
				  cout<<"----------------------------------------"<<endl; 	 
				  cv::Point pt = iv.pointClicked;
				  cv::Mat imgAux = image.clone();
				  autoThreshold(imgAux,pt);
				  
				  double H_MIN = threshouldAuto[0][0]-threshouldAuto[0][1]; double H_MAX = threshouldAuto[0][0]+threshouldAuto[0][1]; 
 				  double S_MIN = threshouldAuto[1][0]-threshouldAuto[1][1]; double S_MAX = threshouldAuto[1][0]+threshouldAuto[1][1]; 
				  double V_MIN = threshouldAuto[2][0]-threshouldAuto[2][1]; double V_MAX = threshouldAuto[2][0]+threshouldAuto[2][1]; 
				  
				  int color_id = v.Img_id; 
				  switch(color_id){
									  
						case 0:// TEAM MAIN COLOR						
							v.HScale_Hmin.set_value(H_MIN); v.HScale_Hmax.set_value(H_MAX);						
							v.HScale_Smin.set_value(S_MIN); //v.HScale_Smax.set_value(S_MAX);					
							v.HScale_Vmin.set_value(V_MIN); //v.HScale_Vmax.set_value(V_MAX);
							
							v.H[color_id][0] = H_MIN; v.H[color_id][1] = H_MAX;
							v.S[color_id][0] = S_MIN; //v.S[color_id][1] = S_MAX;
							v.V[color_id][0] = V_MIN; //v.V[color_id][1] = V_MAX;
							break;												
						
						case 1:// TEAM FIRST SECUNDARY COLOR
							v.HScale_Hmin.set_value(H_MIN); v.HScale_Hmax.set_value(H_MAX);						
							v.HScale_Smin.set_value(S_MIN); //v.HScale_Smax.set_value(S_MAX);					
							v.HScale_Vmin.set_value(V_MIN); //v.HScale_Vmax.set_value(V_MAX);
							
							v.H[color_id][0] = H_MIN; v.H[color_id][1] = H_MAX;
							v.S[color_id][0] = S_MIN; //v.S[color_id][1] = S_MAX;
							v.V[color_id][0] = V_MIN; //v.V[color_id][1] = V_MAX;   	
							break;
													
						case 2:// TEAM SECOND SECUNDARY COLOR					
							v.HScale_Hmin.set_value(H_MIN); v.HScale_Hmax.set_value(H_MAX);						
							v.HScale_Smin.set_value(S_MIN); //v.HScale_Smax.set_value(S_MAX);					
							v.HScale_Vmin.set_value(V_MIN); //v.HScale_Vmax.set_value(V_MAX);
							
							v.H[color_id][0] = H_MIN; v.H[color_id][1] = H_MAX;
							v.S[color_id][0] = S_MIN; //v.S[color_id][1] = S_MAX;
							v.V[color_id][0] = V_MIN; //v.V[color_id][1] = V_MAX;   	
							break;	
						
						case 3:// TEAM THIRD SECUNDARY COLOR
							v.HScale_Hmin.set_value(H_MIN); v.HScale_Hmax.set_value(H_MAX);						
							v.HScale_Smin.set_value(S_MIN); //v.HScale_Smax.set_value(S_MAX);					
							v.HScale_Vmin.set_value(V_MIN); //v.HScale_Vmax.set_value(V_MAX);
							
							v.H[color_id][0] = H_MIN; v.H[color_id][1] = H_MAX;
							v.S[color_id][0] = S_MIN; //v.S[color_id][1] = S_MAX;
							v.V[color_id][0] = V_MIN; //v.V[color_id][1] = V_MAX;   	
							break;
													
						case 4:// BALL
							v.HScale_Hmin.set_value(H_MIN); v.HScale_Hmax.set_value(H_MAX);						
							v.HScale_Smin.set_value(S_MIN); //v.HScale_Smax.set_value(S_MAX);					
							v.HScale_Vmin.set_value(V_MIN); //v.HScale_Vmax.set_value(V_MAX);
							
							v.H[color_id][0] = H_MIN; v.H[color_id][1] = H_MAX;
							v.S[color_id][0] = S_MIN; // v.S[color_id][1] = S_MAX;
							v.V[color_id][0] = V_MIN; // v.V[color_id][1] = V_MAX; 
							break;						
							
						case 5:// ADVERSARY MAIN COLOR
							v.HScale_Hmin.set_value(H_MIN); v.HScale_Hmax.set_value(H_MAX);						
							v.HScale_Smin.set_value(S_MIN); v.HScale_Smax.set_value(S_MAX);					
							v.HScale_Vmin.set_value(V_MIN); v.HScale_Vmax.set_value(V_MAX);
							
							v.H[color_id][0] = H_MIN; // v.H[color_id][1] = H_MAX;
							v.S[color_id][0] = S_MIN; // v.S[color_id][1] = S_MAX;
							v.V[color_id][0] = V_MIN; // v.V[color_id][1] = V_MAX;   	
							break;	
				  }
				  //v.auto_calib_flag = false;
			  }
			  

				parallel_tracking(image);
				robot_creation();
				
				
		//PRINT TAG PROPERTIES
		/*
			 cout<<"--------------------------------------------------------------- "<<endl;
			  cout<<"robot_list Size "<<robot_list.size()<<endl;
			 for(int i=0;i<robot_list.size();i++){
				cout<<i<<" px "<<robot_list[i].position.x<<" py "<<robot_list[i].position.y<<endl;
				cout<<i<<" sx "<<robot_list[i].secundary.x<<" sy "<<robot_list[i].secundary.y<<endl;
				cout<<" o "<<robot_list[i].orientation*180/PI<<endl;
				
		}*/
			
			
				circle(image,robot_list[0].position, 15, cv::Scalar(255,153,204), 2);
				line(image,robot_list[0].position,robot_list[0].secundary,cv::Scalar(255,153,204), 2);
				
				circle(image,robot_list[1].position, 15, cv::Scalar(51,153,102), 2);
				line(image,robot_list[1].position,robot_list[1].secundary,cv::Scalar(51,153,102), 2);
				
				circle(image,robot_list[2].position, 15, cv::Scalar(245,0,155), 2);
				line(image,robot_list[2].position,robot_list[2].secundary,cv::Scalar(245,0,155), 2);
				
				circle(image,Ball, 7, cv::Scalar(255,255,255), 2);
			
				
				
				if(iv.PID_test_flag)	 PID_test();
				else{
					for(int i=0;i<robot_list.size();i++){
					robot_list[i].target=cv::Point(-1,-1);
					}
					Selec_index=-1;
					}				
					if(Selec_index!=-1){
					circle(image,robot_list[Selec_index].position, 17, cv::Scalar(255,255,255), 2);				
			}
				
				for(int i=0;i<robot_list.size();i++){
					if(robot_list[i].target.x!=-1&&robot_list[i].target.y!=-1)
					line(image, robot_list[i].position,robot_list[i].target, cv::Scalar(255,255,255),2);
					circle(image,robot_list[i].target, 7, cv::Scalar(255,255,255), 2);
					}
				// ----------- ESTRATEGIA -----------------//
				strats.set_Ball(Ball);
				//count<<' cheou aqui 0'   <<endl 
				robot_list[0].goTo(strats.get_Attack_Classic(Ball, robot_list[0].position)); // Estratégia clássica
				//count<<' cheou aqui'   <<endl  
				
				//robot_list[0].target = strats.get_gk_target();
				//circle(image,robot_list[0].target, 7, cv::Scalar(127,127,255), 2);
				send_vel_to_robots();
				//count<<' cheou aqui 2'   <<endl 
				// ----------------------------------------//
				Ball_Est=strats.get_Ball_Est();
				line(image,Ball,Ball_Est,cv::Scalar(127,127,255), 2);
				circle(image,Ball_Est, 7, cv::Scalar(127,127,255), 2);
				if(v.HSV_calib_event_flag){
				for(int i=0;i<3*(width*height + width) +2;i++)
					d[i]=threshold[v.Img_id][i];
					}
					
					
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
			*/	
				//timer.stop();
				//cout<<"Time: "<<timer.getCPUTotalSecs()<<"	FPS: "<<1/timer.getCPUTotalSecs()<<endl;
				//timer.reset();
		
			return true;
		}

		
		void send_vel_to_robots(){
			for(int i=0;i<robot_list.size();i++){
			 if(robot_list[i].target.x!=-1&&robot_list[i].target.y!=-1){
					robot_list[i].goTo(robot_list[i].target);
				}else{
					robot_list[i].Vr = 0 ;
					robot_list[i].Vl = 0 ;
				}	
			}
			control.s.sendToThree(robot_list[0],robot_list[1],robot_list[2]);
			}
		

		void PID_test(){	
			double dist;
			int old_Selec_index;
			old_Selec_index = Selec_index;
			for(int i=0; i<robot_list.size();i++){
			dist = sqrt(pow((iv.robot_pos[0]-robot_list[i].position.x),2)+pow((iv.robot_pos[1]-robot_list[i].position.y),2));
			if(dist<=17){
			Selec_index=i;	
		    iv.tar_pos[0] = -1;
			iv.tar_pos[1] = -1;
			robot_list[Selec_index].target=cv::Point(-1,-1);
			fixed_ball[Selec_index]=false;
				}
			}
			if(Selec_index>-1){
			if(sqrt(pow((Ball.x-robot_list[Selec_index].target.x),2)+pow((Ball.y-robot_list[Selec_index].target.y),2))<=7)
				fixed_ball[Selec_index]=true;


			if(fixed_ball[Selec_index])
			    robot_list[Selec_index].target=Ball;
			else
				robot_list[Selec_index].target = cv::Point(iv.tar_pos[0],iv.tar_pos[1]);
			}
			
			
			for(int i=0;i<robot_list.size();i++){
			if(fixed_ball[i])
			 robot_list[i].target=Ball;
			 			
			 if(sqrt(pow((robot_list[i].position.x-robot_list[i].target.x),2)+
				pow((robot_list[i].position.y-robot_list[i].target.y),2))<15){

				robot_list[i].target = cv::Point(-1,-1);
				iv.tar_pos[0]=-1;
				iv.tar_pos[1]=-1;
				robot_list[i].Vr = 0 ;
				robot_list[i].Vl = 0 ;
			}
			 if(robot_list[i].target.x!=-1&&robot_list[i].target.y!=-1){
			 robot_list[i].goTo(robot_list[i].target);
			}else{
				robot_list[i].Vr = 0 ;
				robot_list[i].Vl = 0 ;
				}
				
			}
			
			

			//cout<<"---------------------------------------------------"<<endl;
		
			
			}
		
		void robot_creation(){
			Robot robot;
			cv::Point secundary;
			int index[2];
			

			float distanceRef = 999999999.0;
			float distance = 0;
			for(int j = 0; j < Team_Main.size()&&j<=3; j++){								
				for(int i = 0; i < 3; i++){	
					for(int k = 0; k < Team_Sec[i].size(); k++){
						
						distance = calcDistance(Team_Main[j],Team_Sec[i][k]);
						if(distance < distanceRef){
							distanceRef = distance; 
							index[0] = i;
							index[1] = k;
						}
					}
				}
				
				robot.position = Team_Main[j];
				robot.secundary = Team_Sec[index[0]][index[1]];	
				distanceRef = 999999999.0;			
				robot_list[index[0]].position = robot.position; // colocar em um vetor
				robot_list[index[0]].secundary = robot.secundary; // colocar em um vetor
				calcOrientation(index[0]);

				
			}
			// Atualizar as labels de posição dos robos

			stringstream aux1;
			aux1 << "(" << round((robot_list[0].position.x)*170/double(w))<< "," << round((robot_list[0].position.y)*130/double(h))<< "," << round(robot_list[0].orientation*(180/PI)) << ")";
			robot1_pos_lb->set_text(aux1.str());

			stringstream aux2;
			aux2 << "(" << round((robot_list[1].position.x)*170/double(w))<< "," << round((robot_list[1].position.y)*130/double(h))<< "," << round((robot_list[1].orientation*(180/PI))) << ")";
			robot2_pos_lb->set_text(aux2.str());

			stringstream aux3;
			aux3 << "(" << round((robot_list[2].position.x)*170/double(w))<< "," << round((robot_list[2].position.y)*130/double(h)) << "," <<  round((robot_list[2].orientation*(180/PI))) << ")";
			
			robot3_pos_lb->set_text(aux3.str());

	}	

		void calcOrientation(int tag_id){ //Define a orientação da tag em analise;				
			float sx,sy,px,py;

			sx =  robot_list[tag_id].secundary.x; 
			sy =  robot_list[tag_id].secundary.y;
			
			px = robot_list[tag_id].position.x; 
			py = robot_list[tag_id].position.y; 
			
			robot_list[tag_id].orientation = atan2(sy-py,sx-px);
			robot_list[tag_id].position.x = robot_list[tag_id].position.x;	
			robot_list[tag_id].position.y = robot_list[tag_id].position.y;	
		}

		float calcDistance(cv::Point position, cv::Point secundary){
		return sqrt(pow(position.x-secundary.x,2) + pow(position.y-secundary.y,2));
	}
			
		void parallel_tracking(cv::Mat im){
			 cv::Mat image_copy = im.clone();
			 cv::cvtColor(image_copy,image_copy,cv::COLOR_RGB2HSV);
			 
			 cv::medianBlur(image_copy, image_copy, 5);
			 
			 for(int i =0;i<6;i++)
			 threshold_threads.add_thread(new boost::thread(&CamCap::img_tracking,this, boost::ref(image_copy), (i)));
			 
			 threshold_threads.join_all();
			 image_copy.release();
			}
	
		void img_tracking(cv::Mat image,int color_id){
			int ec,e3c,H,S,V;
			vector< vector<cv::Point> > contours;
			vector<cv::Vec4i> hierarchy;
		
			for(int i =0; i<image.rows;i++){ //Threshold calculations
				for(int j =0; j<image.cols;j++){
				ec = image.cols*i + j;
				e3c = ec*3;
				
				H = image.data[e3c];
				S = image.data[e3c + 1];
				V = image.data[e3c + 2];
					
				if((H>=v.H[color_id][0]&&H<=v.H[color_id][1])&&
				   (S>=v.S[color_id][0]&&S<=v.S[color_id][1])&&
				   (V>=v.V[color_id][0]&&V<=v.V[color_id][1])){
					
					threshold[color_id][e3c] = 255;
					threshold[color_id][e3c+1] = 255;
					threshold[color_id][e3c+2] = 255;
				}else{
					threshold[color_id][e3c] = 0;
					threshold[color_id][e3c+1] = 0;
					threshold[color_id][e3c+2] = 0;	
				 }
		}
		}
		cv::Mat temp(height,width,CV_8UC3,threshold[color_id]);
		cv::cvtColor(temp,temp,cv::COLOR_RGB2GRAY);
		cv::findContours(temp,contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);
	
		
		switch(color_id){
		
		case 0:// TEAM MAIN COLOR
		
	if (hierarchy.size() > 0) {
			Team_Main.clear();
			int index = 0;
		while(index >= 0){
			cv::Moments moment = moments((cv::Mat)contours[index]);
			double area = moment.m00;
			//Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
			if(area >= v.Amin[color_id]/100){
				Team_Main.push_back(cv::Point(moment.m10/area,moment.m01/area));
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
		while(index >= 0){
			cv::Moments moment = moments((cv::Mat)contours[index]);
			double area = moment.m00;
			//Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
			if(area >= v.Amin[color_id]/100){
				Adv_Main.push_back(cv::Point(moment.m10/area,moment.m01/area));
			}
			index = hierarchy[index][0];
		}
		
	}
	
		break;
		
		case 4:// BALL
		if (hierarchy.size() > 0) {
			cv::Moments moment = moments((cv::Mat)contours[0]);
			double area = moment.m00;
			//Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
			if(area >= v.Amin[color_id]/100){
				Ball = cv::Point(moment.m10/area,moment.m01/area);
			     stringstream aux1;
				aux1 << "(" << (Ball.x)*(170/width) << "," << (Ball.y)*(130/height) << ")";
				ball_pos_lb->set_text(aux1.str());
			}
	}
		break;
		
		
		case 1:// TEAM FIRST SECUNDARY COLOR
		if (hierarchy.size() > 0) {
			Team_Sec[0].clear();
			int index = 0;
			while(index >= 0){
			cv::Moments moment = moments((cv::Mat)contours[index]);
			double area = moment.m00;
			//Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
			if(area >= v.Amin[color_id]/100){
				Team_Sec[0].push_back(cv::Point(moment.m10/area,moment.m01/area));
			}
			index = hierarchy[index][0];
		}
	}
		
			
		break;
		
		case 2:// TEAM SECOND SECUNDARY COLOR
				if (hierarchy.size() > 0) {
			Team_Sec[1].clear();
			int index = 0;
			while(index >= 0){
			cv::Moments moment = moments((cv::Mat)contours[index]);
			double area = moment.m00;
			//Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
			if(area >= v.Amin[color_id]/100){
				Team_Sec[1].push_back(cv::Point(moment.m10/area,moment.m01/area));
			}
			index = hierarchy[index][0];
		}
	}
		break;
		case 3:// TEAM THIRD SECUNDARY COLOR
			if (hierarchy.size() > 0) {
			Team_Sec[2].clear();
			int index = 0;
			while(index >= 0){
			cv::Moments moment = moments((cv::Mat)contours[index]);
			double area = moment.m00;
			//Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
			if(area >= v.Amin[color_id]/100){
				Team_Sec[2].push_back(cv::Point(moment.m10/area,moment.m01/area));
			}
			index = hierarchy[index][0];
		}
	}
		break;
	
	}
	}

		void reset_warp(){
			warped=false;
			v.reset_warp_flag=false;
			v.bt_warp.set_state(Gtk::STATE_NORMAL);
			v.bt_adjust.set_active(false);
			v.bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
			v.adjust_event_flag = false;
			iv.adjust_rdy=false;
			v.offsetL = 0;
			v.offsetR = 0;
			v.HScale_offsetL.set_value(0);
			v.HScale_offsetR.set_value(0);
			}
		
		void save_warp(){
			ofstream txtFile;

			if (v.quick_save_flag)
			{
				txtFile.open("WARP_quicksave.txt");
			}
			else
			{
				FileChooser loadWindow1;
				if (loadWindow1.result == Gtk::RESPONSE_OK)
				{
					txtFile.open(loadWindow1.filename);
				}
				else
				{
					v.save_warp_flag = false;
					return;
				}
			}
			

			txtFile << iv.warp_mat[0][0] <<std::endl<<iv.warp_mat[0][1] <<std::endl;
			txtFile << iv.warp_mat[1][0] <<std::endl<<iv.warp_mat[1][1] <<std::endl;
			txtFile << iv.warp_mat[2][0] <<std::endl<<iv.warp_mat[2][1] <<std::endl;
			txtFile << iv.warp_mat[3][0] <<std::endl<<iv.warp_mat[3][1] <<std::endl;
			txtFile << v.offsetL <<std::endl<<v.offsetR <<std::endl;	
			txtFile << iv.adjust_mat[0][0] <<std::endl<<iv.adjust_mat[0][1] <<std::endl;
			txtFile << iv.adjust_mat[1][0] <<std::endl<<iv.adjust_mat[1][1] <<std::endl;
			txtFile << iv.adjust_mat[2][0] <<std::endl<<iv.adjust_mat[2][1] <<std::endl;
			txtFile << iv.adjust_mat[3][0] <<std::endl<<iv.adjust_mat[3][1] <<std::endl;
			txtFile.close();
			v.save_warp_flag = false;
			}
			
		void save_HSV(){
			ofstream txtFile;

			if (v.quick_save_flag)
			{
				txtFile.open("HSV_quicksave.txt");
			}
			else
			{
				FileChooser loadWindow2;
				if (loadWindow2.result == Gtk::RESPONSE_OK)
				{
					txtFile.open(loadWindow2.filename);
				}
				else
				{
					v.save_HSV_calib_flag = false;
					return;
				}
			}

			for(int i=0;i<6;i++){
			txtFile <<v.H[i][0]<<std::endl<<v.H[i][1]<<std::endl;
			txtFile <<v.S[i][0]<<std::endl<<v.S[i][1]<<std::endl;
			txtFile <<v.V[i][0]<<std::endl<<v.V[i][1]<<std::endl;
			txtFile <<v.Amin[i]<<std::endl;
			
			}
			
			txtFile.close();
			v.save_HSV_calib_flag = false;
			v.quick_save_flag = false;
			}
			
		void load_HSV(){
			ifstream txtFile;	
			
			if (v.quick_load_flag)
			{
				txtFile.open("HSV_quicksave.txt");
			}
			else
			{
				FileChooser loadWindow3;
				if (loadWindow3.result == Gtk::RESPONSE_OK)
				{
					txtFile.open(loadWindow3.filename);
				}
				else
				{
					v.load_HSV_calib_flag = false;	
					return;
				}
			}
			

			string linha;
			
			for(int i=0;i<6;i++){
				getline(txtFile, linha); v.H[i][0]=atoi(linha.c_str());
				getline(txtFile, linha); v.H[i][1]=atoi(linha.c_str());
				getline(txtFile, linha); v.S[i][0]=atoi(linha.c_str());
				getline(txtFile, linha); v.S[i][1]=atoi(linha.c_str());
				getline(txtFile, linha); v.V[i][0]=atoi(linha.c_str());
				getline(txtFile, linha); v.V[i][1]=atoi(linha.c_str());
				getline(txtFile, linha); v.Amin[i]=atoi(linha.c_str());
			}
					
				txtFile.close();		
				v.HScale_Hmin.set_value(v.H[v.Img_id][0]);
				v.HScale_Hmax.set_value(v.H[v.Img_id][1]);
				
				v.HScale_Smin.set_value(v.S[v.Img_id][0]);
				v.HScale_Smax.set_value(v.S[v.Img_id][1]);
			
				v.HScale_Vmin.set_value(v.V[v.Img_id][0]);
				v.HScale_Vmax.set_value(v.V[v.Img_id][1]);	
				v.HScale_Amin.set_value(v.Amin[v.Img_id]);
				
				v.load_HSV_calib_flag = false;
				v.quick_load_flag = false;	
				
			
				}
		
		void load_warp(){	
		ifstream txtFile;
		
		if (v.quick_load_flag)
		{
			txtFile.open("WARP_quicksave.txt");
		}
		else
		{
			FileChooser loadWindow4;
			if (loadWindow4.result == Gtk::RESPONSE_OK)
			{
				txtFile.open(loadWindow4.filename);
			}
			else
			{
				v.load_warp_flag = false;
				return;
			}
		}
		
		

		string linha;

		getline(txtFile, linha); iv.warp_mat[0][0] = atoi(linha.c_str());
		getline(txtFile, linha); iv.warp_mat[0][1] = atoi(linha.c_str());
		//std::cout<< iv.warp_mat[0][0] <<std::endl<<iv.warp_mat[0][1] <<std::endl;
		
		getline(txtFile, linha); iv.warp_mat[1][0] = atoi(linha.c_str());
		getline(txtFile, linha); iv.warp_mat[1][1] = atoi(linha.c_str());
		//std::cout<< iv.warp_mat[1][0] <<std::endl<<iv.warp_mat[1][1] <<std::endl;
		
		getline(txtFile, linha); iv.warp_mat[2][0] = atoi(linha.c_str());
		getline(txtFile, linha); iv.warp_mat[2][1] = atoi(linha.c_str());
		//std::cout<< iv.warp_mat[2][0] <<std::endl<<iv.warp_mat[2][1] <<std::endl;
		
		getline(txtFile, linha); iv.warp_mat[3][0] = atoi(linha.c_str());
		getline(txtFile, linha); iv.warp_mat[3][1] = atoi(linha.c_str());
		//std::cout<< iv.warp_mat[3][0] <<std::endl<<iv.warp_mat[3][1] <<std::endl;
		getline(txtFile, linha); v.offsetL = atoi(linha.c_str());
		getline(txtFile, linha); v.offsetR = atoi(linha.c_str());
		
		getline(txtFile, linha); iv.adjust_mat[0][0] = atoi(linha.c_str());
		getline(txtFile, linha); iv.adjust_mat[0][1] = atoi(linha.c_str());
		//std::cout<< iv.warp_mat[0][0] <<std::endl<<iv.warp_mat[0][1] <<std::endl;
		
		getline(txtFile, linha); iv.adjust_mat[1][0] = atoi(linha.c_str());
		getline(txtFile, linha); iv.adjust_mat[1][1] = atoi(linha.c_str());
		//std::cout<< iv.warp_mat[1][0] <<std::endl<<iv.warp_mat[1][1] <<std::endl;
		
		getline(txtFile, linha); iv.adjust_mat[2][0] = atoi(linha.c_str());
		getline(txtFile, linha); iv.adjust_mat[2][1] = atoi(linha.c_str());
		//std::cout<< iv.warp_mat[2][0] <<std::endl<<iv.warp_mat[2][1] <<std::endl;
		
		getline(txtFile, linha); iv.adjust_mat[3][0] = atoi(linha.c_str());
		getline(txtFile, linha); iv.adjust_mat[3][1] = atoi(linha.c_str());
		//std::cout<< iv.warp_mat[3][0] <<std::endl<<iv.warp_mat[3][1] <<std::endl;
		
		txtFile.close();
		v.load_warp_flag = false;
		v.bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
		
		warped=true;
		iv.adjust_rdy = true;
		v.HScale_offsetL.set_value(v.offsetL);
		v.HScale_offsetR.set_value(v.offsetR);
		iv.warp_event_flag =false;
		v.warp_event_flag =false;
		
}
				
		void warp_transform(cv::Mat image){	
			cv::Point2f inputQuad[4]; 
			cv::Point2f outputQuad[4];
			cv::Mat lambda = cv::Mat::zeros( image.rows, image.cols, image.type() );
			
			inputQuad[0] = cv::Point2f( iv.warp_mat[0][0]-v.offsetL,iv.warp_mat[0][1]);
			inputQuad[1] = cv::Point2f( iv.warp_mat[1][0]+v.offsetR,iv.warp_mat[1][1]);
			inputQuad[2] = cv::Point2f( iv.warp_mat[2][0]+v.offsetR,iv.warp_mat[2][1]);
			inputQuad[3] = cv::Point2f( iv.warp_mat[3][0]-v.offsetL,iv.warp_mat[3][1]);  
			
			outputQuad[0] = cv::Point2f( 0,0 );
			outputQuad[1] = cv::Point2f( w-1,0);
			outputQuad[2] = cv::Point2f( w-1,h-1);
			outputQuad[3] = cv::Point2f( 0,h-1  );		
			lambda = getPerspectiveTransform( inputQuad, outputQuad );
			 warpPerspective(image,image,lambda,image.size());
			if(iv.adjust_rdy){ 
				v.bt_adjust.set_active(false);
				v.bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
				v.adjust_event_flag = false;
				iv.adjust_event_flag = false;
			for(int i =0; i<iv.adjust_mat[0][1];i++){
				 for(int j =0; j<3*iv.adjust_mat[0][0];j++){
			 image.at<uchar>(i, j) =0;
				}
			}
			
			for(int i = height; i>iv.adjust_mat[1][1];i--){
				 for(int j =0; j<3*iv.adjust_mat[1][0];j++){
			 image.at<uchar>(i, j) =0;
				}
			}
			
			for(int i =0; i<iv.adjust_mat[2][1];i++){
				 for(int j =3*width; j>3*iv.adjust_mat[2][0];j--){
			 image.at<uchar>(i, j) =0;
				}
			}
			
			for(int i =height; i>iv.adjust_mat[3][1];i--){
				 for(int j =3*width; j>3*iv.adjust_mat[3][0];j--){
			 image.at<uchar>(i, j) =0;
				}
			}
			
		 } 
	}

		CamCap() : data(0), width(0), height(0) {
			
		
			fixed_ball[0]=false;
			fixed_ball[1]=false;
			fixed_ball[2]=false;
			fm.set_label("Image");
			fm.add(iv);
			notebook.append_page(v, "Vision");
			notebook.append_page(control, "Control");
			notebook.append_page(strategy, "Strategy");
			Robot r;

			red_button_pressed.set("img/1475197289_pause-circle-outline.png");
			red_button_released.set("img/1475197265_play-circle-outline.png");
			red_button_released.set_size_request(100,100);
			red_button_pressed.set_size_request(100,100);



				robot_list.push_back(r);
				robot_list.push_back(r);
				robot_list.push_back(r);
				
			robot_list[0].ID = 'A';
			robot_list[1].ID = 'B';
			robot_list[2].ID = 'C';
				

			for(int i=0;i<robot_list.size();i++){
				robot_list[i].position = cv::Point(-1,-1);
			}	
				vector< cv::Point > p;
				p.push_back(cv::Point(0,0));
				Team_Sec.push_back(p);
				Team_Sec.push_back(p);
				Team_Sec.push_back(p);
				
				for(int i=0; i<4;i++){
					iv.adjust_mat[i][0] = -1;
					iv.adjust_mat[i][1] = -1;
					}
		


			
			camera_vbox.pack_start(fm, false, true, 10);
			camera_vbox.pack_start(info_fm, false, true, 10);
			info_fm.add(info_hbox);

			createPositionsFrame();
			createIDsFrame();
			createSpeedsFrame();


			

			info_hbox.pack_end(buttons_vbox, false, true, 5);
			buttons_vbox.pack_start(start_game_hbox, false, true, 5);
			start_game_hbox.pack_start(start_game_bt, false, true, 5);
			buttons_vbox.set_valign(Gtk::ALIGN_CENTER);
			//start_game_bt.set_label("BRING IT ON!");
			start_game_bt.property_always_show_image();
			start_game_bt.set_size_request(50,100);
			start_game_bt.set_image(red_button_released);


			
			pack_start(camera_vbox, true, true, 10);
			pack_start(notebook, false, false, 10);

			v.signal_start().connect(sigc::mem_fun(*this, &CamCap::start_signal));
			start_game_bt.signal_clicked().connect(sigc::mem_fun(*this, &CamCap::event_start_game_bt_signal_clicked));
		}

		~CamCap() {
			
			con.disconnect();
			iv.disable_image_show();
			free(data);
			if (threshold != NULL)
			{
				for(int i = 0; i < 6; i++)
					free(threshold[i]); 
				free(threshold);	
			}
			
			data = 0;
		}

		void event_robots_id_edit_bt_signal_pressed()
		{
			if (!robots_id_edit_flag)
			{
				robots_id_edit_flag = true;
				robots_id_box[0].set_state(Gtk::STATE_NORMAL);
				robots_id_box[1].set_state(Gtk::STATE_NORMAL);
				robots_id_box[2].set_state(Gtk::STATE_NORMAL);
				robots_id_done_bt.set_state(Gtk::STATE_NORMAL);
			}
			else
			{
				robots_id_edit_flag = false;
				robots_id_box[0].set_state(Gtk::STATE_INSENSITIVE);
				robots_id_box[1].set_state(Gtk::STATE_INSENSITIVE);
				robots_id_box[2].set_state(Gtk::STATE_INSENSITIVE);
				robots_id_done_bt.set_state(Gtk::STATE_INSENSITIVE);
			}
		}

		void event_robots_id_done_bt_signal_clicked()
		{
			std::string str;
			str = robots_id_box[0].get_text();
			robot_list[0].ID = str[0];
			str = robots_id_box[1].get_text();
			robot_list[1].ID = str[0];
			str = robots_id_box[2].get_text();
			robot_list[2].ID = str[0];
			/*std::cout << "ID Robo 1: " << robot_list[0].ID << endl;
			std::cout << "ID Robo 2: " << robot_list[1].ID << endl;
			std::cout << "ID Robo 3: " << robot_list[2].ID << endl;*/
		}

		void event_robots_speed_edit_bt_signal_pressed()
		{
			if (!robots_speed_edit_flag)
			{
				robots_speed_edit_flag = true;
				robots_speed_done_bt.set_state(Gtk::STATE_NORMAL);
				robots_speed_vscale[0].set_state(Gtk::STATE_NORMAL);
				robots_speed_vscale[1].set_state(Gtk::STATE_NORMAL);
				robots_speed_vscale[2].set_state(Gtk::STATE_NORMAL);
			}
			else
			{
				robots_speed_edit_flag = false;
				robots_speed_done_bt.set_state(Gtk::STATE_INSENSITIVE);
				robots_speed_vscale[0].set_state(Gtk::STATE_INSENSITIVE);
				robots_speed_vscale[1].set_state(Gtk::STATE_INSENSITIVE);
				robots_speed_vscale[2].set_state(Gtk::STATE_INSENSITIVE);
			}
		}

		void event_robots_speed_done_bt_signal_clicked()
		{
			robot_list[0].vmax = (float) robots_speed_vscale[0].get_value();
			robot_list[1].vmax = (float) robots_speed_vscale[1].get_value();
			robot_list[2].vmax = (float) robots_speed_vscale[2].get_value();
			/*std::cout << "Velocidade Robo 1: " << robot_list[0].V << endl;
			std::cout << "Velocidade Robo 2: " << robot_list[1].V << endl;
			std::cout << "Velocidade Robo 3: " << robot_list[2].V << endl;*/
		}

		void event_start_game_bt_signal_clicked()
		{
			if (!start_game_flag)
			{
				start_game_flag = true;
				start_game_bt.set_image(red_button_pressed);
			}
			else
			{
				start_game_flag = false;
				start_game_bt.set_image(red_button_released);
			}
		}

		void createPositionsFrame()
		{
			robots_pos_fm.set_label("Positions");
			info_hbox.pack_start(robots_pos_fm, false, true, 5);
			robots_pos_fm.add(robots_pos_vbox);
			robots_pos_vbox.set_size_request(170,-1);


			label = new Gtk::Label("Robot 1:");
			robot1_pos_lb = new Gtk::Label("-");
			robots_pos_hbox[0].pack_start(*label, false, true, 5);
			robots_pos_hbox[0].pack_start(*robot1_pos_lb, false, true, 5);
			robots_pos_vbox.pack_start(robots_pos_hbox[0], false, true, 5);


			label = new Gtk::Label("Robot 2:");
			robot2_pos_lb = new Gtk::Label("-");
			robots_pos_hbox[1].pack_start(*label, false, true, 5);
			robots_pos_hbox[1].pack_start(*robot2_pos_lb, false, true, 5);
			robots_pos_vbox.pack_start(robots_pos_hbox[1], false, true, 5);


			label = new Gtk::Label("Robot 3:");
			robot3_pos_lb = new Gtk::Label("-");
			robots_pos_hbox[2].pack_start(*label, false, true, 5);
			robots_pos_hbox[2].pack_start(*robot3_pos_lb, false, true, 5);
			robots_pos_vbox.pack_start(robots_pos_hbox[2], false, true, 5);

			label = new Gtk::Label("Ball:");
			ball_pos_lb = new Gtk::Label("-");
			robots_pos_hbox[3].pack_start(*label, false, true, 5);
			robots_pos_hbox[3].pack_start(*ball_pos_lb, false, true, 5);
			robots_pos_vbox.pack_start(robots_pos_hbox[3], false, true, 5);
		}

		void createIDsFrame()
		{
			info_hbox.pack_start(robots_id_fm, false, true, 5);
			robots_id_fm.set_label("IDs");
			robots_id_fm.add(robots_id_vbox);
			robots_id_vbox.pack_start(robots_id_hbox[0], false, true, 5);
			
			robots_id_hbox[0].pack_start(robots_id_edit_bt, false, true, 5);
			robots_id_hbox[0].pack_end(robots_id_done_bt, false, true, 5);
			robots_id_edit_bt.set_label("Edit");
			robots_id_done_bt.set_label("Done");
			

			
			label = new Gtk::Label("Robot 1: ");
			robots_id_hbox[1].pack_start(*label, false, true, 5);
			robots_id_hbox[1].pack_start(robots_id_box[0], false, true, 5);
			robots_id_box[0].set_max_length(1);
			robots_id_box[0].set_width_chars(2);
			robots_id_box[0].set_text(Glib::ustring::format("A"));
			robots_id_vbox.pack_start(robots_id_hbox[1], false, true, 5);
			

			
			label = new Gtk::Label("Robot 2: ");
			robots_id_hbox[2].pack_start(*label, false, true, 5);
			robots_id_hbox[2].pack_start(robots_id_box[1], false, true, 5);
			robots_id_box[1].set_max_length(1);
			robots_id_box[1].set_width_chars(2);
			robots_id_box[1].set_text(Glib::ustring::format("B"));
			robots_id_vbox.pack_start(robots_id_hbox[2], false, true, 5);

			
			label = new Gtk::Label("Robot 3: ");
			robots_id_hbox[3].pack_start(*label, false, true, 5);
			robots_id_hbox[3].pack_start(robots_id_box[2], false, true, 5);
			robots_id_box[2].set_max_length(1);
			robots_id_box[2].set_width_chars(2);
			robots_id_box[2].set_text(Glib::ustring::format("C"));
			robots_id_vbox.pack_start(robots_id_hbox[3], false, true, 5);

			robots_id_box[0].set_state(Gtk::STATE_INSENSITIVE);
			robots_id_box[1].set_state(Gtk::STATE_INSENSITIVE);
			robots_id_box[2].set_state(Gtk::STATE_INSENSITIVE);
			robots_id_done_bt.set_state(Gtk::STATE_INSENSITIVE);

			robots_id_edit_bt.signal_pressed().connect(sigc::mem_fun(*this, &CamCap::event_robots_id_edit_bt_signal_pressed));
			robots_id_done_bt.signal_clicked().connect(sigc::mem_fun(*this, &CamCap::event_robots_id_done_bt_signal_clicked));
		}

		void createSpeedsFrame()
		{
			info_hbox.pack_start(robots_speed_fm, false, true, 5);
			robots_speed_fm.set_label("Speeds");
			robots_speed_fm.add(robots_speed_vbox[0]);
			robots_speed_vbox[0].pack_start(robots_speed_hbox[0], false, true, 5);
			robots_speed_vbox[0].pack_start(robots_speed_hbox[1], false, true, 5);
			robots_speed_hbox[1].set_halign(Gtk::ALIGN_CENTER);


			robots_speed_hbox[0].pack_start(robots_speed_edit_bt, false, true, 5);
			robots_speed_edit_bt.set_label("Edit");
			robots_speed_hbox[0].pack_end(robots_speed_done_bt, false, true, 5);
			robots_speed_done_bt.set_label("Done");

			label = new Gtk::Label("R1");
			robots_speed_vscale[0].set_digits(1);
			robots_speed_vscale[0].set_increments(0.1,1);
			robots_speed_vscale[0].set_range(0,6);
			robots_speed_vscale[0].set_size_request(25,100);
			robots_speed_vscale[0].set_inverted(true);
			robots_speed_vscale[0].set_value(6);
			robots_speed_vbox[1].pack_start(robots_speed_vscale[0], false, true, 5);
			robots_speed_vbox[1].pack_start(*label, false, true, 5);
			robots_speed_hbox[1].pack_start(robots_speed_vbox[1], false, true, 5);

			label = new Gtk::Label("R2");
			robots_speed_vscale[1].set_digits(1);
			robots_speed_vscale[1].set_increments(0.1,1);
			robots_speed_vscale[1].set_range(0,6);
			robots_speed_vscale[1].set_size_request(25,100);
			robots_speed_vscale[1].set_inverted(true);
			robots_speed_vscale[1].set_value(6);
			robots_speed_vbox[2].pack_start(robots_speed_vscale[1], false, true, 5);
			robots_speed_vbox[2].pack_start(*label, false, true, 5);
			robots_speed_hbox[1].pack_start(robots_speed_vbox[2], false, true, 5);

			label = new Gtk::Label("R3");
			robots_speed_vscale[2].set_digits(1);
			robots_speed_vscale[2].set_increments(0.1,1);
			robots_speed_vscale[2].set_range(0,6);
			robots_speed_vscale[2].set_size_request(25,100);
			robots_speed_vscale[2].set_inverted(true);
			robots_speed_vscale[2].set_value(6);
			robots_speed_vbox[3].pack_start(robots_speed_vscale[2], false, true, 5);
			robots_speed_vbox[3].pack_start(*label, false, true, 5);
			robots_speed_hbox[1].pack_start(robots_speed_vbox[3], false, true, 5);

			robots_speed_done_bt.set_state(Gtk::STATE_INSENSITIVE);
			robots_speed_vscale[0].set_state(Gtk::STATE_INSENSITIVE);
			robots_speed_vscale[1].set_state(Gtk::STATE_INSENSITIVE);
			robots_speed_vscale[2].set_state(Gtk::STATE_INSENSITIVE);

			robots_speed_edit_bt.signal_pressed().connect(sigc::mem_fun(*this, &CamCap::event_robots_speed_edit_bt_signal_pressed));
			robots_speed_done_bt.signal_clicked().connect(sigc::mem_fun(*this, &CamCap::event_robots_speed_done_bt_signal_clicked));



		}

};

#endif /* CAMCAP_HPP_ */
