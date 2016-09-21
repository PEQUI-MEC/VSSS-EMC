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

boost::mutex io_mutex;
 boost::thread_group threshold_threads;
struct ImageGray;

struct Robot{
	cv::Point primary= cv::Point(-1,-1);
	cv::Point secundary= cv::Point(-1,-1);
	double orientation;
	cv::Point position= cv::Point(-1,-1);
	cv::Point target = cv::Point(-1,-1);
};

class CamCap: 	


public Gtk::HBox {
	public:
	 cv::Mat image_copy;
	bool warped = false;
		StrategyGUI strategy;
		ControlGUI control;
		capture::V4LInterface v;
		capture::ImageView iv;
		unsigned char * d;
		unsigned char **threshold;
		Gtk::Notebook notebook;
		int w, h;
		
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



		bool start_signal(bool b) {
			if (b) {
				
				cout << "Start Clicked!" << endl;

				if (data) {
					iv.disable_image_show();
					free(data);
					data = 0;
				}
		
				width = v.vcap.format_dest.fmt.pix.width;
				height = v.vcap.format_dest.fmt.pix.height;
			
				
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

			 iv.PID_test_flag = control.PID_test_flag;
			 iv.adjust_event_flag = v.adjust_event_flag;
			 if(v.save_warp_flag)	 save_warp();
			 if(v.load_warp_flag)	 load_warp();
			 if(v.reset_warp_flag)	 reset_warp();
			
			 if(v.save_HSV_calib_flag)	 save_HSV();
			 if(v.load_HSV_calib_flag) 	 load_HSV();
			 
			 if(warped){
				v.bt_warp.set_active(false);			
				warp_transform(image);
				v.bt_warp.set_state(Gtk::STATE_INSENSITIVE);
				iv.warp_event_flag=false;		
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
				


			return true;
		}
		void PID_test(){
			double dist;
			int old_Selec_index;
			old_Selec_index = Selec_index;
			for(int i=0; i<robot_list.size();i++){
			dist = sqrt(pow((iv.robot_pos[0]-robot_list[i].primary.x),2)+pow((iv.robot_pos[1]-robot_list[i].primary.y),2));
			//cout<<"Dist "<<dist<<endl;
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


			if(sqrt(pow((robot_list[Selec_index].primary.x-robot_list[Selec_index].target.x),2)+
					pow((robot_list[Selec_index].primary.y-robot_list[Selec_index].target.y),2))<15){

				robot_list[Selec_index].target = cv::Point(-1,-1);
				iv.tar_pos[0]=-1;
				iv.tar_pos[1]=-1;
			}else if(fixed_ball[Selec_index])
			    robot_list[Selec_index].target=Ball;
			else
				robot_list[Selec_index].target = cv::Point(iv.tar_pos[0],iv.tar_pos[1]);
			}
			for(int i=0;i<robot_list.size();i++){
			if(fixed_ball[i])
			 robot_list[i].target=Ball;
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
				
				robot.primary = Team_Main[j];
				robot.secundary = Team_Sec[index[0]][index[1]];	
				distanceRef = 999999999.0;			
				robot_list[index[0]].primary = robot.primary; // colocar em um vetor
				robot_list[index[0]].secundary = robot.secundary; // colocar em um vetor
				calcOrientation(index[0]);

				
			}
			// Atualizar as labels de posição dos robos

			stringstream aux1;
			aux1 << "(" << (robot_list[0].primary.x)*(170/width) << "," << (robot_list[0].primary.y)*(130/height) << "," << round(robot_list[0].orientation*(180/PI)) << ")";
			robot1_pos_lb->set_text(aux1.str());

			stringstream aux2;
			aux2 << "(" << (robot_list[1].primary.x)*(170/width) << "," << (robot_list[1].primary.y)*(130/height) << "," << round((robot_list[1].orientation*(180/PI))) << ")";
			robot2_pos_lb->set_text(aux2.str());

			stringstream aux3;
			aux3 << "(" << (robot_list[2].primary.x)*(170/width) << "," << (robot_list[2].primary.y)*(130/height) << "," <<  round((robot_list[2].orientation*(180/PI))) << ")";

			robot3_pos_lb->set_text(aux3.str());
			
	}	

		void calcOrientation(int tag_id){ //Define a orientação da tag em analise;				
			float sx,sy,px,py;

			sx =  robot_list[tag_id].secundary.x; 
			sy =  robot_list[tag_id].secundary.y;
			
			px = robot_list[tag_id].primary.x; 
			py = robot_list[tag_id].primary.y; 
			
			robot_list[tag_id].orientation = atan2(sy-py,sx-px);
			robot_list[tag_id].position.x = robot_list[tag_id].primary.x;	
			robot_list[tag_id].position.y = robot_list[tag_id].primary.y;	
		}

		float calcDistance(cv::Point primary, cv::Point secundary){
		return sqrt(pow(primary.x-secundary.x,2) + pow(primary.y-secundary.y,2));
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
			v.bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
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
				v.bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
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

		CamCap() :
				data(0), width(0), height(0) {
			fixed_ball[0]=false;
			fixed_ball[1]=false;
			fixed_ball[2]=false;
			fm.set_label("Image");
			fm.add(iv);
			info_fm.set_label("Info");
			notebook.append_page(v, "Vision");
			notebook.append_page(control, "Control");
			notebook.append_page(strategy, "Strategy");
			Robot t;
				robot_list.push_back(t);
				robot_list.push_back(t);
				robot_list.push_back(t);

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
			info_hbox.pack_start(robots_pos_vbox, false, true, 5);
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

			info_hbox.pack_start(buttons_vbox, false, true, 5);
			buttons_vbox.pack_start(start_game_hbox, false, true, 5);
			start_game_hbox.pack_start(start_game_bt, false, true, 5);
			buttons_vbox.set_valign(Gtk::ALIGN_CENTER);
			start_game_bt.set_label("BRING IT ON!");
			start_game_bt.set_size_request(50,100);


			
			pack_start(camera_vbox, true, true, 10);
			pack_start(notebook, false, false, 10);

			v.signal_start().connect(sigc::mem_fun(*this, &CamCap::start_signal));
			start_game_bt.signal_clicked().connect(sigc::mem_fun(*this, &CamCap::event_start_game_bt_signal_clicked));


		}

		~CamCap() {
			
			con.disconnect();
			iv.disable_image_show();
			free(data);
			for(int i = 0; i < 6; i++)
			free(threshold[i]); 
			free(threshold);	
			data = 0;
		}

		void event_start_game_bt_signal_clicked()
		{
			cout << "BRING IT ON!" <<std::endl;
		}

};

#endif /* CAMCAP_HPP_ */
