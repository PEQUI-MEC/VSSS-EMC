#include <Strategy2/Robot2.h>
#include "CamCap.hpp"
#include "ImageArt.hpp"

using namespace vision;

using std::cout;
using std::endl;
using std::vector;

void CamCap::update_positions(const std::array<Vision::RecognizedTag, 3> &tags) {
	for (int i = 0; i < tags.size(); i++) {
		const Vision::RecognizedTag& tag = tags[i];
		for (auto& robot: robots) {
			if (robot->tag == i) {
				robot->set_pose(tag.position, tag.orientation);
			}
		}
	}

	interface.updateRobotLabels();

	const cv::Point cv_ball = interface.visionGUI.vision->getBall();
	ball = Geometry::from_cv_point(cv_ball);
	interface.update_ball_position(ball);
}

bool CamCap::start_signal(bool b) {
	if (b) {
		cout << "Start Clicked!" << endl;

		if (data) {
			interface.imageView.disable_image_show();
			free(data);
			data = nullptr;
		}

		width = interface.vcap.format_dest.fmt.pix.width;
		height = interface.vcap.format_dest.fmt.pix.height;

		interface.visionGUI.setFrameSize(width, height);

		// Liberar os botões de edit
		robotGUI.enable_main_buttons();

		data = (unsigned char *) calloc(interface.vcap.format_dest.fmt.pix.sizeimage, sizeof(unsigned char));

		interface.imageView.set_size_request(width, height);
		con = Glib::signal_idle().connect(sigc::mem_fun(*this, &CamCap::capture_and_show));
	} else {
		cout << "Stop Button Clicked!" << endl;
		con.disconnect();

		// Travar os botões de edit
		robotGUI.enable_main_buttons(false);
	}

	interface.__event_bt_quick_load_clicked();
	interface.visionGUI.quickLoadGMM();

	return true;
} // start_signal

bool CamCap::capture_and_show() {

	bool chessBoardFound = false;
	std::vector<cv::Vec2f> foundPoints;

	if (!data) return false;


	if (frameCounter == 0) {
		timer_start = std::chrono::high_resolution_clock::now();
	}
	frameCounter++;

	interface.vcap.grab_rgb(data);
	interface.imageView.set_data(data, width, height);
	interface.imageView.refresh();

	cv::Mat imageView(height, width, CV_8UC3, data);

	if (interface.imageView.hold_warp) {
		interface.warped = true;
		interface.bt_adjust.set_state(Gtk::STATE_NORMAL);
		interface.imageView.warp_event_flag = false;
		interface.imageView.warp_event_flag = false;
		interface.imageView.hold_warp = false;
	}

	if (interface.reset_warp_flag) {
		interface.imageView.warp_counter = 0;
		interface.reset_warp_flag = false;
	}

	interface.imageView.split_flag = interface.visionGUI.getIsSplitView();
	interface.imageView.PID_test_flag = control.PID_test_flag;
	interface.imageView.adjust_event_flag = interface.adjust_event_flag;
	interface.imageView.gmm_sample_flag = interface.visionGUI.getSamplesEventFlag();

	if (interface.imageView.sector != -1) {
		interface.visionGUI.selectFrame(interface.imageView.sector);
		interface.imageView.sector = -1;
	}

	if (interface.visionGUI.vision->flag_cam_calibrated) {
		cv::Mat temp;
		imageView.copyTo(temp);
		cv::undistort(temp, imageView, interface.visionGUI.vision->getcameraMatrix(),
					  interface.visionGUI.vision->getdistanceCoeficents());
	}

	if (interface.warped) {
		interface.bt_warp.set_active(false);
		interface.bt_warp.set_state(Gtk::STATE_INSENSITIVE);
		warp_transform(imageView);
		interface.imageView.warp_event_flag = false;

		if (interface.invert_image_flag) {
			cv::flip(imageView, imageView, -1);
		}
	}

	if (interface.CamCalib_flag_event && !interface.get_start_game_flag() &&
		!interface.visionGUI.vision->flag_cam_calibrated) {

		chessBoardFound = cv::findChessboardCorners(imageView, CHESSBOARD_DIMENSION, foundPoints,
													CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
	}

	if (interface.imageView.gmm_ready_flag) {
		interface.visionGUI.gmm->setFrame(imageView);
		interface.visionGUI.gmm->pushSample(interface.imageView.gmm_clicks);
		interface.visionGUI.incrementSamples();
		interface.imageView.gmm_ready_flag = false;
	}
//	if (!interface.visionGUI.getIsHSV()) { // GMM
//		if (interface.visionGUI.gmm->getIsTrained()) {
//			interface.visionGUI.gmm->run(imageView);
//			updateAllPositions();
//			if (control.ekf_always_send || interface.get_start_game_flag() || interface.imageView.PID_test_flag) {
//				notify_data_ready(true);
//			}
//			interface.visionGUI.vision->recordVideo(imageView);
//			if (interface.visionGUI.gmm->getDoneFlag()) {
//				for (auto &window : interface.visionGUI.gmm->windowsList) {
//					rectangle(imageView, window.getPosition(),
//							  window.getEnd(), cv::Scalar(250, 155, 0));
//				}
//			}
//
//			interface.visionGUI.vision->runGMM(interface.visionGUI.gmm->getAllThresholds(),
//											   interface.visionGUI.gmm->getWindowsList());
//
//			if (interface.visionGUI.getGaussiansFrameFlag()) {
//				interface.imageView.set_data(interface.visionGUI.gmm->getGaussiansFrame().data, width, height);
//				interface.imageView.refresh();
//			} else if (interface.visionGUI.getFinalFrameFlag()) {
//				interface.imageView.set_data(interface.visionGUI.gmm->getFinalFrame().data, width, height);
//				interface.imageView.refresh();
//			} else if (interface.visionGUI.getThresholdFrameFlag()) {
//				interface.imageView.set_data(
//						interface.visionGUI.gmm->getThresholdFrame(interface.visionGUI.getGMMColorIndex()).data,
//						width, height);
//				interface.imageView.refresh();
//			}
//		}
//	} else { // HSV Simples
//		interface.visionGUI.vision->run(imageView);
//		updateAllPositions();
//		if (control.ekf_always_send || interface.get_start_game_flag() || interface.imageView.PID_test_flag) {
//			notify_data_ready(true);
//		}
//
//		if (interface.visionGUI.getIsSplitView()) {
//			interface.imageView.set_data(interface.visionGUI.vision->getSplitFrame().clone().data, width, height);
//			interface.imageView.refresh();
//		} else if (interface.visionGUI.HSV_calib_event_flag) {
//			interface.imageView.set_data(interface.visionGUI.vision->getThreshold(interface.visionGUI.Img_id).data,
//										 width, height);
//			interface.imageView.refresh();
//		}
//	}

	std::array<Vision::RecognizedTag, 3> tags = interface.visionGUI.vision->run(imageView);

	calculate_ball_est();
	update_positions(tags);

	interface.updateFPS(fps_average);

	if (control.ekf_always_send || interface.get_start_game_flag() || interface.imageView.PID_test_flag) {
		notify_data_ready(true);
	}

	if (interface.visionGUI.getIsSplitView()) {
		interface.imageView.set_data(interface.visionGUI.vision->getSplitFrame().clone().data, width, height);
		interface.imageView.refresh();
	} else if (interface.visionGUI.CIELAB_calib_event_flag) {
		interface.imageView.set_data(interface.visionGUI.vision->getThreshold(interface.visionGUI.Img_id).data,
									 width, height);
		interface.imageView.refresh();
	}

	control.update_dropped_frames();

	if (interface.imageView.PID_test_flag && !interface.get_start_game_flag()) {
		control.button_PID_Test.set_active(true);
		PID_test();
	} else {
		Selec_index = -1;
		control.PID_test_flag = false;
	}

	if (interface.imageView.PID_test_flag && interface.get_start_game_flag())
		control.button_PID_Test.set_active(false);


	// ----------- ESTRATEGIA -----------------//
	if (interface.get_start_game_flag()) {
		circle(imageView, Ball_Est, 7, cv::Scalar(255, 140, 0), 2);
//		strategyGUI.strategy.get_uvf_targets( interface.robot_list );

		strategy.run();

		robotGUI.update_speed_progressBars();
		robotGUI.update_robot_functions();
	} // if start_game_flag
	// ----------------------------------------//

	if (!interface.visionGUI.CIELAB_calib_event_flag) {
		if (chessBoardFound) {
			cv::TermCriteria termCriteria = cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001);
			cv::Mat grayFrame;
			cv::cvtColor(imageView, grayFrame, cv::COLOR_RGB2GRAY);
			cv::cornerSubPix(grayFrame, foundPoints, cv::Size(11, 11), cv::Size(-1, -1), termCriteria);
		}

		if (interface.visionGUI.getIsDrawing() && !interface.visionGUI.getIsSplitView()) {
			cv::drawChessboardCorners(imageView, CHESSBOARD_DIMENSION, foundPoints, chessBoardFound);

			art::draw(imageView,
					  interface.visionGUI.gmm->getSamplePoints(),
					  interface.visionGUI.vision->getBall(),
					  tags,
					  interface.visionGUI.vision->get_adv_robots(),
					  robots);

		} // if !interface.draw_info_flag
	} // if !draw_info_flag


	if (frameCounter == 10) {
		auto timer_finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = timer_finish - timer_start;
		fps_average = static_cast<int>(10 / elapsed.count());
		frameCounter = 0;
	}

	if (interface.get_start_game_flag() || interface.imageView.PID_test_flag) {
		control.update_msg_time();
		notify_data_ready(false);
	}

	return true;
} // capture_and_show

void CamCap::send_cmd_thread() {
	boost::unique_lock<boost::mutex> lock(data_ready_mutex);
	while (true) {
		try {
			data_ready_cond.wait(lock, [this]() { return data_ready_flag; });
		} catch (...) {
			lock.unlock();
			return;
		}
		data_ready_flag = false;
		if(ekf_data_ready) {
			for (auto& robot : this->robots)
				control.messenger.send_ekf_data(*robot);
			ekf_data_ready = false;
		} else {
			control.messenger.send_commands(this->robots);
		}
	}
}

void CamCap::notify_data_ready(bool send_ekf_data) {
	data_ready_flag = true;
	ekf_data_ready = send_ekf_data;
	data_ready_cond.notify_all();
}

double CamCap::distance(cv::Point a, cv::Point b) {
	return sqrt(pow(double(b.x - a.x), 2) + pow(double(b.y - a.y), 2));
}

void CamCap::PID_test() {
	if (interface.get_start_game_flag()) return;

	//double dist;
	//int old_Selec_index;
	//old_Selec_index = Selec_index;

	// FIXME: Implementar PID test on click com novo robot
//	for (int i = 0; i < interface.robot_list.size() && i < 3; i++) {
//		dist = sqrt(pow((interface.imageView.robot_pos[0] - interface.robot_list[i].position.x), 2) +
//					pow((interface.imageView.robot_pos[1] - interface.robot_list[i].position.y), 2));
//		if (dist <= 17) {
//			Selec_index = i;
//			interface.imageView.tar_pos[0] = -1;
//			interface.imageView.tar_pos[1] = -1;
//			interface.robot_list[Selec_index].target = cv::Point(-1, -1);
//			fixed_ball[Selec_index] = false;
//		}
//	}
//	if (Selec_index > -1) {
//		if (sqrt(pow((interface.visionGUI.vision->getBall().x - interface.robot_list[Selec_index].target.x), 2) +
//				 pow((interface.visionGUI.vision->getBall().y - interface.robot_list[Selec_index].target.y), 2)) <=
//			7)
//			fixed_ball[Selec_index] = true;
//
//		if (fixed_ball[Selec_index])
//			interface.robot_list[Selec_index].target = interface.visionGUI.vision->getBall();
//		else
//			interface.robot_list[Selec_index].target = cv::Point(static_cast<int>(interface.imageView.tar_pos[0]),
//																 static_cast<int>(interface.imageView.tar_pos[1]));
//	}
//
//	for (int i = 0; i < interface.robot_list.size() && i < 3; i++) {
//		if (fixed_ball[i])
//			interface.robot_list[i].target = interface.visionGUI.vision->getBall();
//		else {
//			if (sqrt(pow((interface.robot_list[i].position.x - interface.robot_list[i].target.x), 2) +
//					 pow((interface.robot_list[i].position.y - interface.robot_list[i].target.y), 2)) < 15) {
//				interface.robot_list[i].target = cv::Point(-1, -1);
//				interface.imageView.tar_pos[0] = -1;
//				interface.imageView.tar_pos[1] = -1;
//				interface.robot_list[i].Vr = 0;
//				interface.robot_list[i].Vl = 0;
//				interface.robot_list[i].vmax = 0;
//			}
//			if (interface.robot_list[i].target.x != -1 && interface.robot_list[i].target.y != -1) {
//				interface.robot_list[Selec_index].target = cv::Point(static_cast<int>(interface.imageView.tar_pos[0]),
//																	 static_cast<int>(interface.imageView.tar_pos[1]));
//				interface.robot_list[Selec_index].vmax = interface.robot_list[Selec_index].vdefault;
//				interface.robot_list[i].cmdType = VECTOR;
//				interface.robot_list[i].transAngle = atan2(
//						double(interface.robot_list[i].position.y - interface.robot_list[i].target.y),
//						-double(interface.robot_list[i].position.x - interface.robot_list[i].target.x));
//				//interface.robot_list[i].goTo(interface.robot_list[i].target,interface.visionGUI.vision->getBall());
//			} else {
//				interface.robot_list[i].Vr = 0;
//				interface.robot_list[i].Vl = 0;
//			}
//		}
//	}
} // PID_test

void CamCap::warp_transform(cv::Mat imageView) {
	cv::Point2f inputQuad[4];
	cv::Point2f outputQuad[4];
	cv::Mat lambda = cv::Mat::zeros(imageView.rows, imageView.cols, imageView.type());

	inputQuad[0] = cv::Point2f(interface.imageView.warp_mat[0][0] - interface.offsetL,
							   interface.imageView.warp_mat[0][1]);
	inputQuad[1] = cv::Point2f(interface.imageView.warp_mat[1][0] + interface.offsetR,
							   interface.imageView.warp_mat[1][1]);
	inputQuad[2] = cv::Point2f(interface.imageView.warp_mat[2][0] + interface.offsetR,
							   interface.imageView.warp_mat[2][1]);
	inputQuad[3] = cv::Point2f(interface.imageView.warp_mat[3][0] - interface.offsetL,
							   interface.imageView.warp_mat[3][1]);

	outputQuad[0] = cv::Point2f(0, 0);
	outputQuad[1] = cv::Point2f(width - 1, 0);
	outputQuad[2] = cv::Point2f(width - 1, height - 1);
	outputQuad[3] = cv::Point2f(0, height - 1);
	lambda = getPerspectiveTransform(inputQuad, outputQuad);
	warpPerspective(imageView, imageView, lambda, imageView.size());

	if (interface.imageView.adjust_rdy) {
		interface.bt_adjust.set_active(false);
		interface.bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
		interface.adjust_event_flag = false;
		interface.imageView.adjust_event_flag = false;

		for (int i = 0; i < interface.imageView.adjust_mat[0][1]; i++) {
			for (int j = 0; j < 3 * interface.imageView.adjust_mat[0][0]; j++) {
				imageView.at<uchar>(i, j) = 0;
			}
		}

		for (int i = height; i > interface.imageView.adjust_mat[1][1]; i--) {
			for (int j = 0; j < 3 * interface.imageView.adjust_mat[1][0]; j++) {
				imageView.at<uchar>(i, j) = 0;
			}
		}

		for (int i = 0; i < interface.imageView.adjust_mat[2][1]; i++) {
			for (int j = 3 * width; j > 3 * interface.imageView.adjust_mat[2][0]; j--) {
				imageView.at<uchar>(i, j) = 0;
			}
		}

		for (int i = height; i > interface.imageView.adjust_mat[3][1]; i--) {
			for (int j = 3 * width; j > 3 * interface.imageView.adjust_mat[3][0]; j--) {
				imageView.at<uchar>(i, j) = 0;
			}
		}
	}
} // warp_transform


void CamCap::calculate_ball_est() {
	ls_x.addValue(ball.x);
	ls_y.addValue(ball.y);

	ball_est.x = ls_x.estimate(10);
	ball_est.y = ls_y.estimate(10);
}

CamCap::CamCap(int screenW, int screenH, bool isLowRes) : data(0), width(0), height(0), frameCounter(0),
										   screenWidth(screenW), screenHeight(screenH),
										   msg_thread(&CamCap::send_cmd_thread, this),
										   robotGUI(robots, isLowRes),
										   interface(&control.messenger, robots, robotGUI, isLowRes),
										   strategy(attacker, defender, goalkeeper, ball, ball_est),
										   robots {&attacker, &defender, &goalkeeper} {

	ls_x.init(15, 1);
	ls_y.init(15, 1);

	fixed_ball[0] = false;
	fixed_ball[1] = false;
	fixed_ball[2] = false;
	fm.set_label("imageView");
	fm.add(interface.imageView);

	notebook.append_page(interface, "Capture");
	notebook.append_page(interface.visionGUI, "Vision");
	notebook.append_page(control, "Control");
	if (isLowRes)
	{
		// Caso esteja em baixa resolução, será criado uma aba Robot.
		// Caso contrário, robotGUI será colocado na info_box da interface.
		notebook.append_page(robotGUI, "Robot");
	}
	notebook.append_page(strategyGUI, "Strategy");

	for (int i = 0; i < 3; i++) {
		virtual_robots_orientations[i] = 0;
		virtual_robots_positions[i] = cv::Point(200, 480 / 6 + (i + 1) * 480 /
															   6); // !TODO hardcoded, usar variáveis quando possível
	}

	for (auto &i : interface.imageView.adjust_mat) {
		i[0] = -1;
		i[1] = -1;
	}

	camera_vbox.pack_start(fm, false, true, 5);
	camera_vbox.pack_start(info_fm, false, true, 5);
	info_fm.add(interface.info_hbox);

	pack_start(camera_vbox, true, true, 10);
	pack_start(notebook, false, false, 10);

	interface.signal_start().connect(sigc::mem_fun(*this, &CamCap::start_signal));
}

CamCap::~CamCap() {
	con.disconnect();
	interface.imageView.disable_image_show();
	free(data);

	data = nullptr;

	msg_thread.interrupt();
	if (msg_thread.joinable()) msg_thread.join();
}


