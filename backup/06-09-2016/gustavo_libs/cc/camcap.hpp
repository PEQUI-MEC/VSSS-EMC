/*
 * camcap.hpp
 *
 *  Created on: Feb 18, 2014
 *      Author: gustavo
 */

#ifndef CAMCAP_HPP_
#define CAMCAP_HPP_

//#include <capture/v4l_device.hpp>
#include "opencv2/opencv.hpp"
#include <capture-gui/V4LInterface.hpp>
#include <capture-gui/ImageView.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <vector>
boost::mutex io_mutex;
 boost::thread_group threshold_threads;
struct ImageGray;

#include <gtkmm.h>

class CamCap: 	


public Gtk::HBox {
	public:
	 cv::Mat image_copy;
	bool warped = false;
		capture::V4LInterface v;
		capture::ImageView iv;
		unsigned char * d;
		unsigned char **threshold;
		int w, h;
		vector< cv::Point > Team_Main; 
		vector< cv::Point > Team_Sec; 
		vector< cv::Point > Adv_Main; 
		cv::Point Ball;
		Gtk::Frame fm;
		sigc::connection con;

		unsigned char * data;
		int width, height;
	
		
		
		/*virtual bool on_key_release_event(GdkEventKey *event)
		{

            cerr << event->keyval << endl;

			return true;

			}*/


		


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
				v.warp_event_flag =false;
				iv.hold_warp=false;			
			 }
			 iv.warp_event_flag = v.warp_event_flag;
			 if(v.save_warp_flag)	 save_warp();
			 if(v.load_warp_flag)	 load_warp();
			 if(v.reset_warp_flag)	 reset_warp();
			 
			 if(v.save_HSV_calib_flag)	 save_HSV();
			 if(v.load_HSV_calib_flag) 	 load_HSV();
			 
			 if(warped){
				v.bt_warp.set_active(false);
				v.bt_warp.set_state(Gtk::STATE_INSENSITIVE);
				warp_transform(image);
			 }

			 parallel_tracking(image);
			 for(int i=0;i<Team_Main.size();i++)
				cout<<i<<" "<<Team_Main[i].x<<" "<<Team_Main[i].y<<endl;
		
				
			 if(v.HSV_calib_event_flag){
				for(int i=0;i<3*(width*height + width) +2;i++){
					d[i]=threshold[v.Img_id][i];
					}
				
				}
					 
			
			
				 

        
			return true;
		}
		
		void parallel_tracking(cv::Mat im){
			 cv::Mat image_copy = im.clone();
			 cv::cvtColor(image_copy,image_copy,cv::COLOR_RGB2HSV);
			 
			 cv::medianBlur(image_copy, image_copy, 3);
			 
			 for(int i =0;i<6;i++)
			 threshold_threads.add_thread(new boost::thread(&CamCap::img_tracking,this, boost::ref(image_copy), (i)));
			 threshold_threads.join_all();
			 image_copy.release();
			}

			
		void img_tracking(cv::Mat image,int color_id){
			int ec,e3c,H,S,V;
			unsigned char * data_pointer = (unsigned char*) malloc((3*(width*height + width) +3) * sizeof(unsigned char));
			vector< vector<cv::Point> > contours;
			vector<cv::Vec4i> hierarchy;
		
			for(int i =0; i<image.rows;i++){ //Threshold calculations
				for(int j =0; j<image.cols;j++){
				ec = image.cols*i + j;
				e3c = ec*3;
				
				H = image.data[e3c];
				S = image.data[e3c + 1];
				V = image.data[e3c + 2];
					
				if((H>v.H[color_id][0]&&H<v.H[color_id][1])&&
				   (S>v.S[color_id][0]&&S<v.S[color_id][1])&&
				   (V>v.V[color_id][0]&&V<v.V[color_id][1])){
					
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
		cv::imwrite("teste.png",temp);
		cv::findContours(temp,contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);
		
		
		switch(color_id){
		
		case 0:// TEAM MAIN COLOR
		
		if (hierarchy.size() > 0) {
			Team_Main.clear();
			int j =0;
			int index = 0;
		while(j < 3 && index >= 0){
			cv::Moments moment = moments((cv::Mat)contours[index]);
			double area = moment.m00;
			//Se a área do objeto for muito pequena então provavelmente deve ser apenas ruído.
			if(area >= 0.1){
				Team_Main.push_back(cv::Point(moment.m10/area,moment.m01/area));
				j++;
			}
			index = hierarchy[index][0];
		}
		
	}
				
			
		break;
		case 1:// TEAM FIRST SECUNDARY COLOR
			
		break;
		case 2:// TEAM SECOND SECUNDARY COLOR
		
		break;
		case 3:// TEAM THIRD SECUNDARY COLOR
		
		break;
		case 4:// BALL
		
		break;
		case 5:// ADVERSARY MAIN COLOR
			
		break;
	
	}
	free(data_pointer);	
	}

		void reset_warp(){
			warped=false;
			v.reset_warp_flag=false;
			v.bt_warp.set_state(Gtk::STATE_NORMAL);
			v.offsetL = 0;
			v.offsetR = 0;
			v.HScale_offsetL.set_value(0);
			v.HScale_offsetR.set_value(0);
			}
		void save_warp(){
			
			std::ofstream txtFile;
			txtFile.open("warp_config.txt");
			txtFile << iv.warp_mat[0][0] <<std::endl<<iv.warp_mat[0][1] <<std::endl;
			txtFile << iv.warp_mat[1][0] <<std::endl<<iv.warp_mat[1][1] <<std::endl;
			txtFile << iv.warp_mat[2][0] <<std::endl<<iv.warp_mat[2][1] <<std::endl;
			txtFile << iv.warp_mat[3][0] <<std::endl<<iv.warp_mat[3][1] <<std::endl;
			txtFile << v.offsetL <<std::endl<<v.offsetR <<std::endl;	
			txtFile.close();
			v.save_warp_flag = false;
			}
			
			void save_HSV(){
			
			std::ofstream txtFile;
			txtFile.open("HSV_calib.txt");
			for(int i=0;i<6;i++){
			txtFile <<v.H[i][0]<<std::endl<<v.H[i][1]<<std::endl;
			txtFile <<v.S[i][0]<<std::endl<<v.S[i][1]<<std::endl;
			txtFile <<v.V[i][0]<<std::endl<<v.V[i][1]<<std::endl;
			}
			
			txtFile.close();
			v.save_HSV_calib_flag = false;
			}
			
			void load_HSV(){	
	
			std::ifstream txtFile;
			string linha;
			txtFile.open("HSV_calib.txt");
			for(int i=0;i<6;i++){
				getline(txtFile, linha); v.H[i][0]=atoi(linha.c_str());
				getline(txtFile, linha); v.H[i][1]=atoi(linha.c_str());
				getline(txtFile, linha); v.S[i][0]=atoi(linha.c_str());
				getline(txtFile, linha); v.S[i][1]=atoi(linha.c_str());
				getline(txtFile, linha); v.V[i][0]=atoi(linha.c_str());
				getline(txtFile, linha); v.V[i][1]=atoi(linha.c_str());
				}
					
				txtFile.close();		
				v.HScale_Hmin.set_value(v.H[v.Img_id][0]);
				v.HScale_Hmax.set_value(v.H[v.Img_id][1]);
				
				v.HScale_Smin.set_value(v.S[v.Img_id][0]);
				v.HScale_Smax.set_value(v.S[v.Img_id][1]);
			
				v.HScale_Vmin.set_value(v.V[v.Img_id][0]);
				v.HScale_Vmax.set_value(v.V[v.Img_id][1]);	
				
				v.load_HSV_calib_flag = false;	
		
				}
void load_warp(){	
		std::ifstream txtFile;
		string linha;
		txtFile.open("warp_config.txt");

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
		
		txtFile.close();
		v.load_warp_flag = false;
		warped=true;
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
			}

		CamCap() :
				data(0), width(0), height(0) {
					
			fm.set_label("Image");
			fm.add(iv);
			//pack_start(button, false, false, 10);
			pack_start(fm, true, true, 10);
			pack_start(v, false, false, 10);

			v.signal_start().connect(sigc::mem_fun(*this, &CamCap::start_signal));
//			button.signal_clicked().connect(sigc::mem_fun(*this, &CamCap::capture_and_show));

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

};

#endif /* CAMCAP_HPP_ */
