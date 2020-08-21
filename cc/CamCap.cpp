#include <Strategy2/Robot2.h>
#include "CamCap.hpp"
#include "ImageArt.hpp"

using namespace vision;
using namespace art;

using std::cout;
using std::endl;
using std::vector;

void CamCap::update_positions(Tags &tags) {
	for (auto &robot : game.yellow_team().robots) {
		if (robot.TAG < tags.yellow.size()) {
			auto tag = tags.yellow[robot.TAG];
			robot.set_pose(tag.position, tag.orientation);
		}
	}

	for (auto &robot : game.blue_team().robots) {
		if (robot.TAG < tags.blue.size()) {
			auto tag = tags.blue[robot.TAG];
			robot.set_pose(tag.position, tag.orientation);
		}
	}

	interface.updateRobotLabels();

	if (tags.found_ball) game.ball.set_position(Geometry::from_cv_point(tags.ball.position));
	interface.update_ball_position(game.ball.position);
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

		interface.imageView.set_frame_size(width, height);
//		con = Glib::signal_idle().connect(sigc::mem_fun(*this, &CamCap::capture_and_show));

		interface.__event_bt_quick_load_clicked();

		interface.gameGUI.use_simulator_button.set_active(false);

		has_camera = true;
	} else {
		has_camera = false;

		cout << "Stop Button Clicked!" << endl;
//		con.disconnect();

		// Travar os botões de edit
//		robotGUI.enable_main_buttons(false);

//		Apaga imagem antiga da GUI
		if (data) {
			cv::Mat imageView(height, width, CV_8UC3, data);
			imageView = cv::Scalar(0, 0, 0);
			interface.imageView.set_data(data);
			interface.imageView.refresh();
		}
	}

	return true;
} // start_signal

bool CamCap::run_game_loop() {
	if (!game.is_simulated && has_camera) {
		capture_and_show();
	} else if (game.is_simulated){
		if (has_camera) interface.bt_start.activate();
		simulated_game_loop();
	} else {
//		Sem camera e nem simulator, apenas atualiza a GUI
		robotGUI.update_robots();
		interface.updateRobotLabels();
		interface.update_ball_position(game.ball.position);
	}
	return true;
}

void CamCap::simulated_game_loop() {
	if(!simulator.new_data) return;
	std::lock_guard<std::mutex> guard(simulator.data_mutex);

	simulator.update_robots(game);
	robotGUI.update_robots();
	interface.updateRobotLabels();
	interface.update_ball_position(game.ball.position);
	fps_update();

	if (game.playing_game) {
		auto inverted_team = game.team->get_inverted_robot_positions();
		auto inverted_adv = game.adversary->get_inverted_robot_positions();
		if (game.team->inverted_field) {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv, game.ball.get_inverted());
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team, game.ball);
		} else {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv, game.ball);
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team,
												   game.ball.get_inverted());
		}

		if (game.team->controlled) {
			simulator.send_commands(*game.team.get());
		}
		if (game.adversary->controlled) {
			simulator.send_commands(*game.adversary.get());
		}
	}
}

bool CamCap::capture_and_show() {
	bool chessBoardFound = false;
	std::vector<cv::Vec2f> foundPoints;

	if (!data) return false;

	interface.vcap.grab_rgb(data);
	interface.imageView.set_data(data);
	interface.imageView.refresh();

	cv::Mat imageView(height, width, CV_8UC3, data);

	interface.imageView.split_flag = interface.visionGUI.getIsSplitView();

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

	bool is_test_on_click_on = interface.controlGUI.test_controller.is_active();

	if (interface.CamCalib_flag_event && !game.playing_game && !is_test_on_click_on &&
		!interface.visionGUI.vision->flag_cam_calibrated) {

		chessBoardFound = cv::findChessboardCorners(imageView, CHESSBOARD_DIMENSION, foundPoints,
													cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
	}

	auto tags = interface.visionGUI.vision->run(imageView, game.yellow_team().controlled,
												game.blue_team().controlled);

	interface.visionGUI.recorder.run(imageView);

	update_positions(tags);

	interface.updateFPS(fps_average);

	if (interface.controlGUI.ekf_always_send || game.playing_game || is_test_on_click_on) {
		notify_data_ready(true);
	}

	if (interface.visionGUI.getIsSplitView()) {
		interface.imageView.set_data(interface.visionGUI.vision->getSplitFrame().clone().data);
		interface.imageView.refresh();
	} else if (interface.visionGUI.CIELAB_calib_event_flag) {
		interface.imageView.set_data(interface.visionGUI.vision->getThreshold(interface.visionGUI.Img_id).data);
		interface.imageView.refresh();
	}

	interface.controlGUI.update_dropped_frames();

	// ----------- ESTRATEGIA -----------------//
	if (game.playing_game) {
		circle(imageView, game.ball.estimative.to_cv_point(), 7, cv::Scalar(255, 140, 0), 2);
//		strategyGUI.strategy.get_uvf_targets( interface.robot_list );

		auto inverted_team = game.team->get_inverted_robot_positions();
		auto inverted_adv = game.adversary->get_inverted_robot_positions();
		if (game.team->inverted_field) {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv, game.ball.get_inverted());
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team, game.ball);
		} else {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv, game.ball);
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team, game.ball.get_inverted());
		}

		interface.controlGUI.update_msg_time();
		notify_data_ready(false);

	} // ---------- TEST ON CLICK --------------//
	else if (is_test_on_click_on) {
		interface.controlGUI.test_controller.run();
		interface.controlGUI.update_msg_time();
		notify_data_ready(false);
	}

	robotGUI.update_robot_ids();
	robotGUI.update_speed_progressBars();
	robotGUI.update_robot_functions();
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

			interface.imageView.imageArt.draw(imageView, tags, *game.team, game.playing_game);
		} // if !interface.draw_info_flag
	} // if !draw_info_flag

	fps_update();

	return true;
} // capture_and_show

void CamCap::fps_update() {
	if (frameCounter == 0) {
		timer_start = std::chrono::high_resolution_clock::now();
	}
	if (frameCounter == 20) {
		auto timer_finish = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = timer_finish - timer_start;
		fps_average = 20 / elapsed.count();
		interface.updateFPS((int) std::round(fps_average));
		frameCounter = 0;
	} else {
		frameCounter++;
	}
}

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
			for (auto &robot : game.team->robots)
				interface.controlGUI.messenger.send_ekf_data(robot);
			ekf_data_ready = false;
		} else {
			interface.controlGUI.messenger.send_commands(game.team->robots);
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

CamCap::CamCap(bool isLowRes) : data(nullptr), width(0), height(0), frameCounter(0),
								msg_thread(&CamCap::send_cmd_thread, this),
								robotGUI(game, isLowRes),
								interface(game, robotGUI, isLowRes) {

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

	camera_vbox.pack_start(fm, false, true, 5);
	camera_vbox.pack_start(info_fm, false, true, 5);
	info_fm.add(interface.info_hbox);

	pack_start(camera_vbox, true, true, 10);
	pack_start(notebook, false, false, 10);

	interface.signal_start().connect(sigc::mem_fun(*this, &CamCap::start_signal));


	interface.__event_bt_start_clicked();

//	Remove focus from last created GUI item
	interface.visionGUI.bt_record_video.grab_focus();

	on_idle = Glib::signal_idle().connect(sigc::mem_fun(*this, &CamCap::run_game_loop));
}

CamCap::~CamCap() {
	con.disconnect();
	interface.imageView.disable_image_show();
	free(data);

	data = nullptr;

	msg_thread.interrupt();
	if (msg_thread.joinable()) msg_thread.join();
}

