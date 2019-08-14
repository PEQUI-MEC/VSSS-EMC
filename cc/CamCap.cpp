#include <Strategy2/Robot2.h>
#include <vsss_msgs/Control.h>
#include "CamCap.hpp"
#include "ImageArt.hpp"

using namespace vision;
using namespace art;

using std::cout;
using std::endl;
using std::vector;

void CamCap::update_positions(const std::map<unsigned int, Vision::RecognizedTag> &tags) {
	for (const auto &robot : teams[static_cast<int>(static_cast<int>(Teams::Real))].robots) {
		auto search = tags.find(robot->tag);
		if (search != tags.end())
			robot->set_pose(search->second.position, search->second.orientation);
	}

	interface.updateRobotLabels();

	const Vision::Ball cv_ball = interface.visionGUI.vision->getBall();
	ball.position = Geometry::from_cv_point(cv_ball.position);
	interface.update_ball_position(ball.position);
}

bool CamCap::start_signal(bool b) {
	if (b) {
		cout << "Start Clicked!" << endl;

		if (simulationGUI.use_simu_button.get_active()) {
			simulationGUI.use_simu_button.set_active(false);
		}

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

	interface.imageView.split_flag = interface.visionGUI.getIsSplitView();
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

	interface.imageView.imageWarp.run(imageView);

	bool is_game_on = interface.get_start_game_flag();
	bool is_test_on_click_on = interface.controlGUI.test_controller.is_active();

	if (interface.CamCalib_flag_event && !is_game_on && !is_test_on_click_on &&
		!interface.visionGUI.vision->flag_cam_calibrated) {

		chessBoardFound = cv::findChessboardCorners(imageView, CHESSBOARD_DIMENSION, foundPoints,
													cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
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
//			if (control.ekf_always_send || is_game_on || interface.imageView.PID_test_flag) {
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
//		if (control.ekf_always_send || is_game_on || interface.imageView.PID_test_flag) {
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

	std::map<unsigned int, Vision::RecognizedTag> tags = interface.visionGUI.vision->run(imageView);

	interface.visionGUI.recorder.run(imageView);

	ball.update_estimate();
	update_positions(tags);

	interface.updateFPS(fps_average);

	if (interface.controlGUI.ekf_always_send || is_game_on || is_test_on_click_on) {
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

	interface.controlGUI.update_dropped_frames();

	// ----------- ESTRATEGIA -----------------//
	if (is_game_on) {
		circle(imageView, Ball_Est, 7, cv::Scalar(255, 140, 0), 2);
//		strategyGUI.strategy.get_uvf_targets( interface.robot_list );

		teams[static_cast<int>(Teams::Real)].strategy.run();

		interface.controlGUI.update_msg_time();
		notify_data_ready(false);

		robotGUI.update_speed_progressBars();
		robotGUI.update_robot_functions();
	} // ---------- TEST ON CLICK --------------//
	else if (is_test_on_click_on) {
		interface.controlGUI.test_controller.run();
		interface.controlGUI.update_msg_time();
		notify_data_ready(false);
	}
	// -----------------------------------------//

	if (!interface.visionGUI.CIELAB_calib_event_flag && !interface.visionGUI.getIsSplitView()) {
		if (chessBoardFound) {
			cv::TermCriteria termCriteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 40, 0.001);
			cv::Mat grayFrame;
			cv::cvtColor(imageView, grayFrame, cv::COLOR_RGB2GRAY);
			cv::cornerSubPix(grayFrame, foundPoints, cv::Size(11, 11), cv::Size(-1, -1), termCriteria);
		}

		if (interface.visionGUI.getIsDrawing()) {
			cv::drawChessboardCorners(imageView, CHESSBOARD_DIMENSION, foundPoints, chessBoardFound);

			interface.imageView.imageArt.draw(imageView,
											  interface.visionGUI.gmm->getSamplePoints(),
											  interface.visionGUI.vision->getBall(),
											  tags,
											  interface.visionGUI.vision->get_adv_robots(),
											  teams[static_cast<int>(Teams::Real)].robots,
											  is_game_on);
		} // if !interface.draw_info_flag
	} // if !draw_info_flag


	if (frameCounter == 10) {
		auto timer_finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = timer_finish - timer_start;
		fps_average = static_cast<int>(10 / elapsed.count());
		frameCounter = 0;
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
		if (ekf_data_ready) {
			for (auto &robot : this->teams[static_cast<int>(Teams::Real)].robots)
				interface.controlGUI.messenger.send_ekf_data(*robot);
			ekf_data_ready = false;
		} else {
			interface.controlGUI.messenger.send_commands(this->teams[static_cast<int>(Teams::Real)].robots);
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

CamCap::CamCap(bool isLowRes, int argc, char **argv) : ball(teams[static_cast<int>(Teams::Real)].ball),
													   data(nullptr), width(0), height(0), frameCounter(0),
													   msg_thread(&CamCap::send_cmd_thread, this),
													   robotGUI(teams, isLowRes, current_team),
													   interface(teams, robotGUI, isLowRes, current_team),
													   simulationGUI(interface, current_team, argc, argv) {


	fm.set_label("imageView");
	fm.add(interface.imageView);

	notebook.append_page(interface, "Capture");
	notebook.append_page(interface.visionGUI, "Vision");
	notebook.append_page(interface.controlGUI, "Control");
	if (isLowRes) {
		// Caso esteja em baixa resolução, será criado uma aba Robot.
		// Caso contrário, robotGUI será colocado na info_box da interface.
		notebook.append_page(robotGUI, "Robot");
	}
	notebook.append_page(strategyGUI, "Strategy");
	notebook.append_page(simulationGUI, "Simulator");

	camera_vbox.pack_start(fm, false, true, 5);
	camera_vbox.pack_start(info_fm, false, true, 5);
	info_fm.add(interface.info_hbox);

	pack_start(camera_vbox, true, true, 10);
	pack_start(notebook, false, false, 10);

	interface.signal_start().connect(sigc::mem_fun(*this, &CamCap::start_signal));
	Glib::signal_idle().connect( sigc::mem_fun(interface, &capture::V4LInterface::update_gui) );
}

CamCap::~CamCap() {
	con.disconnect();
	interface.imageView.disable_image_show();
	free(data);

	data = nullptr;

	msg_thread.interrupt();
	if (msg_thread.joinable()) msg_thread.join();
}