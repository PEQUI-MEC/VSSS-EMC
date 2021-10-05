#include "RobotGUI.hpp"

using namespace Gtk;
using std::to_string;

void RobotGUI::createIDsFrame() {
	pack_start(id_frame, false, false, 5);
	id_frame.add(id_vbox);
	id_frame.set_label("IDs");
	id_vbox.pack_start(id_hbox, false, true, 5);
	id_vbox.pack_start(id_grid, false, true, 5);

	id_hbox.pack_start(robots_id_edit_bt, false, true, 5);
	id_hbox.pack_start(robots_auto_bt, false, true, 5);
	id_hbox.pack_end(robots_id_done_bt, false, true, 5);

	int grid_index = 0;
	for (auto& robot : game.team->robots) {
		Label * label = new Label(robot.tag_name()+":");
		id_grid.attach(*label, 0, grid_index, 2, 1);
		id_grid.attach(robots_id_box[grid_index], 2, grid_index, 1, 1);
		grid_index++;
	}
}

void RobotGUI::createFunctionsFrame() {
	pack_start(role_frame, false, false, 5);
	role_frame.add(role_vbox);
	role_frame.set_label("Role");
	role_vbox.pack_start(role_hbox, false, true, 5);
	role_vbox.pack_start(role_grid, false, true, 5);

	role_hbox.pack_start(robots_role_edit_bt, false, true, 5);
	role_hbox.pack_end(robots_role_done_bt, false, true, 5);

	int grid_index = 0;
	for(auto& robot : game.team->robots) {
		Label * label = new Label(robot.tag_name()+":");
		role_grid.attach(*label, 0, grid_index, 2, 1);
		role_grid.attach(cb_robot_role[grid_index], 2, grid_index, 1, 1);
		grid_index++;
	}
}

void RobotGUI::createSpeedsFrame() {
	pack_start(speed_frame, false, false, 5);
	speed_frame.add(speed_vbox);
	speed_frame.set_label("Speed");
	speed_vbox.pack_start(speed_hbox, false, true, 5);
	speed_vbox.pack_start(speed_grid, false, true, 5);

	speed_hbox.pack_start(robots_speed_edit_bt, false, true, 5);
	speed_hbox.pack_end(robots_speed_done_bt, false, true, 5);

	int grid_index = 0;
	for (auto& robot : game.team->robots) {
		Label * label = new Label(robot.tag_name()+":");
		speed_bars_box[grid_index].pack_start(robots_speed_hscale[grid_index], false, true, 0);
		speed_bars_box[grid_index].pack_start(robots_speed_progressBar[grid_index], false, true, 0);
		speed_grid.attach(*label, 0, grid_index, 2, 1);
		speed_grid.attach(speed_bars_box[grid_index], 2, grid_index, 2, 1);
		grid_index++;
	}
}

void RobotGUI::update_robot_ids() {
	if (robots_id_edit_flag) return;
	for (uint i = 0; i < game.robot_count; i++) {
		robots_id_box[i].set_active(game.team->robots[i].ID - 'A');
	}
}

void RobotGUI::update_robot_functions() {
	if (robots_function_edit_flag) return;
	for (unsigned i = 0; i < game.robot_count; i++) {
		Role role = game.team->robots[i].role;
		cb_robot_role[i].set_active((int) role);
	}
}

void RobotGUI::update_speed_progressBars() {
	if (robots_speed_edit_flag) return;
	for(unsigned i = 0; i < game.robot_count; i++) {
		auto& robot = game.team->robots[i];
		robots_speed_hscale[i].set_value(robot.default_target_velocity);
		robots_speed_progressBar[i].set_fraction(robot.target.pose.velocity.linear / 1.5);
		const std::string velocity = std::to_string(robot.target.pose.velocity.linear);
		const std::string vel_text = velocity.substr(0, velocity.find('.') + 2);
		robots_speed_progressBar[i].set_text(vel_text);
	}
}

void RobotGUI::update_robots() {
	update_robot_ids();
	update_robot_functions();
	update_speed_progressBars();
}

void RobotGUI::initRobotGUI() {
	if (isLowRes)
		set_orientation(ORIENTATION_VERTICAL);
	else
		set_orientation(ORIENTATION_HORIZONTAL);

	createIDsFrame();
	createFunctionsFrame();
	createSpeedsFrame();

	setup_grids();
	setup_combo_boxes();
	setup_buttons();
	setup_bars();
}

RobotGUI::RobotGUI(Game& game, bool isLowRes) :
		game(game),
		isLowRes(isLowRes),
		robots_id_tmp(game.robot_count),
		robots_id_change_tmp(game.robot_count),
		robots_function_tmp(game.robot_count),
		robots_function_change_tmp(game.robot_count),
		robots_speed_tmp(game.robot_count),
		speed_bars_box(game.robot_count),
		robots_id_box(game.robot_count),
		robots_speed_hscale(game.robot_count),
		robots_speed_progressBar(game.robot_count),
		cb_robot_role(game.robot_count){
			initRobotGUI();
}

void RobotGUI::event_robots_role_done_bt_signal_clicked() {
	for (unsigned i = 0; i < game.robot_count; ++i) {
		int role = cb_robot_role[i].get_active_row_number();
		game.team->robots[i].role = (Role) role;
	}

	robots_function_edit_flag = false;
	robots_role_edit_bt.set_label("Edit");
	for (unsigned i = 0; i < game.robot_count; ++i) {
		cb_robot_role[i].set_state(Gtk::STATE_INSENSITIVE);
	}
	robots_role_done_bt.set_state(Gtk::STATE_INSENSITIVE);
}

void RobotGUI::event_robots_role_edit_bt_signal_clicked() {
	if (!robots_function_edit_flag) {
		robots_function_edit_flag = true;
		robots_role_edit_bt.set_label("Cancel");
		robots_role_edit_bt.set_state(Gtk::STATE_NORMAL);
		robots_role_done_bt.set_state(Gtk::STATE_NORMAL);
		for (unsigned i = 0; i < game.robot_count; ++i) {
			cb_robot_role[i].set_state(Gtk::STATE_NORMAL);
			robots_function_change_tmp[i] = robots_function_tmp[i] = cb_robot_role[i].get_active_row_number();
		}
	} else {
		robots_function_edit_flag = false;
		robots_role_edit_bt.set_label("Edit");
		robots_role_done_bt.set_state(Gtk::STATE_INSENSITIVE);
		for (unsigned i = 0; i < game.robot_count; ++i) {
			cb_robot_role[i].set_state(Gtk::STATE_INSENSITIVE);
			cb_robot_role[i].set_active(robots_function_tmp[i]);
		}
	}
}

void RobotGUI::event_robots_speed_done_bt_signal_clicked() {
	for (unsigned i = 0; i < game.robot_count; i++) {
		game.team->robots[i].default_target_velocity = (float) robots_speed_hscale[i].get_value();
		robots_speed_hscale[i].set_state(Gtk::STATE_INSENSITIVE);
	}
	robots_speed_edit_flag = false;
	robots_speed_edit_bt.set_label("Edit");
	robots_speed_done_bt.set_state(Gtk::STATE_INSENSITIVE);
}

void RobotGUI::event_robots_speed_edit_bt_signal_pressed() {
	if (!robots_speed_edit_flag) {
		robots_speed_edit_flag = true;
		robots_speed_edit_bt.set_label("Cancel");
		robots_speed_done_bt.set_state(Gtk::STATE_NORMAL);
		for (unsigned i = 0; i < game.robot_count; i++) {
			robots_speed_hscale[i].set_state(Gtk::STATE_NORMAL);
			robots_speed_tmp[i] = robots_speed_hscale[i].get_value();
		}
	} else {
		robots_speed_edit_flag = false;
		robots_speed_edit_bt.set_label("Edit");
		robots_speed_done_bt.set_state(Gtk::STATE_INSENSITIVE);
		for (unsigned i = 0; i < game.robot_count; i++) {
			robots_speed_hscale[i].set_state(Gtk::STATE_INSENSITIVE);
			robots_speed_hscale[i].set_value(robots_speed_tmp[i]);
		}
	}
}

void RobotGUI::event_robots_id_done_bt_signal_clicked() {
	for (uint i = 0; i < game.robot_count; i++) {
		const char * id = robots_id_box[i].get_active_text().c_str();
		game.team->robots[i].ID = *id;
	}

	robots_id_edit_flag = false;
	robots_id_edit_bt.set_label("Edit");
	for (uint i = 0; i < game.robot_count; i++) {
		robots_id_box[i].set_state(Gtk::STATE_INSENSITIVE);
	}
	robots_id_done_bt.set_state(Gtk::STATE_INSENSITIVE);
	robots_auto_bt.set_state(Gtk::STATE_NORMAL);
}

void RobotGUI::event_robots_id_edit_bt_signal_pressed() {
	if (!robots_id_edit_flag) {
		robots_id_edit_flag = true;
		robots_id_edit_bt.set_label("Cancel");
		for (uint i = 0; i < game.robot_count; i++) {
			robots_id_box[i].set_state(Gtk::STATE_NORMAL);
			robots_id_change_tmp[i] = robots_id_tmp[i] = robots_id_box[i].get_active_row_number();
		}
		robots_id_done_bt.set_state(Gtk::STATE_NORMAL);
		robots_auto_bt.set_state(Gtk::STATE_INSENSITIVE);
	} else {
		robots_id_edit_flag = false;
		robots_id_edit_bt.set_label("Edit");
		for (uint i = 0; i < game.robot_count; i++) {
			robots_id_box[i].set_state(Gtk::STATE_INSENSITIVE);
			robots_id_box[i].set_active(robots_id_tmp[i]);
		}
		robots_id_done_bt.set_state(Gtk::STATE_INSENSITIVE);
		robots_auto_bt.set_state(Gtk::STATE_NORMAL);
	}
}

void RobotGUI::event_robots_auto_bt_signal_pressed() {
	// FIXME: Implementar discoberta automatica de IDs para novo Robot (1/3)
	//robots_auto_bt.set_state(Gtk::STATE_INSENSITIVE);
	//robots_id_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
	//std::thread discover_robots_thread(&RobotGUI::discover_robot_ids, this);
	//discover_robots_thread.detach();
}

void RobotGUI::enable_main_buttons(bool enable) {
	if (enable) {
		// FIXME: Implementar discoberta automatica de IDs para novo Robot (2/3)
		//robots_auto_bt.set_state(STATE_NORMAL);
		robots_id_edit_bt.set_state(STATE_NORMAL);
		robots_speed_edit_bt.set_state(STATE_NORMAL);
		robots_role_edit_bt.set_state(STATE_NORMAL);
	} else {
		//robots_auto_bt.set_state(STATE_INSENSITIVE);
		robots_id_edit_bt.set_state(STATE_INSENSITIVE);
		robots_speed_edit_bt.set_state(STATE_INSENSITIVE);
		robots_role_edit_bt.set_state(STATE_INSENSITIVE);
	}
}

void RobotGUI::setup_grids() {
	id_grid.set_border_width(5);
	id_grid.set_column_spacing(5);
	id_grid.set_row_spacing(5);
	id_grid.set_halign(ALIGN_CENTER);

	role_grid.set_border_width(5);
	role_grid.set_column_spacing(5);
	role_grid.set_row_spacing(5);
	role_grid.set_halign(ALIGN_CENTER);

	speed_grid.set_border_width(5);
	speed_grid.set_column_spacing(5);
	speed_grid.set_row_spacing(5);
	speed_grid.set_halign(ALIGN_CENTER);
}

void RobotGUI::setup_combo_boxes() {

	for (unsigned index = 0; index < game.robot_count; index++) {
		robots_id_box[index].signal_changed()
				.connect(sigc::mem_fun(*this, &RobotGUI::auto_change_id_combo_boxes));
		cb_robot_role[index].signal_changed()
				.connect(sigc::mem_fun(*this, &RobotGUI::auto_change_role_combo_boxes));
	}

	// Roles/Functions
	for (auto &cb : cb_robot_role) {
		for (unsigned i = 0; i < ROLE_COUNT; i++) {
			Role role = (Role) i;
			cb.append(role_to_str(role));
		}
		cb.set_state(Gtk::STATE_INSENSITIVE);
	}

	for(unsigned i = 0; i < game.robot_count; i++) {
		cb_robot_role[i].set_active((int) Role::None);
	}

	// IDs
	char robotsIDs[6] = {'A', 'B', 'C', 'D', 'E', 'F'};
	for (auto &cb : robots_id_box) {
		for (char robotsID : robotsIDs) {
			cb.append(std::string(1, robotsID));
		}
		cb.set_state(STATE_INSENSITIVE);
	}
	for (unsigned i = 0; i < game.robot_count; i++) {
		robots_id_box[i].set_active(i);
	}
}

void RobotGUI::auto_change_id_combo_boxes() {
	int robots_id_tmp_active[game.robot_count];

	for (unsigned i = 0; i < game.robot_count; i++) {
		robots_id_tmp_active[i] = robots_id_box[i].get_active_row_number();
	}

	for (int i = 0; i < 3; ++i) {
		if (robots_id_tmp_active[i] != robots_id_change_tmp[i]) {
			for (int j = 0; j < 3; ++j) {
				if (j != i) {
					if (robots_id_tmp_active[j] == robots_id_tmp_active[i]) {
						robots_id_tmp_active[j] = robots_id_change_tmp[i];
					}
				}
			}
		}
	}


	for (unsigned i = 0; i < game.robot_count; i++) {
		robots_id_change_tmp[i] = robots_id_tmp_active[i];
		robots_id_box[i].set_active(robots_id_change_tmp[i]);
	}
}

void RobotGUI::auto_change_role_combo_boxes() {
	//Para ROLE
	int robots_function_tmp_active[game.robot_count];

	for (unsigned i = 0; i < game.robot_count; i++) {
		robots_function_tmp_active[i] = cb_robot_role[i].get_active_row_number();
	}

	for (unsigned i = 0; i < game.robot_count; ++i) {
		if(robots_function_tmp_active[i] != robots_function_change_tmp[i]){
			for (unsigned j = 0; j < game.robot_count; ++j) {
				if(j != i) {
					if(robots_function_tmp_active[j] == robots_function_tmp_active[i]
					   && robots_function_tmp_active[j] != (int) Role::None) {
						robots_function_tmp_active[j] = robots_function_change_tmp[i];
					}
				}
			}
		}
	}

	for (unsigned i = 0; i < game.robot_count; i++) {
		robots_function_change_tmp[i] = robots_function_tmp_active[i];
		cb_robot_role[i].set_active(robots_function_change_tmp[i]);
	}
}

void RobotGUI::setup_buttons() {
	// ID buttons
	robots_auto_bt.set_label("Auto");
	robots_id_edit_bt.set_label("Edit");
	robots_id_done_bt.set_label("Done");

	robots_id_done_bt.set_state(Gtk::STATE_INSENSITIVE);
	robots_id_edit_bt.set_state(Gtk::STATE_NORMAL);
	robots_auto_bt.set_state(Gtk::STATE_INSENSITIVE);

	robots_id_edit_bt.signal_pressed().connect(
			sigc::mem_fun(*this, &RobotGUI::event_robots_id_edit_bt_signal_pressed));
	robots_auto_bt.signal_pressed().connect(
			sigc::mem_fun(*this, &RobotGUI::event_robots_auto_bt_signal_pressed));
	robots_id_done_bt.signal_clicked().connect(
			sigc::mem_fun(*this, &RobotGUI::event_robots_id_done_bt_signal_clicked));

	// Speed buttons
	robots_speed_edit_bt.set_label("Edit");
	robots_speed_done_bt.set_label("Done");

	robots_speed_edit_bt.set_state(Gtk::STATE_NORMAL);
	robots_speed_done_bt.set_state(Gtk::STATE_INSENSITIVE);

	robots_speed_edit_bt.signal_pressed().connect(
			sigc::mem_fun(*this, &RobotGUI::event_robots_speed_edit_bt_signal_pressed));
	robots_speed_done_bt.signal_clicked().connect(
			sigc::mem_fun(*this, &RobotGUI::event_robots_speed_done_bt_signal_clicked));

	// Role buttons
	robots_role_edit_bt.set_label("Edit");
	robots_role_done_bt.set_label("Done");

	robots_role_done_bt.set_state(Gtk::STATE_INSENSITIVE);
	robots_role_edit_bt.set_state(Gtk::STATE_NORMAL);

	robots_role_edit_bt.signal_clicked().connect(
			sigc::mem_fun(*this, &RobotGUI::event_robots_role_edit_bt_signal_clicked));
	robots_role_done_bt.signal_clicked().connect(
			sigc::mem_fun(*this, &RobotGUI::event_robots_role_done_bt_signal_clicked));
}

void RobotGUI::setup_bars() {
	for (unsigned i = 0; i < game.robot_count; i++) {
		robots_speed_hscale[i].set_digits(1);
		robots_speed_hscale[i].set_increments(0.1, 1);
		robots_speed_hscale[i].set_range(0, 1.5);
		robots_speed_hscale[i].set_size_request(100, -1);
		robots_speed_hscale[i].set_value(0.8);
		robots_speed_hscale[i].set_state(Gtk::STATE_INSENSITIVE);

		auto& robot = game.team->robots[i];
		robots_speed_progressBar[i].set_halign(Gtk::ALIGN_CENTER);
		robots_speed_progressBar[i].set_valign(Gtk::ALIGN_CENTER);
		robots_speed_progressBar[i].set_text(to_string(robot.default_target_velocity));
		robots_speed_progressBar[i].set_show_text(true);
		robots_speed_progressBar[i].set_size_request(100, -1);
		robots_speed_progressBar[i].set_fraction(robot.target.pose.velocity.linear / 1.5);
	}
}

//void RobotGUI::discover_robot_ids() {
	// FIXME: Implementar discoberta automatica de IDs para novo Robot (3/3)
//	std::array<char, 3> robot_ids{'A', 'B', 'E'};
//	for (char id : robot_ids) {
//		auto initial_robots = robot_list;
//		messenger->send_msg(id, "O90;0.8");
//		std::this_thread::sleep_for(std::chrono::seconds(1));
//		int robot_index = 0;
//		bool robot_found = false;
//		for (int i = 0; i < 3; i++) {
//			double theta = robot_list[i].orientation;
//			double prev_theta = initial_robots[i].orientation;
//			if (std::abs(theta - prev_theta) * (180/PI) > 60) {
//				robot_index = i;
//				robot_found = true;
//			}
//		}
//		if(robot_found) {
//			std::cout << "Robot " << robot_index + 1 << ": " << id << std::endl;
//			robot_list[robot_index].ID = id;
//		} else std::cout << "Robot " << id << " not found" << std::endl;
//	}
//	messenger->send_ekf_data(robot_list);
//	for(auto& robot : robot_list)
//		messenger->send_msg(robot.ID, "O0;0.8");
//	update_robots_id_box();
//	robots_auto_bt.set_state(Gtk::STATE_NORMAL);
//	robots_id_edit_bt.set_state(Gtk::STATE_NORMAL);
//}
