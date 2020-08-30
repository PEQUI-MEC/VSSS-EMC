#ifndef CONTROLGUI_HPP_
#define CONTROLGUI_HPP_

#include <gtkmm.h>
#include <string>
#include "Messenger.h"
#include "TestOnClick.hpp"
#include <unistd.h>
#include <fcntl.h>
// system_clock::now
#include <iostream>
#include <ctime>
#include <chrono>
#include <Strategy3/Game.hpp>
#include <GUI/XbeeSelectGUI.hpp>

class ControlGUI : public Gtk::VBox {
	public:
		Messenger messenger;
		onClick::TestOnClick test_controller;

		const static int TOTAL_ROBOTS = 6;

		Gtk::Label *label;

		double acc_time;
		int time_count;

		// Botões e combo box Rádio

		Gtk::Button bt_Robot_Status;

		bool ekf_always_send = false;

		// Radio Frame
		Gtk::Frame radio_fm;
		Gtk::Label radio_rawcmd_lbl;
		Gtk::Grid radio_options_grid;
		Gtk::Label radio_skip_lbl;
		Gtk::Label radio_time_lbl;
		Gtk::SpinButton radio_skip_sbt;
		Gtk::CheckButton radio_ekf_chbt;
		Gtk::CheckButton radio_acks_chbt;


//		Select multiple xbees
		std::list<XbeeSelectGUI> xbee_select_list;
		Gtk::VBox xbee_select_vbox;
		Gtk::Button add_select_xbee_bt;
		Gtk::Button remove_select_xbee_bt;

		// Commands Frame
		Gtk::Frame commands_fm;
		Gtk::Grid commands_grid;
		Gtk::Label commands_speed_lb;
		Gtk::Label commands_L_lb;
		Gtk::Label commands_R_lb;
		Gtk::Label commands_to_lb;
		Gtk::HScale commands_L_hsc;
		Gtk::HScale commands_R_hsc;
		Gtk::Button commands_send_speed_bt;
		Gtk::Button commands_send_cmd_bt;
		Gtk::Entry commands_cmd_entry;
		Gtk::ComboBoxText commands_robots_cb;


		Gtk::Grid status_grid;
		Gtk::Frame status_fm;
		Gtk::Image status_img[TOTAL_ROBOTS];
		Gtk::Label robots_lb[TOTAL_ROBOTS];
		Gtk::Label status_lb[TOTAL_ROBOTS];
		Gtk::Label lastUpdate_lb;
		Gtk::ProgressBar battery_bar[TOTAL_ROBOTS];
		Gtk::Label dropped_frames[TOTAL_ROBOTS];
		Gtk::Button bt_reset_ack;

		Gtk::Frame test_frame;
		Gtk::VBox test_vbox;
		Gtk::Grid test_grid;
		Gtk::Scale test_angle_scale;
		Gtk::ComboBoxText test_command_cb;
		Gtk::ToggleButton test_start_bt;
		Gtk::Button test_set_bt;
		Gtk::Label test_default_lb[2], test_tip_lb;
		const std::string test_start_txt = "Start";
		const std::string test_stop_txt = "Stop";

		void _test_start_bt_event();

		explicit ControlGUI(Game &game);
		ControlGUI(const ControlGUI &) = delete;
		ControlGUI& operator=(const ControlGUI&) = delete;

		void stop_test_on_click();

		void _send_command();

		void _test_command_changed_event();

		void _test_send_bt_event();

		// Gets battery % and robot id to update a single robot's battery status
		void updateInterfaceStatus(double battery, double voltage, int id);

		// update the battery status of all robots
		void _robot_status();

		void _send_test();

		void _create_radio_frame();

		void _create_commands_frame();

		void _create_status_frame();

		void _create_test_on_click_frame();

		char get_robot_id(int pos);
		void update_dropped_frames();
		void reset_lost_acks();
		void update_ack_interface();
		void set_frameskipper();
		void update_msg_time();
		void ekf_always_send_enable();

		void adjust_widgets_state();

		void add_xbee_select_gui();
		void remove_xbee_select_gui();

};

#endif /* CONTROLGUI_HPP_ */
