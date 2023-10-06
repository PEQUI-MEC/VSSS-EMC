#include "controlGUI.hpp"

ControlGUI::ControlGUI(Game &game)
	: test_controller(game) {
	adjust_widgets_state();

//	Sempre deve haver pelo menos um XbeeSelectGUI
	add_xbee_select_gui();

	_create_radio_frame();
	_create_commands_frame();
	_create_test_on_click_frame();
	_create_status_frame();

	update_ack_interface();
}

void ControlGUI::_send_command() {
	std::string cmd = commands_cmd_entry.get_text();
	messenger.send_old_format(cmd);
}

// Gets battery % and robot id to update a single robot's battery status
void ControlGUI::updateInterfaceStatus(double battery, double voltage, int id) {
	auto text = std::to_string(battery).substr(0, 4) + "%, " + std::to_string(voltage).substr(0, 4) + "V";
	if (voltage > 6.6) {
		status_img[id].set("img/online.png");
		battery_bar[id].set_fraction(battery / 100);
		status_lb[id].set_text(text);
	} else if (battery > 6.4) {
		status_img[id].set("img/critical.png");
		battery_bar[id].set_fraction(battery / 100);
		status_lb[id].set_text(text);
	} else {
		status_img[id].set("img/zombie.png");
		battery_bar[id].set_fraction(0.0);
		battery_bar[id].set_text("0%");
		status_lb[id].set_text("DEAD");
	}
}

// update the battery status of all robots
void ControlGUI::_robot_status() {
	std::string dateString;
	time_t tt;

	// define last update time
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	tt = std::chrono::system_clock::to_time_t(now);
	dateString.append("Last Update: ");
	dateString.append(std::string(ctime(&tt)).substr(0, 24));
	lastUpdate_lb.set_text(dateString);

	auto batteries = messenger.read_batteries();

	// update robot status
	for (int i = 0; i < TOTAL_ROBOTS; ++i) {
		char id = get_robot_id(i);
		if (batteries.find(id) != batteries.end()) {
			double voltage = batteries[id];
			double battery = (voltage - 6.4) / (8.4 - 6.4) * 100.0;
			std::cout << "Robot " << id << " battery: " << battery << "%, " << voltage << "V" << std::endl;
			updateInterfaceStatus(battery, voltage, i);
		} else {
			status_img[i].set("img/offline.png");
			battery_bar[i].set_fraction(0.0);
			battery_bar[i].set_text("0%");
			status_lb[i].set_text("Offline");
		}
	}
}

void ControlGUI::_send_test() {
	int pos = commands_robots_cb.get_active_row_number();
	std::string cmd = "W" + messenger.rounded_str((commands_L_hsc.get_value()))
		 + ";" + messenger.rounded_str(commands_R_hsc.get_value());

	if (pos == -1) {
		return;
	} else if (pos != 6) {
		char id = get_robot_id(pos);
		messenger.send_msg(id, cmd);
	} else {
		for (int i = 0; i < 6; ++i) {
			char id = get_robot_id(i);
			messenger.send_msg(id, cmd);
		}
	}
}

char ControlGUI::get_robot_id(int pos) {
	return char(65 + pos);
}

void ControlGUI::_create_status_frame() {

	pack_start(status_fm, false, true, 0);
	status_fm.set_label("Robot Status");
	status_fm.add(status_grid);
	status_fm.set_border_width(10);

	status_grid.set_border_width(10);
	status_grid.set_column_spacing(10);
	status_grid.set_halign(Gtk::ALIGN_CENTER);

	bt_Robot_Status.set_label("Update");
	status_grid.attach(bt_Robot_Status, 0, 0, 1, 1);
	lastUpdate_lb.set_text("Last Update: -");
	lastUpdate_lb.set_valign(Gtk::ALIGN_BASELINE);
	status_grid.attach(lastUpdate_lb, 1, 0, 3, 1);
	bt_reset_ack.set_label("Reset ACKs");
	status_grid.attach(bt_reset_ack, 4, 0, 1, 1);

	std::vector<std::string> name;
	name.emplace_back("Robot A");
	name.emplace_back("Robot B");
	name.emplace_back("Robot C");
	name.emplace_back("Robot D");
	name.emplace_back("Robot E");
	name.emplace_back("Robot F");

	for (int i = 0; i < TOTAL_ROBOTS; i++) {
		status_img[i].set("img/offline.png");
		status_grid.attach(status_img[i], 0, i + 1, 1, 1);
		robots_lb[i].set_text(name.at(i));
		status_grid.attach(robots_lb[i], 1, i + 1, 1, 1);
		battery_bar[i].set_valign(Gtk::ALIGN_CENTER);
		status_grid.attach(battery_bar[i], 2, i + 1, 1, 1);
		status_lb[i].set_text("Offline");
		status_grid.attach(status_lb[i], 3, i + 1, 1, 1);
		dropped_frames[i].set_label("Lost ACKs: 0.00%, Total: 0");
		status_grid.attach(dropped_frames[i], 4, i + 1, 1, 1);
	}

	// Conectar os sinais para o acontecimento dos eventos
	bt_Robot_Status.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_robot_status));
	bt_reset_ack.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::reset_lost_acks));
}

void ControlGUI::update_ack_interface() {
	bool is_active = radio_acks_chbt.get_active();
	// messenger.set_ack_enabled(is_active);
	if (is_active) {
		for (Gtk::Label &label : dropped_frames) {
			label.show();
		}
		bt_reset_ack.show();
	} else {
		for (Gtk::Label &label : dropped_frames) {
			label.hide();
		}
		bt_reset_ack.hide();
	}
}

void ControlGUI::ekf_always_send_enable() {
	ekf_always_send = radio_ekf_chbt.get_active();
//	if(!ekf_always_send) messenger.ekf_data_file << std::flush;
}

void ControlGUI::update_dropped_frames() {
	for (int i = 0; i < TOTAL_ROBOTS; ++i) {
		char id = get_robot_id(i);
		// ack_count ack = messenger.get_ack_count(id);
		// if (ack.total == -1) return;

		// std::stringstream stream;
		// stream << "Lost ACKs: ";
		// stream << std::fixed << std::setprecision(2) << ack.lost_rate << "%";
		// stream << ", Total: " << ack.total;
		// dropped_frames[i].set_text(stream.str());
	}
}

void ControlGUI::reset_lost_acks() {
	// messenger.reset_lost_acks();
	update_dropped_frames();
}

void ControlGUI::set_frameskipper() {
	int frames;
	try {
		frames = static_cast<int>(radio_skip_sbt.get_value());
	} catch (...) {
		return;
	}
	messenger.set_frameskipper(frames);
	std::string time_str = std::to_string(33 * (frames + 1));
	radio_time_lbl.set_label("Time between CMDs: " + time_str + " ms");
}

void ControlGUI::update_msg_time() {
	acc_time += messenger.get_time();
	time_count++;
	if (acc_time > 500) {
		std::ostringstream ss;
		ss << round(acc_time / time_count * 100) / 100;
		radio_time_lbl.set_label("Time between CMDs: " + ss.str() + " ms");
		acc_time = 0;
		time_count = 0;
	}
}

void ControlGUI::_create_test_on_click_frame() {
	// Setup labels
	test_default_lb[0].set_label("Command:");
	test_default_lb[1].set_label("Angle:");
	test_default_lb[0].set_halign(Gtk::ALIGN_END);
	test_default_lb[1].set_halign(Gtk::ALIGN_END);
	test_tip_lb.set_label("Tip: Left-click on image to select robot. Right-click to set the target.");
	test_start_bt.set_label(test_start_txt);
	test_set_bt.set_label("Set");

	pack_start(test_frame, false, true, 0);
	test_frame.set_label("Test On Click");
	test_frame.set_border_width(10);
	test_frame.add(test_vbox);
	test_vbox.pack_start(test_grid, false, true, 12);

	test_grid.set_border_width(5);
	test_grid.set_column_spacing(10);
	test_grid.set_row_spacing(5);
	test_grid.set_halign(Gtk::ALIGN_CENTER);
	test_grid.attach(test_start_bt, 0, 0, 1, 1);
	test_grid.attach(test_default_lb[0], 1, 0, 1, 1);
	test_grid.attach(test_command_cb, 2, 0, 2, 1);
	test_grid.attach(test_default_lb[1], 1, 2, 1, 1);
	test_grid.attach(test_angle_scale, 2, 2, 1, 1);
	test_grid.attach(test_set_bt, 3, 2, 1, 1);
	test_grid.attach(test_tip_lb, 0, 3, 4, 1);

	test_command_cb.append("Stop (None)");
	test_command_cb.append("Go to target direction (Vector)");
	test_command_cb.append("Set Robot Orientation (Orientation)");
	test_command_cb.append("Go to target pose (UVF)");
	test_command_cb.append("Go to target position (Position)");
	test_command_cb.set_active(0);

	test_angle_scale.set_digits(0);
	test_angle_scale.set_increments(1, 5);
	test_angle_scale.set_range(-180, 180);
	test_angle_scale.set_value(0);
	test_angle_scale.set_value_pos(Gtk::POS_TOP);
	test_angle_scale.set_draw_value();
	test_angle_scale.set_size_request(175, -1);

	test_start_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_test_start_bt_event));
	test_command_cb.signal_changed().connect(sigc::mem_fun(*this, &ControlGUI::_test_command_changed_event));
	test_set_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_test_send_bt_event));
}

void ControlGUI::_test_start_bt_event() {
	bool is_active = test_start_bt.get_active();

	test_controller.set_active(is_active);

	if (is_active) {
		Command command = test_controller.get_command();

		if (command == Command::Vector || command == Command::Orientation
			|| command == Command::UVF) {
			test_angle_scale.set_state(Gtk::STATE_NORMAL);
			test_set_bt.set_state(Gtk::STATE_NORMAL);
		}

		test_command_cb.set_state(Gtk::STATE_NORMAL);
		test_start_bt.set_label(test_stop_txt);
	} else {
		test_command_cb.set_state(Gtk::STATE_INSENSITIVE);
		test_angle_scale.set_state(Gtk::STATE_INSENSITIVE);
		test_set_bt.set_state(Gtk::STATE_INSENSITIVE);
		test_start_bt.set_label(test_start_txt);
	}
}

void ControlGUI::_test_command_changed_event() {
	// 0: None
	// 1: Vector
	// 2: Orientation
	// 3: UVF
	// 4: Position
	switch (test_command_cb.get_active_row_number()) {
		case 0:
			test_controller.set_command(Command::None);
			test_angle_scale.set_state(Gtk::STATE_INSENSITIVE);
			test_set_bt.set_state(Gtk::STATE_INSENSITIVE);
			break;
		case 1:
			test_controller.set_command(Command::Vector);
			test_angle_scale.set_state(Gtk::STATE_NORMAL);
			test_set_bt.set_state(Gtk::STATE_NORMAL);
			break;
		case 2:
			test_controller.set_command(Command::Orientation);
			test_angle_scale.set_state(Gtk::STATE_NORMAL);
			test_set_bt.set_state(Gtk::STATE_NORMAL);
			break;
		case 3:
			test_controller.set_command(Command::UVF);
			test_angle_scale.set_state(Gtk::STATE_NORMAL);
			test_set_bt.set_state(Gtk::STATE_NORMAL);
			break;
		default:
			test_controller.set_command(Command::Position);
			test_angle_scale.set_state(Gtk::STATE_INSENSITIVE);
			test_set_bt.set_state(Gtk::STATE_INSENSITIVE);

	}
	_test_send_bt_event();
}

void ControlGUI::_test_send_bt_event() {
	test_controller.set_orientation(test_angle_scale.get_value());
}

void ControlGUI::stop_test_on_click() {
	test_controller.set_active(false);
	test_start_bt.set_active(false);
	test_start_bt.set_state(Gtk::STATE_INSENSITIVE);
	test_angle_scale.set_state(Gtk::STATE_INSENSITIVE);
	test_set_bt.set_state(Gtk::STATE_INSENSITIVE);
}

void ControlGUI::_create_radio_frame() {
	pack_start(radio_fm, false, true, 0);

	radio_fm.add(radio_options_grid);

	radio_options_grid.attach(xbee_select_vbox, 0, 0, 5, 1);
	radio_options_grid.attach(add_select_xbee_bt, 1, 1, 1, 1);
	radio_options_grid.attach(remove_select_xbee_bt, 2, 1, 1, 1);
	radio_options_grid.attach(radio_skip_lbl, 0, 2, 1, 1);
	radio_options_grid.attach(radio_skip_sbt, 1, 2, 1, 1);
	radio_options_grid.attach(radio_time_lbl, 2, 2, 3, 1);
	radio_options_grid.attach(radio_ekf_chbt, 0, 3, 2, 1);
	radio_options_grid.attach(radio_acks_chbt, 2, 3, 2, 1);

	//Frame
	radio_fm.set_border_width(10);

	//Grid
	radio_options_grid.set_border_width(5);
	radio_options_grid.set_column_spacing(10);
	radio_options_grid.set_row_spacing(5);
	radio_options_grid.set_halign(Gtk::ALIGN_CENTER);

	// Labels
	radio_fm.set_label("Radio Connection");
	radio_skip_lbl.set_label("Skip frames:");
	radio_skip_lbl.set_halign(Gtk::ALIGN_END);
	radio_time_lbl.set_label("Time between CMDs: " + std::to_string((messenger.get_frameskipper() + 1) * 33) + " ms");
	radio_ekf_chbt.set_label("Always send EKF data");
	radio_acks_chbt.set_label("Receive acks");
	add_select_xbee_bt.set_label("Add XBee");
	remove_select_xbee_bt.set_label("Remove XBee");


	// Spin button
	radio_skip_sbt.set_range(0, 10);
	radio_skip_sbt.set_increments(1, 2);
	radio_skip_sbt.set_value(0);

	// Event handlers
	add_select_xbee_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::add_xbee_select_gui));
	remove_select_xbee_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::remove_xbee_select_gui));
	radio_skip_sbt.signal_value_changed().connect(sigc::mem_fun(*this, &ControlGUI::set_frameskipper));
	radio_acks_chbt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::update_ack_interface));
	radio_ekf_chbt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::ekf_always_send_enable));
}

void ControlGUI::_create_commands_frame() {
	pack_start(commands_fm, false, true, 0);

	commands_fm.add(commands_grid);

	commands_grid.attach(commands_speed_lb, 0, 0, 1, 1);
	commands_grid.attach(commands_L_lb, 1, 0, 1, 1);
	commands_grid.attach(commands_L_hsc, 2, 0, 1, 1);
	commands_grid.attach(commands_R_lb, 3, 0, 1, 1);
	commands_grid.attach(commands_R_hsc, 4, 0, 1, 1);
	commands_grid.attach(commands_to_lb, 5, 0, 1, 1);
	commands_grid.attach(commands_robots_cb, 6, 0, 1, 1);
	commands_grid.attach(commands_send_speed_bt, 7, 0, 1, 1);
	commands_grid.attach(radio_rawcmd_lbl, 0, 1, 1, 1);
	commands_grid.attach(commands_cmd_entry, 1, 1, 6, 1);
	commands_grid.attach(commands_send_cmd_bt, 7, 1, 1, 1);

	//Frame
	commands_fm.set_border_width(10);

	//Grid
	commands_grid.set_column_spacing(10);
	commands_grid.set_row_spacing(5);
	commands_grid.set_border_width(10);
	commands_grid.set_halign(Gtk::ALIGN_CENTER);

	//Labels
	commands_fm.set_label("Commands");
	commands_speed_lb.set_text("Wheels Speed:");
	commands_speed_lb.set_halign(Gtk::ALIGN_END);
	commands_L_lb.set_text("Left");
	commands_R_lb.set_text("Right");
	commands_to_lb.set_text("to");
	commands_send_speed_bt.set_label("Send");
	radio_rawcmd_lbl.set_text("Command:");
	radio_rawcmd_lbl.set_halign(Gtk::ALIGN_END);
	commands_send_cmd_bt.set_label("Send");

	//HScale
	commands_L_hsc.set_increments(0.1, 0.5);
	commands_L_hsc.set_range(-1.4, 1.4);
	commands_L_hsc.set_size_request(50, -1);
	commands_L_hsc.set_value(0.0);
	commands_R_hsc.set_increments(0.1, 0.5);
	commands_R_hsc.set_range(-1.4, 1.4);
	commands_R_hsc.set_size_request(50, -1);
	commands_R_hsc.set_value(0.0);

	//ComboBoxText
	commands_robots_cb.append("Robot A");
	commands_robots_cb.append("Robot B");
	commands_robots_cb.append("Robot C");
	commands_robots_cb.append("Robot D");
	commands_robots_cb.append("Robot E");
	commands_robots_cb.append("Robot F");
	commands_robots_cb.append("All Robots");
	commands_robots_cb.set_active(6); // All Robots

	//Event Handlers
	commands_send_speed_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_send_test));
	commands_send_cmd_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_send_command));


}

void ControlGUI::adjust_widgets_state() {
	if (messenger.has_serial()) {
		//Commands Frame
		commands_L_hsc.set_state(Gtk::STATE_NORMAL);
		commands_R_hsc.set_state(Gtk::STATE_NORMAL);
		commands_send_cmd_bt.set_state(Gtk::STATE_NORMAL);
		commands_send_speed_bt.set_state(Gtk::STATE_NORMAL);
		commands_cmd_entry.set_state(Gtk::STATE_NORMAL);
		commands_robots_cb.set_state(Gtk::STATE_NORMAL);

		//Radio Frame
		radio_skip_sbt.set_state(Gtk::STATE_NORMAL);
		radio_ekf_chbt.set_state(Gtk::STATE_NORMAL);
		radio_acks_chbt.set_state(Gtk::STATE_NORMAL);

		//Test On Click Frame
		test_start_bt.set_state(Gtk::STATE_NORMAL);
		test_angle_scale.set_state(Gtk::STATE_NORMAL);
		test_set_bt.set_state(Gtk::STATE_NORMAL);

		//Status Frame
		bt_Robot_Status.set_state(Gtk::STATE_NORMAL);

	} else {
		//Commands Frame
		commands_L_hsc.set_state(Gtk::STATE_INSENSITIVE);
		commands_R_hsc.set_state(Gtk::STATE_INSENSITIVE);
		commands_send_cmd_bt.set_state(Gtk::STATE_INSENSITIVE);
		commands_send_speed_bt.set_state(Gtk::STATE_INSENSITIVE);
		commands_cmd_entry.set_state(Gtk::STATE_INSENSITIVE);
		commands_robots_cb.set_state(Gtk::STATE_INSENSITIVE);

//		Radio frame
		radio_skip_sbt.set_state(Gtk::STATE_INSENSITIVE);
		radio_ekf_chbt.set_state(Gtk::STATE_INSENSITIVE);
		radio_acks_chbt.set_state(Gtk::STATE_INSENSITIVE);

		//Test On Click Frame
		test_start_bt.set_state(Gtk::STATE_INSENSITIVE);
		test_command_cb.set_state(Gtk::STATE_INSENSITIVE);
		test_angle_scale.set_state(Gtk::STATE_INSENSITIVE);
		test_set_bt.set_state(Gtk::STATE_INSENSITIVE);

		//Status Frame
		bt_Robot_Status.set_state(Gtk::STATE_INSENSITIVE);
	}

//	Xbee select list
	for (auto& xbee_select : xbee_select_list) {
		if (xbee_select.has_xbee()) {
			xbee_select.connect_bt.set_state(Gtk::STATE_INSENSITIVE);
			xbee_select.xbee_cb.set_state(Gtk::STATE_INSENSITIVE);
			xbee_select.refresh_bt.set_label("Stop");
		} else {
			xbee_select.connect_bt.set_state(Gtk::STATE_NORMAL);
			xbee_select.xbee_cb.set_state(Gtk::STATE_NORMAL);
			xbee_select.refresh_bt.set_label("Refresh");
			xbee_select.update_combobox();
		}
	}

//	Não permite remover GUI de seleção de xbees caso haja apenas uma
	if (xbee_select_list.size() > 1) {
		remove_select_xbee_bt.set_state(Gtk::STATE_NORMAL);
	} else {
		remove_select_xbee_bt.set_state(Gtk::STATE_INSENSITIVE);
	}
}

void ControlGUI::add_xbee_select_gui() {
	auto& xbee_select = xbee_select_list.emplace_back(messenger);
	xbee_select_vbox.pack_start(xbee_select, false, false, 5);
	xbee_select.update_widgets_state_signal.connect(sigc::mem_fun(this, &ControlGUI::adjust_widgets_state));
	xbee_select.refresh_xbee_cb();
	xbee_select.auto_start_xbee();
}

void ControlGUI::remove_xbee_select_gui() {
	xbee_select_vbox.remove(xbee_select_list.back());
	xbee_select_list.pop_back();
}
