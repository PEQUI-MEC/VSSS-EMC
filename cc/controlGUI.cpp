#include "controlGUI.hpp"

ControlGUI::ControlGUI(const std::array<Robot2 *, 3> &robots) : test_controller(robots) {
	// Adicionar o frame do Serial e sua VBOX
	pack_start(Top_hbox, false, true, 5);
	Top_hbox.pack_start(Serial_fm, true, true, 0);
	Serial_fm.set_label("Serial");
	Serial_fm.add(Serial_vbox);

	Serial_hbox[0].pack_start(cb_serial, false, true, 5);
	Serial_hbox[0].pack_start(bt_Serial_Start, false, true, 5);
	Serial_hbox[0].pack_start(bt_Serial_Refresh, false, true, 5);
	Serial_vbox.pack_start(Serial_hbox[0], false, true, 5);

	Serial_hbox[1].pack_start(cb_test, false, true, 5);
	Serial_hbox[1].pack_start(Tbox_V1, false, true, 5);
	Serial_hbox[1].pack_start(Tbox_V2, false, true, 5);
	Serial_hbox[1].pack_start(bt_Serial_test, false, true, 5);
	Serial_vbox.pack_start(Serial_hbox[1], false, true, 5);

	Serial_hbox[2].pack_start(send_cmd_box, false, true, 5);
	Serial_hbox[2].pack_start(bt_send_cmd, false, true, 5);
	send_cmd_box.set_width_chars(25);
	bt_send_cmd.set_label("Send Command");

	ack_enable_label.set_label("Enable ACKs");
	Serial_hbox[2].pack_start(ack_enable_button, false, true, 5);
	Serial_hbox[2].pack_start(ack_enable_label, false, true, 0);

	Serial_vbox.pack_start(Serial_hbox[2], false, true, 5);

	bt_set_frameskip.set_label("Set skipped frames");
	int frameskipper = messenger.get_frameskipper();
	time_msgs.set_label("Time between CMDs: " + std::to_string((frameskipper + 1) * 33) + " ms");
	entry_set_frameskip.set_text(std::to_string(frameskipper));
	Serial_hbox[3].pack_start(entry_set_frameskip, false, false, 5);
	Serial_hbox[3].pack_start(bt_set_frameskip, false, false, 5);
	Serial_hbox[3].pack_start(time_msgs, false, false, 5);
	Serial_vbox.pack_start(Serial_hbox[3], false, false, 5);

	ekf_always_send_label.set_label("Always send ekf data");
	Serial_hbox[4].pack_start(ekf_always_send_button, false, false, 5);
	Serial_hbox[4].pack_start(ekf_always_send_label, false, false, 0);
	Serial_vbox.pack_start(Serial_hbox[4], false, false, 5);

	Tbox_V1.set_max_length(6);
	Tbox_V2.set_max_length(6);
	Tbox_V1.set_width_chars(6);
	Tbox_V2.set_width_chars(6);
	cb_test.append("Robot A");
	cb_test.append("Robot B");
	cb_test.append("Robot C");
	cb_test.append("Robot D");
	cb_test.append("Robot E");
	cb_test.append("Robot F");
	cb_test.append("All");

	cb_test.set_active(6); // ALL
	Tbox_V1.set_text("0.8");
	Tbox_V2.set_text("0.8");

	bt_Serial_Start.set_state(Gtk::STATE_NORMAL);
	cb_serial.set_state(Gtk::STATE_NORMAL);

	Tbox_V1.set_state(Gtk::STATE_INSENSITIVE);
	Tbox_V2.set_state(Gtk::STATE_INSENSITIVE);
	bt_Serial_Refresh.set_state(Gtk::STATE_INSENSITIVE);
	bt_Serial_test.set_state(Gtk::STATE_INSENSITIVE);
	cb_test.set_state(Gtk::STATE_INSENSITIVE);
	bt_Robot_Status.set_state(Gtk::STATE_INSENSITIVE);
	bt_reset_ack.set_state(Gtk::STATE_INSENSITIVE);

	bt_Serial_Start.set_label("Start");
	bt_Serial_Refresh.set_label("Refresh");

	bt_Serial_test.set_label("Send");

	_update_cb_serial();

	update_ack_interface();
	
	_create_test_on_click_frame();
	_create_status_frame();

	auto_start_serial();

	// Conectar os sinais para o acontecimento dos eventos
	bt_Serial_test.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_send_test));
	bt_Serial_Refresh.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_update_cb_serial));
	bt_Serial_Start.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_start_serial));
	bt_Robot_Status.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_robot_status));
	bt_reset_ack.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::reset_lost_acks));
	bt_send_cmd.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_send_command));
	ack_enable_button.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::update_ack_interface));
	ekf_always_send_button.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::ekf_always_send_enable));
	bt_set_frameskip.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::set_frameskipper));
}

void ControlGUI::_send_command() {
	std::string cmd = send_cmd_box.get_text();
	messenger.send_old_format(cmd);
}

// Gets battery % and robot id to update a single robot's battery status
void ControlGUI::updateInterfaceStatus(double battery, int id) {
	if (battery > 20) {
		status_img[id].set("img/online.png");
		battery_bar[id].set_fraction(battery / 100);
		status_lb[id].set_text(std::to_string(battery).substr(0, 5) + "%");
	} else if (battery > 0) {
		status_img[id].set("img/critical.png");
		battery_bar[id].set_fraction(battery / 100);
		status_lb[id].set_text(std::to_string(battery).substr(0, 5) + "%");
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

	// update robot status
	for (int i = 0; i < TOTAL_ROBOTS; ++i) {
		char id = get_robot_id(i);
		double battery = messenger.get_battery(id);
		if (battery != -1) {
			updateInterfaceStatus(battery, i);
		} else {
			status_img[i].set("img/offline.png");
			battery_bar[i].set_fraction(0.0);
			battery_bar[i].set_text("0%");
			status_lb[i].set_text("Offline");
		}
	}
}

void ControlGUI::auto_start_serial() {
	if (xbee_connections == 1) {
		_start_serial();
	}
}

void ControlGUI::_start_serial() {
	Glib::ustring serial = cb_serial.get_active_text();
	if (serial.empty()) return;

	messenger.start_xbee(serial);

	bt_Serial_Start.set_state(Gtk::STATE_INSENSITIVE);
	cb_serial.set_state(Gtk::STATE_INSENSITIVE);

	Tbox_V1.set_state(Gtk::STATE_NORMAL);
	Tbox_V2.set_state(Gtk::STATE_NORMAL);
	bt_Serial_Refresh.set_state(Gtk::STATE_NORMAL);
	bt_Serial_test.set_state(Gtk::STATE_NORMAL);
	cb_test.set_state(Gtk::STATE_NORMAL);
	bt_Robot_Status.set_state(Gtk::STATE_NORMAL);
	bt_reset_ack.set_state(Gtk::STATE_NORMAL);
	test_start_bt.set_state(Gtk::STATE_NORMAL);
}

bool ControlGUI::isFloat(std::string value) {
	int counter = 0, i = 0;

	if (value.empty() || value.front() == '.' || value.back() == '.')
		return false;

	if (!isdigit(value[0])) {
		if (value[0] != '-')
			return false;
		else
			i = 1;
	}

	for (; i < value.size(); i++) {
		if (value[i] == '.') counter++;
		else if (!isdigit(value[i])) return false;
	}
	// só pode ter um ponto
	return counter <= 1;
}

void ControlGUI::_send_test() {
	// verifica se os valores inseridos nos campos são válidos (são números entre -1.4 e 1.4)
	if (!isFloat(Tbox_V1.get_text()))
		Tbox_V1.set_text("0");
	if (!isFloat(Tbox_V2.get_text()))
		Tbox_V2.set_text("0");

	float v1 = std::stof(Tbox_V1.get_text());
	float v2 = std::stof(Tbox_V2.get_text());

	if (std::abs(v1) > 1.4) {
		if (v1 < 0)
			Tbox_V1.set_text("-1.4");
		else
			Tbox_V1.set_text("1.4");
	}

	if (std::abs(v2) > 1.4) {
		if (v2 < 0)
			Tbox_V2.set_text("-1.4");
		else
			Tbox_V2.set_text("1.4");
	}

	int pos = cb_test.get_active_row_number();
	std::string cmd = Tbox_V1.get_text() + ";" + Tbox_V2.get_text();

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

void ControlGUI::_update_cb_serial() {
	messenger.stop_xbee();

	cb_serial.remove_all();
	xbee_connections = 0;

	for (int i = 0; i < 256; ++i) {
		std::string port = "/dev/ttyUSB";
		port.append(std::to_string(i));

		int fd = open(port.c_str(), O_RDWR);
		if (fd != -1) {
			std::cout << port << std::endl;
			cb_serial.append(port);
			close(fd);
			xbee_connections++;
		}
	}

	// Caso tenha apenas algum dispositivo, atualizar a combo box
	if (xbee_connections > 0)
		cb_serial.set_active(0);

	bt_Serial_Start.set_state(Gtk::STATE_NORMAL);
	cb_serial.set_state(Gtk::STATE_NORMAL);
	bt_Serial_Refresh.set_state(Gtk::STATE_NORMAL);

	Tbox_V1.set_state(Gtk::STATE_INSENSITIVE);
	Tbox_V2.set_state(Gtk::STATE_INSENSITIVE);
	bt_Serial_test.set_state(Gtk::STATE_INSENSITIVE);
	cb_test.set_state(Gtk::STATE_INSENSITIVE);
	bt_Robot_Status.set_state(Gtk::STATE_INSENSITIVE);
	bt_reset_ack.set_state(Gtk::STATE_INSENSITIVE);
	test_start_bt.set_state(Gtk::STATE_INSENSITIVE);
}

void ControlGUI::_create_status_frame() {

	pack_start(status_fm, false, true, 5);
	status_fm.set_label("Robot Status");
	status_fm.add(status_grid);

	status_grid.set_border_width(10);
	status_grid.set_column_spacing(10);
	status_grid.set_halign(Gtk::ALIGN_CENTER);

	bt_Robot_Status.set_label("Update");
	bt_Robot_Status.set_state(Gtk::STATE_NORMAL);
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
}

void ControlGUI::update_ack_interface() {
	bool is_active = ack_enable_button.get_active();
	messenger.set_ack_enabled(is_active);
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
	ekf_always_send = ekf_always_send_button.get_active();
//	if(!ekf_always_send) messenger.ekf_data_file << std::flush;
}

void ControlGUI::update_dropped_frames() {
	for (int i = 0; i < TOTAL_ROBOTS; ++i) {
		char id = get_robot_id(i);
		ack_count ack = messenger.get_ack_count(id);
		if (ack.total == -1) return;

		std::stringstream stream;
		stream << "Lost ACKs: ";
		stream << std::fixed << std::setprecision(2) << ack.lost_rate << "%";
		stream << ", Total: " << ack.total;
		dropped_frames[i].set_text(stream.str());
	}
}

void ControlGUI::reset_lost_acks() {
	messenger.reset_lost_acks();
	update_dropped_frames();
}

void ControlGUI::set_frameskipper() {
	int frames;
	try {
		frames = std::stoi(entry_set_frameskip.get_text());
	} catch (...) {
		return;
	}
	messenger.set_frameskipper(frames);
	std::string time_str = std::to_string(33 * (frames + 1));
	time_msgs.set_label("Time between CMDs: " + time_str + " ms");
}

void ControlGUI::update_msg_time() {
	acc_time += messenger.get_time();
	time_count++;
	if (acc_time > 500) {
		std::ostringstream ss;
		ss << round(acc_time / time_count * 100) / 100;
		time_msgs.set_label("Time between CMDs: " + ss.str() + " ms");
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

	pack_start(test_frame, false, true, 5);
	test_frame.set_label("Test On Click");
	test_frame.set_border_width(5);
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

	test_start_bt.set_state(Gtk::STATE_INSENSITIVE);
	test_command_cb.set_state(Gtk::STATE_INSENSITIVE);
	test_angle_scale.set_state(Gtk::STATE_INSENSITIVE);
	test_set_bt.set_state(Gtk::STATE_INSENSITIVE);

	test_start_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_test_start_bt_event));
	test_command_cb.signal_changed().connect(sigc::mem_fun(*this, &ControlGUI::_test_command_changed_event));
	test_set_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_test_send_bt_event));
}

void ControlGUI::_test_start_bt_event() {
	bool is_active = test_start_bt.get_active();

	test_controller.set_active(is_active);

	if (is_active) {
		Robot2::Command command = test_controller.get_command();

		if (command == Robot2::Command::Vector || command == Robot2::Command::Orientation
			|| command == Robot2::Command::UVF) {
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
			test_controller.set_command(Robot2::Command::None);
			test_angle_scale.set_state(Gtk::STATE_INSENSITIVE);
			test_set_bt.set_state(Gtk::STATE_INSENSITIVE);
			break;
		case 1:
			test_controller.set_command(Robot2::Command::Vector);
			test_angle_scale.set_state(Gtk::STATE_NORMAL);
			test_set_bt.set_state(Gtk::STATE_NORMAL);
			break;
		case 2:
			test_controller.set_command(Robot2::Command::Orientation);
			test_angle_scale.set_state(Gtk::STATE_NORMAL);
			test_set_bt.set_state(Gtk::STATE_NORMAL);
			break;
		case 3:
			test_controller.set_command(Robot2::Command::UVF);
			test_angle_scale.set_state(Gtk::STATE_NORMAL);
			test_set_bt.set_state(Gtk::STATE_NORMAL);
			break;
		default:
			test_controller.set_command(Robot2::Command::Position);
			test_angle_scale.set_state(Gtk::STATE_INSENSITIVE);
			test_set_bt.set_state(Gtk::STATE_INSENSITIVE);

	}
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
