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

class ControlGUI : public Gtk::VBox {
	private:
		int xbee_connections = 0;

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
		Gtk::Label radio_xbee_lbl;
		Gtk::ComboBoxText radio_xbee_cb;
		Gtk::Button radio_refresh_bt;
		Gtk::Button radio_connect_bt;
		Gtk::ComboBoxText radio_robots_cb;
		Gtk::Button radio_send_speed_bt;
		Gtk::Button radio_send_cmd_bt;
		Gtk::Entry radio_cmd_entry;
		Gtk::Label radio_rawcmd_lbl;
		Gtk::Grid radio_options_grid;
		Gtk::Label radio_skip_lbl;
		Gtk::Label radio_time_lbl;
		Gtk::SpinButton radio_skip_sbt;
		Gtk::CheckButton radio_ekf_chbt;
		Gtk::CheckButton radio_acks_chbt;

		// Commands Frame
		Gtk::Frame commands_fm;
		Gtk::Grid commands_grid;
		Gtk::Label commands_cmd_lbl;
		Gtk::Label commands_speed_lb;
		Gtk::Label commands_L_lb;
		Gtk::Label commands_R_lb;
		Gtk::Label commands_to_lb;
		Gtk::HScale commands_L_hsc;
		Gtk::HScale commands_R_hsc;


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

		explicit ControlGUI(const std::array<Robot2 *, 3> &robots, const Geometry::Point &ball);

		void stop_test_on_click();

		void _send_command();

		void _test_command_changed_event();

		void _test_send_bt_event();

		// Gets battery % and robot id to update a single robot's battery status
		void updateInterfaceStatus(double battery, int id);

		// update the battery status of all robots
		void _robot_status();

		void _start_serial();

		void _send_test();

		void _update_cb_serial();

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

		// essa função não deve ser chamada imediatamente após desconectar o xbee
		// o xbee demora alguns segundos para realmente desconectar
		// conecta o xbee automaticamente caso tenha apenas um conectado
		void auto_start_serial();
};

#endif /* CONTROLGUI_HPP_ */
