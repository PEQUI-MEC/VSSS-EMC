#ifndef CAMCAP_HPP_
#define CAMCAP_HPP_
#define PI 3.14159265453
//dimensões dos quadrados do tabuleiro
#define CALIBRATION_SQUARE_DIMENSION 0.02629f
// número de intersecções do tabuleiro
#define CHESSBOARD_DIMENSION cv::Size(6,9)

#include "Strategy2/Strategy2.hpp"
#include "Strategy2/Attacker.h"
#include "Strategy2/Defender.hpp"
#include "Strategy2/Goalkeeper.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/tracking.hpp>

#include "StrategyGUI.hpp"
#include "RobotGUI.hpp"
#include "vision/vision.hpp"
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
#include <chrono>
#include "LS.h"

#define MAX_THETA_TOLERATION 3
#define MAX_POSITIONING_VEL 0.1

class CamCap : public Gtk::HBox {

	public:
		Attacker attacker;
		Defender defender;
		Goalkeeper goalkeeper;

		Geometry::Point ball;
		Geometry::Point ball_est;

		std::array<Robot2 *, 3> robots;

		unsigned char *data;

		int width;
		int height;

		int frameCounter;

		boost::thread msg_thread;
		boost::condition_variable data_ready_cond;
		boost::mutex data_ready_mutex;
		bool data_ready_flag = false;
		bool ekf_data_ready = false;

		Strategy2 strategy;

		StrategyGUI strategyGUI;
		RobotGUI robotGUI;
		capture::V4LInterface interface;

		LS ls_x, ls_y;

		int fps_average = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> timer_start;

		cv::Point2f Ball_Est;

		Gtk::Frame fm;
		Gtk::Frame info_fm;
		Gtk::VBox camera_vbox;
		Gtk::Notebook notebook;

		sigc::connection con;

		void update_positions(const std::map<unsigned int, vision::Vision::RecognizedTag>& tags);
		bool start_signal(bool b);
		bool capture_and_show();
		void send_cmd_thread();
		void notify_data_ready(bool send_ekf_data);
		double distance(cv::Point a, cv::Point b);
		void calculate_ball_est();
		explicit CamCap(bool isLowRes);
		~CamCap() override;
};

#endif /* CAMCAP_HPP_ */
