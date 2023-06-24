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
			robot.set_pose(tag.position, tag.orientation, game.yellow_team().inverted_field);
		}
	}

	for (auto &robot : game.blue_team().robots) {
		if (robot.TAG < tags.blue.size()) {
			auto tag = tags.blue[robot.TAG];
			robot.set_pose(tag.position, tag.orientation, game.blue_team().inverted_field);
		}
	}

	interface.updateRobotLabels();

	if (tags.found_ball) {
		game.ball.set_position(Geometry::from_cv_point(tags.ball.position));
		game.ball.update_ls();
	}
	interface.update_ball_position(game.ball.position);
}

void CamCap::set_image_resolution(int width, int height) {
	this->width = width;
	this->height = height;
	visionImage = cv::Mat(height, width, CV_8UC3);
	visionImage = cv::Scalar(0, 0, 0);
	interface.visionGUI.setFrameSize(width, height);
	interface.imageView.set_frame_size(width, height);
	interface.imageView.set_data(visionImage.data);
	interface.imageView.refresh();
}

bool CamCap::start_signal(bool b) {
	if (b) {
		cout << "Start Clicked!" << endl;

		interface.imageView.disable_image_show();

		set_image_resolution(interface.vcap.format_dest.fmt.pix.width,
							 interface.vcap.format_dest.fmt.pix.height);

		// Liberar os botões de edit
		//robotGUI.enable_main_buttons();

//		con = Glib::signal_idle().connect(sigc::mem_fun(*this, &CamCap::capture_and_show));

		interface.gameGUI.use_simulator_button.set_active(false);

		has_camera = true;
	} else {
		has_camera = false;

		cout << "Stop Button Clicked!" << endl;
//		con.disconnect();

		// Travar os botões de edit
//		robotGUI.enable_main_buttons(false);

//		Apaga imagem antiga da GUI
		visionImage = cv::Scalar(0, 0, 0);
		interface.imageView.set_data(visionImage.data);
		interface.imageView.refresh();
	}

	return true;
} // start_signal

void CamCap::joystick_loop() {
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = now - latest_joystick_update;
	if (elapsed.count() > 1.0/30.0) {
		latest_joystick_update = now;
		fps_update();

		auto inverted_team = game.team->get_inverted_robot_adversary();
		auto inverted_adv = game.adversary->get_inverted_robot_adversary();
		if (game.team->using_joystick) {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv, game.ball, game.first_iteration,
											  false, game.team->inverted_field, game.now());
		}
		if (game.adversary->using_joystick) {
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team, game.ball. get_inverted(),
												   game.first_iteration, false, game.adversary->inverted_field, game.now());
		}
		game.first_iteration = false;
		notify_data_ready(false);
	}
}


bool CamCap::run_game_loop() {
	if (!game.is_simulated && has_camera) {
		simulator.process_referee_cmds(false);
		if (game.send_one_command) {
			notify_data_ready(false);
			game.send_one_command = false;
		}
		capture_and_show();
	} else if (!game.is_simulated && !has_camera && game.playing_game
		&& (game.team->using_joystick || game.adversary->using_joystick)) {
		joystick_loop();
	} else if (game.is_simulated) {
		if (has_camera) interface.bt_start.activate();
		if (width != 640 || height != 480) {
			set_image_resolution(640, 480);
		}
		bool ran_iteration = simulator.game_loop();
		if (ran_iteration) {
			fps_update();
			update_simulated_image();
		}
	} else {
//		Apaga imagem antiga da GUI
		visionImage = cv::Scalar(0, 0, 0);
		interface.imageView.set_data(visionImage.data);
		interface.imageView.refresh();
	}

//	Atualiza a GUI
	robotGUI.update_robots();
	interface.updateRobotLabels();
	interface.update_ball_position(game.ball.position);
	if (interface.start_game_bt.get_active() != game.playing_game)
		interface.start_game_bt.set_active(game.playing_game);
	if (interface.gameGUI.use_simulator_button.get_active() != game.is_simulated)
		interface.gameGUI.use_simulator_button.set_active(game.is_simulated);
	if (interface.gameGUI.inverted_button.get_active() != game.team->inverted_field) {
		interface.gameGUI.inverted_button.set_active(game.team->inverted_field);
	}
	
	return true;
}

void CamCap::update_simulated_image() {
	visionImage = cv::Scalar(0, 0, 0);
	interface.imageView.set_data(visionImage.data);
	interface.imageView.refresh();
	auto tags = game.to_tags();
	interface.imageView.imageArt.draw_robots_and_ball(visionImage, tags);
	if (game.team->controlled) {
		for (auto& robot : game.team->robots)
			interface.imageView.imageArt.draw_targets(robot, visionImage, game.team->inverted_field,
													  game.team->robot_color);
	}
	if (game.adversary->controlled) {
		for (auto& robot : game.adversary->robots)
			interface.imageView.imageArt.draw_targets(robot, visionImage, game.adversary->inverted_field,
													  game.adversary->robot_color);
	}
	Geometry::Point ball_later = game.ball.position + (game.ball.velocity * 0.05);
	circle(visionImage, ball_later.to_cv_point(), 7, cv::Scalar(255, 140, 0), 2);
}

bool CamCap::capture_and_show() {
	bool chessBoardFound = false;
	std::vector<cv::Vec2f> foundPoints;

	interface.vcap.grab_rgb(visionImage.data);
	interface.imageView.set_data(visionImage.data);
	interface.imageView.refresh();

	interface.imageView.split_flag = interface.visionGUI.getIsSplitView();

	if (interface.imageView.sector != -1) {
		interface.visionGUI.selectFrame(interface.imageView.sector);
		interface.imageView.sector = -1;
	}

	if (interface.visionGUI.vision->flag_cam_calibrated) {
		cv::Mat temp;
		visionImage.copyTo(temp);
		cv::undistort(temp, visionImage, interface.visionGUI.vision->getcameraMatrix(),
					  interface.visionGUI.vision->getdistanceCoeficents());
	}

	interface.imageView.imageWarp.run(visionImage);

	bool is_test_on_click_on = interface.controlGUI.test_controller.is_active();

	if (interface.CamCalib_flag_event && !game.playing_game && !is_test_on_click_on &&
		!interface.visionGUI.vision->flag_cam_calibrated) {

		chessBoardFound = cv::findChessboardCorners(visionImage, CHESSBOARD_DIMENSION, foundPoints,
													cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
	}

	auto tags = interface.visionGUI.vision->run(visionImage, game.yellow_team().controlled,
												game.blue_team().controlled);

	interface.visionGUI.recorder.run(visionImage);

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

	// ----------- ESTRATEGIA -----------------//
	if (game.playing_game) {
//		strategyGUI.strategy.get_uvf_targets( interface.robot_list );

		auto inverted_team = game.team->get_inverted_robot_adversary();
		auto inverted_adv = game.adversary->get_inverted_robot_adversary();
		if (game.team->inverted_field) {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv, game.ball.get_inverted(), game.first_iteration, false, game.team->inverted_field, game.now());
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team, game.ball, game.first_iteration, false, game.adversary->inverted_field, game.now());
		} else {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv, game.ball, game.first_iteration, false, game.team->inverted_field, game.now());
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team, game.ball.get_inverted(),
												   game.first_iteration, false, game.adversary->inverted_field, game.now());
		}
		game.first_iteration = false;

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
			cv::cvtColor(visionImage, grayFrame, cv::COLOR_RGB2GRAY);
			cv::cornerSubPix(grayFrame, foundPoints, cv::Size(11, 11), cv::Size(-1, -1), termCriteria);
		}

		if (interface.visionGUI.getIsDrawing()) {
			cv::drawChessboardCorners(visionImage, CHESSBOARD_DIMENSION, foundPoints, chessBoardFound);

			interface.imageView.imageArt.draw(visionImage, tags, *game.team, game.playing_game);
		} // if !interface.draw_info_flag
	} // if !draw_info_flag

	fps_update();

	return true;
} // capture_and_show

void CamCap::fps_update() {
	if (frameCounter == 0) {
		timer_start = hrc::now();
	}
	if (frameCounter == 20) {
		auto timer_finish = hrc::now();
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
			if (game.team->controlled) {
				for (auto &robot : game.team->robots)
					interface.controlGUI.messenger.send_ekf_data(robot);
			}
			if (game.adversary->controlled) {
				for (auto &robot : game.adversary->robots)
					interface.controlGUI.messenger.send_ekf_data(robot);
			}
			ekf_data_ready = false;
		} else {
			if (game.team->controlled) {
				interface.controlGUI.messenger.send_commands(game.team->robots);
			}
			if (game.adversary->controlled) {
				interface.controlGUI.messenger.send_commands(game.adversary->robots);
			}
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

CamCap::CamCap(bool isLowRes, int simulation_id) : game(simulation_id),
								simulator(game),
								visionImage(height, width, CV_8UC3),
								frameCounter(0),
								msg_thread(&CamCap::send_cmd_thread, this),
								robotGUI(game, isLowRes),
								interface(game, robotGUI, isLowRes) {
	fm.set_label("imageView");
	fm.add(interface.imageView);

	latest_joystick_update = std::chrono::high_resolution_clock::now();

	set_image_resolution(640, 480);

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

	//interface.__event_bt_start_clicked();

//	Remove focus from last created GUI item
	interface.visionGUI.bt_record_video.grab_focus();

	on_idle = Glib::signal_idle().connect(sigc::mem_fun(*this, &CamCap::run_game_loop));

	interface.__event_bt_quick_load_clicked();
}

CamCap::~CamCap() {
	con.disconnect();
	interface.imageView.disable_image_show();

	msg_thread.interrupt();
	if (msg_thread.joinable()) msg_thread.join();
}

