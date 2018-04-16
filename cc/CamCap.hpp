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

#include "StrategyGUI.hpp"
#include "controlGUI.hpp"
#include "vision/vision.hpp"
#include "KalmanFilter.hpp"
#include "V4LInterface.hpp"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <cstdlib>
#include <ctgmath>
#include <gtkmm.h>
#include <cmath>
#include <fstream>
#include "CPUTimer.h"
#include "Constants.hpp"

#define MAX_THETA_TOLERATION 3
#define MAX_POSITIONING_VEL 0.1

class CamCap : public Gtk::HBox {

	public:
		int screenWidth, screenHeight;
		bool isLowRes;

		std::string fileName[6];
		double robot_pos[6] = {0, 0, 0, 0, 0, 0};
		double ball_pos = 0, ball_kf_pos = 0;
		double robot_kf_pos[6] = {0, 0, 0, 0, 0, 0};
		int positionCount = 0;
		int width, height;
		int Selec_index = -1;
		int fps_average = 0;
		int timerCounter = 0;
		CPUTimer timer;

		bool fixed_ball[3];
		bool KF_FIRST = true;

		unsigned char *data;

		int frameCounter;
		double ticks = 0;
		vector<cv::Point2f> robot_kf_est;
		vector<cv::Point2f> robot_kf_est_ini;
		vector<KalmanFilter> KF_Robot;

		cv::Point2f Ball_Est;
		cv::Point2f Ball_kf_est;
		cv::Point2f Ball_kf_est_ini;

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

		/* PARA TESTE */
		cv::Point obstacle;
		cv::Point deviation1;
		cv::Point deviation2;

		bool checkForLowRes();
		void updateAllPositions();
		void updateKalmanFilter();
		bool start_signal(bool b);
		bool capture_and_show();
		void arrowedLine(cv::Mat img, cv::Point pt1, cv::Point pt2,
						 const cv::Scalar &color, int thickness = 1,
						 int line_type = 8, int shift = 0, double tipLength = 0.1);
		void sendCmdToRobots(vector<Robot> &robot_list);
		double distance(cv::Point a, cv::Point b);
		double angular_distance(double alpha, double beta);
		void updating_formation();
		void update_formation_information();
		void PID_test();
		void warp_transform(cv::Mat imageView);
		CamCap(int screenW, int screenH);
		~CamCap();
};

#endif /* CAMCAP_HPP_ */
