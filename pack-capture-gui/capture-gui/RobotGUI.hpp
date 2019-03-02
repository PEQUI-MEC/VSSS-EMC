#ifndef VSSS_ROBOTINFOGUI_HPP
#define VSSS_ROBOTINFOGUI_HPP

#include <gtkmm.h>
#include "Robot2.h"

class RobotGUI : public Gtk::Box {
	private:
		const std::array<Robot2*, 3>& robots;
		const bool isLowRes;

		bool robots_speed_edit_flag = false;
		int robots_id_tmp[3];
		int robots_id_change_tmp[3];
		bool robots_id_edit_flag = false;
		int robots_function_tmp[3];
		int robots_function_change_tmp[3];
		bool robots_function_edit_flag = false;
		double robots_speed_tmp[3];

		Gtk::HBox id_hbox, speed_hbox, role_hbox;
		Gtk::VBox id_vbox, speed_vbox, role_vbox, speed_bars_box[3];
		Gtk::Grid id_grid, speed_grid, role_grid;
		Gtk::Frame id_frame, speed_frame, role_frame;

		Gtk::ComboBoxText robots_id_box[3];
		Gtk::Button robots_id_edit_bt;
		Gtk::Button robots_auto_bt;
		Gtk::Button robots_id_done_bt;

		Gtk::Button robots_speed_edit_bt;
		Gtk::Button robots_speed_done_bt;
		Gtk::HScale robots_speed_hscale[3];
		Gtk::ProgressBar robots_speed_progressBar[3];

		Gtk::Button robots_role_edit_bt;
		Gtk::Button robots_role_done_bt;
		Gtk::ComboBoxText cb_robot_role[3];

		void initRobotGUI();
		void createIDsFrame();
		void createSpeedsFrame();
		void createFunctionsFrame();
		void setup_buttons();
		void setup_combo_boxes();
		void auto_change_id_combo_boxes();
		void auto_change_role_combo_boxes();
		void setup_grids();
		void setup_bars();
		//void discover_robot_ids();
		void event_robots_role_done_bt_signal_clicked();
		void event_robots_role_edit_bt_signal_clicked();
		void event_robots_speed_done_bt_signal_clicked();
		void event_robots_speed_edit_bt_signal_pressed();
		void event_robots_id_done_bt_signal_clicked();
		void event_robots_id_edit_bt_signal_pressed();
		void event_robots_auto_bt_signal_pressed();

	public:
		RobotGUI(std::array<Robot2 *, 3> &robots, bool isLowRes);

		void update_speed_progressBars();
		void update_robot_functions();
		void update_robots();
		void enable_main_buttons(bool enable = true);

};

#endif //VSSS_ROBOTINFOGUI_HPP
