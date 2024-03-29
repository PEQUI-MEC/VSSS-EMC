/*
* v4lgui.hpp
*
*  Created on: Feb 1, 2014
*      Author: gustavo
*/

#ifndef V4LINTERFACE_HPP_
#define V4LINTERFACE_HPP_

#include <Goalkeeper.hpp>
#include <Defender.hpp>
#include <Attacker.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <gtkmm.h>
#include <gtkmm/messagedialog.h>
#include <linux/videodev2.h>
#include "v4lcap.hpp"
#include "ImageView.hpp"
#include "Messenger.h"
#include "RobotGUI.hpp"
#include "visionGUI.hpp"
#include "controlGUI.hpp"
#include <ctime>
#include <chrono>
#include <ImageWarp.hpp>
#include <GameGUI.hpp>

#define PI 3.14159265453

namespace capture {
	class V4LInterface;
	struct __ctrl_holder;
}

typedef struct capture::__ctrl_holder {
	struct v4l2_queryctrl qctrl;
	Gtk::Widget *widget;
	sigc::connection con;
} ControlHolder;

class capture::V4LInterface : public Gtk::VBox {

	public:
		bool isLowRes = false;

		Game& game;
		RobotGUI &robotGUI;

		VisionGUI visionGUI;
		ControlGUI controlGUI;

		GameGUI gameGUI;

		bool init_frame = true;

		ImageView imageView;

		std::list<ControlHolder> ctrl_list_default;

		std::string camera_card;

		Gtk::Image red_button_released;
		Gtk::Image red_button_pressed;

		Gtk::Label team_color;
		std::vector<Gtk::Label> robot_pos_lb_list;
		Gtk::Label ball_pos_lb;

		Gtk::HBox info_hbox;
		Gtk::VBox robots_pos_vbox;
		std::vector<Gtk::HBox> robots_pos_hbox;
		Gtk::HBox start_game_hbox;
		Gtk::VBox buttons_vbox;
		Gtk::ToggleButton start_game_bt;

		Gtk::Frame robots_pos_fm;
		Gtk::Frame robots_buttons_fm;
		Gtk::VBox robots_pos_buttons_vbox;
		Gtk::HBox fps_hbox;
		Gtk::Label *fps_label;

		Gtk::CheckButton record_video_checkbox;

		capture::v4lcap vcap;

		explicit V4LInterface(Game &game, RobotGUI &robot_gui, bool isLow);
		void initInterface();

		Gtk::Scale HScale_offsetL;
		Gtk::Scale HScale_offsetR;

		Gtk::Grid warp_grid;
		Gtk::Label warp_label[6];
		Gtk::ToggleButton bt_warp_start;
		Gtk::Button bt_warp_apply;
		Gtk::ToggleButton bt_adjust_start;
		Gtk::Button bt_adjust_apply;
		Gtk::CheckButton bt_invert_field;
		Gtk::Button bt_warp_undo;
		Gtk::Button bt_adjust_undo;

		// Signals
		sigc::connection cb_input_signal;
		sigc::connection cb_standard_signal;
		sigc::connection cb_format_desc_signal;
		sigc::connection cb_frame_size_signal;
		sigc::connection cb_frame_interval_signal;

		// Camera calibration frames
		Gtk::Frame fr_camCalib_offline;
		Gtk::Frame fr_camCalib_online;
		Gtk::Button btn_camCalib_collect, btn_camCalib_reset, btn_camCalib_start, btn_camCalib_pop, btn_camCalib_offline_start;
		Gtk::ToggleButton btn_camCalib;

		bool CamCalib_flag_event = false;

		void __event_bt_quick_save_clicked();
		void __event_bt_quick_load_clicked();
		void __event_bt_save_clicked();
		void __event_bt_load_clicked();
		void __event_bt_start_clicked();
		void __event_bt_refresh_clicked();
		void __event_bt_warp_start_clicked();
		void __event_bt_adjust_start_clicked();
		void __event_bt_warp_apply_clicked();
		void __event_bt_adjust_apply_clicked();
		void __event_bt_invert_field_clicked();
		void __event_bt_warp_undo_clicked();
		void __event_bt_adjust_undo_clicked();
		void __event_bt_reset_warp_clicked();
		void HScale_offsetR_value_changed();
		void HScale_offsetL_value_changed();
		void __event_bt_invert_image_signal_clicked();
		void __event_cb_device_changed();
		void __event_cb_input_changed();
		void __event_cb_standard_changed();
		void __event_cb_format_desc_changed();
		void __event_cb_frame_size_changed();
		void __event_camCalib_mode_clicked();
		void __event_camCalib_online_collect_clicked();
		void __event_camCalib_online_pop_clicked();
		void __event_camCalib_online_reset_clicked();
		void __event_camCalib_online_start_clicked();
		void __event_camCalib_offline_start_clicked();
		void __event_camCalib_pressed();
		void __event_cb_frame_interval_changed();
		void createPositionsAndButtonsFrame();
		void event_start_game_bt_signal_clicked();
		void updateRobotLabels();
		void update_ball_position(Geometry::Point ball);
		void updateFPS(int fps);
		void update_interface_camera();

		/* Signals */
		typedef sigc::signal<bool, bool> SignalStart;

		SignalStart signal_start() {
			return m_signal_start;
		}

	Gtk::Button bt_start;

	protected:
		SignalStart m_signal_start;

		bool on_button_press_event(GdkEventButton *event) override;

	private:

		void __init_combo_boxes();
		void __create_frm_device_info();
		void __create_frm_device_properties();
		void __create_frm_cam_calib();
		void createQuickActionsFrame();
		void __create_frm_warp();

		// Combo properties updates
		int __update_cb_device();
		void __update_cb_input();
		void __update_cb_standard();
		void __update_cb_format_desc();
		void __update_cb_frame_size();
		void __update_cb_frame_interval();
		void __update_all();

		Gtk::Frame frm_device_info;
		Gtk::ComboBoxText cb_device;
		Gtk::Button bt_refresh;
		Gtk::Button bt_reset_warp;

		Gtk::Frame frm_quick_actions;
		Gtk::Button bt_quick_save;
		Gtk::Button bt_quick_load;
		Gtk::Button bt_save;
		Gtk::Button bt_load;

		Gtk::Label lb_device_name;
		Gtk::Label lb_device_card;
		Gtk::Label lb_device_driver;
		Gtk::Label lb_device_bus;

		Gtk::Frame frm_device_prop;
		Gtk::Frame frm_warp;
		Gtk::SpinButton sp_width;
		Gtk::SpinButton sp_height;
		Gtk::Frame frm_cam_calib;

		Gtk::ComboBox cb_input;
		Gtk::ComboBox cb_standard;
		Gtk::ComboBox cb_format_desc;
		Gtk::ComboBox cb_frame_size;
		Gtk::ComboBox cb_frame_interval;

		Glib::RefPtr<Gtk::ListStore> ls_input;
		Glib::RefPtr<Gtk::ListStore> ls_standard;
		Glib::RefPtr<Gtk::ListStore> ls_format_desc;
		Glib::RefPtr<Gtk::ListStore> ls_frame_size;
		Glib::RefPtr<Gtk::ListStore> ls_frame_interval;

		// Camera Calibration Mode
		Gtk::RadioButton calib_offline, calib_online;

		template<class T>
		class ModelColumn : public Gtk::TreeModel::ColumnRecord {
			public:

				ModelColumn() {
					add(m_col_name);
					add(m_col_data);
				}

				Gtk::TreeModelColumn<Glib::ustring> m_col_name;
				Gtk::TreeModelColumn<T> m_col_data;
		};

		typedef ModelColumn<struct v4l2_input> ModelInput;
		typedef ModelColumn<struct v4l2_standard> ModelStandard;
		typedef ModelColumn<struct v4l2_fmtdesc> ModelFormatDesc;
		typedef ModelColumn<struct v4l2_frmsizeenum> ModelFrameSize;
		typedef ModelColumn<struct v4l2_querymenu> ModelControlMenu;
		typedef ModelColumn<struct v4l2_frmivalenum> ModelFrameInterval;

		ModelInput model_input;
		ModelStandard model_standard;
		ModelFormatDesc model_format_desc;
		ModelFrameSize model_frame_size;
		ModelFrameInterval model_frame_interval;

		ModelControlMenu model_control_menu;

		Gtk::Notebook notebook;

		Gtk::VBox capture_vbox;

		void __make_control_list_default();
		void __make_control_table(std::list<ControlHolder> &list, const char *title);
		void __update_control_widgets(std::list<ControlHolder> &list);
		void __block_control_signals(std::list<ControlHolder> &list, bool block);
		void __set_control_hscale(std::list<ControlHolder> *list, Gtk::Widget *wctrl);
		void __set_control(std::list<ControlHolder> *list, Gtk::Widget *wctrl);
};

#endif /* V4LINTERFACE_HPP_ */
