/*
 * v4lgui.hpp
 *
 *  Created on: Feb 1, 2014
 *      Author: gustavo
 */

#ifndef V4LINTERFACE_HPP_
#define V4LINTERFACE_HPP_
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <gtkmm.h>
#include <gtkmm/messagedialog.h>
#include <linux/videodev2.h>
#include <capture/v4lcap.hpp>
#include "Robot.hpp"
#include "ImageView.hpp"

namespace capture {

class V4LInterface: public Gtk::VBox {

    public:

      bool warped = false;

      ImageView imageView;

      double ballX, ballY;

      bool start_game_flag = false;

      std::vector<Robot> robot_list;

      Gtk::Image red_button_released;
          Gtk::Image red_button_pressed;

          Gtk::Label *robot1_pos_lb, *robot2_pos_lb, *robot3_pos_lb;
          Gtk::Label *ball_pos_lb;

      Gtk::Frame robots_id_fm;
          Gtk::HBox info_hbox;
          Gtk::VBox robots_pos_vbox;
          Gtk::HBox robots_pos_hbox[7];
          Gtk::HBox start_game_hbox;
          Gtk::VBox buttons_vbox;
          std::vector<std::string> robot_pos;
          Gtk::Button start_game_bt;

          Gtk::Frame robots_pos_fm;
          Gtk::Frame robots_buttons_fm;
          Gtk::Frame robots_checkbox_fm;
          Gtk::VBox robots_pos_buttons_vbox;
          Gtk::Button robots_save_bt;
          Gtk::Button robots_load_bt;
          Gtk::HBox robots_buttons_hbox;
          Gtk::CheckButton draw_info_checkbox;
          Gtk::HBox draw_info_hbox;
          bool draw_info_flag = false;

      Gtk::VBox robots_id_vbox;
          Gtk::HBox robots_id_hbox[4];
          Gtk::Button robots_id_edit_bt;
          Gtk::Button robots_id_done_bt;
          Gtk::Entry robots_id_box[3];
          Glib::ustring robots_id_tmp[3];
          bool robots_id_edit_flag = false;

          Gtk::Frame robots_speed_fm;
          Gtk::VBox robots_speed_vbox[4];
          Gtk::HScale robots_speed_hscale[3];
          double robots_speed_tmp[3];
          Gtk::HBox robots_speed_hbox[4];
          Gtk::ProgressBar robots_speed_progressBar[3];
          Gtk::Button robots_speed_edit_bt;
          Gtk::Button robots_speed_done_bt;
          bool robots_speed_edit_flag = false;

          Gtk::Frame robots_function_fm;
          Gtk::VBox robots_function_vbox;
          Gtk::HBox robots_function_hbox[4];
          Gtk::ComboBoxText cb_robot_function[3];
          int robots_function_tmp[3];
          Gtk::Button robots_function_edit_bt;
          Gtk::Button robots_function_done_bt;
          bool robots_function_edit_flag = false;

        capture::v4lcap vcap;
        V4LInterface();
        int offsetL;
        int offsetR;
        int Amin[6];
        Gtk::Scale HScale_offsetL;
        Gtk::Scale HScale_offsetR;
        int H[6][2];
        int S[6][2];
        int V[6][2];
        int Img_id = 0;
        bool adjust_event_flag;
        bool invert_image_flag = false;


        bool HSV_calib_event_flag;
        bool auto_calib_flag = false;
        bool quick_save_flag = false;
        bool quick_load_flag = false;

        Gtk::ToggleButton bt_warp;
        Gtk::ToggleButton bt_adjust;
        Gtk::ToggleButton bt_HSV_calib;
        Gtk::ToggleButton bt_auto_calib;
        Gtk::CheckButton bt_invert_image;
        Gtk::Button bt_HSV_left;
        Gtk::Button bt_HSV_right;
        Gtk::Button bt_save_HSV_calib;



        Gtk::Scale HScale_Hmin;
        Gtk::Scale HScale_Smin;
        Gtk::Scale HScale_Vmin;

        Gtk::Scale HScale_Hmax;
        Gtk::Scale HScale_Smax;
        Gtk::Scale HScale_Vmax;

        Gtk::Scale HScale_Amin;

        void grab_rgb(unsigned char * rgb) {
            std::cout << "Grabbing\n";
            vcap.grab_rgb(rgb);
        }

    private:
        void __init_combo_boxes();
        void __create_frm_device_info();
        void __create_frm_device_properties();
        void __create_frm_quick_actions();
        void __create_frm_warp();
        void __create_frm_calibration();

    private:
        // Combo properties updates
        void __update_cb_device();
        void __update_cb_input();
        void __update_cb_standard();
        void __update_cb_format_desc();
        void __update_cb_frame_size();
        void __update_cb_frame_interval();
        void __update_all();

    public:
        // Signals
        sigc::connection cb_input_signal;
        sigc::connection cb_standard_signal;
        sigc::connection cb_format_desc_signal;
        sigc::connection cb_frame_size_signal;
        sigc::connection cb_frame_interval_signal;


        void __event_bt_save_cam_prop_clicked();
        void __event_bt_load_cam_prop_clicked();
        void __event_bt_quick_save_clicked();
        void __event_bt_quick_load_clicked();


        void __event_bt_start_clicked();
        void __event_bt_warp_clicked();
        void __event_bt_adjust_pressed();
        void __event_bt_save_warp_clicked();
        void __event_bt_load_warp_clicked();
        void __event_bt_reset_warp_clicked();
        void __event_bt_save_robots_info_clicked();
        void __event_bt_load_robots_info_clicked();

        void __event_bt_HSV_calib_pressed();
        void __event_bt_auto_calib_pressed();
        void __event_bt_save_HSV_calib_clicked();
        void __event_bt_load_HSV_calib_clicked();
        void __event_bt_right_HSV_calib_clicked();
        void __event_bt_left_HSV_calib_clicked();

        void HScale_offsetR_value_changed();
        void HScale_offsetL_value_changed();
        void __event_bt_invert_image_signal_clicked();

        void HScale_Hmin_value_changed();
        void HScale_Smin_value_changed();
        void HScale_Vmin_value_changed();

        void HScale_Hmax_value_changed();
        void HScale_Smax_value_changed();
        void HScale_Vmax_value_changed();
        void HScale_Amin_value_changed();

        void __event_cb_device_changed();
        void __event_cb_input_changed();
        void __event_cb_standard_changed();
        void __event_cb_format_desc_changed();
        void __event_cb_frame_size_changed();
        void __event_cb_frame_interval_changed();

        void createIDsFrame();
        void createSpeedsFrame();
        void update_speed_progressBars();
        void event_draw_info_checkbox_signal_clicked();
        void createPositionsAndButtonsFrame();
        void createFunctionsFrame();

        void event_robots_function_done_bt_signal_clicked();
        void event_robots_function_edit_bt_signal_clicked();
        void event_start_game_bt_signal_clicked();
        void event_robots_speed_done_bt_signal_clicked();
        void event_robots_speed_edit_bt_signal_pressed();
        void event_robots_id_done_bt_signal_clicked();
        void event_robots_id_edit_bt_signal_pressed();

        void updateRobotLabels();
        void init_HSV();
        bool get_start_game_flag();

    public:

        /* Signals */
        typedef sigc::signal<bool, bool> SignalStart;

        SignalStart signal_start() {
            return m_signal_start;
        }
    protected:
        SignalStart m_signal_start;
        bool on_button_press_event(GdkEventButton * event);
    private:

        //==================================================================
        Gtk::Frame frm_device_info;
        Gtk::ComboBoxText cb_device;
        Gtk::Button bt_start;
        Gtk::Button bt_save_warp;
        Gtk::Button bt_load_warp;
        Gtk::Button bt_reset_warp;
        Gtk::Button bt_load_HSV_calib;
        Gtk::Button bt_save_cam_prop;
        Gtk::Button bt_load_cam_prop;
        //-------------------------------

        Gtk::Frame frm_quick_actions;
        Gtk::Button bt_quick_save;
        Gtk::Button bt_quick_load;


        //-------------------------------
        Gtk::Label lb_device_name;
        Gtk::Label lb_device_card;
        Gtk::Label lb_device_driver;
        Gtk::Label lb_device_bus;
        Gtk::Label right_offset_label;
        Gtk::Label left_offset_label;
        Gtk::Label HSV_label;
        //==================================================================
        Gtk::Frame frm_device_prop;
        Gtk::Frame frm_warp;
        Gtk::Frame frm_calibration;
        Gtk::SpinButton sp_width;
        Gtk::SpinButton sp_height;

        //==================================================================
        Gtk::ComboBox cb_input;
        Gtk::ComboBox cb_standard;
        Gtk::ComboBox cb_format_desc;
        Gtk::ComboBox cb_frame_size;
        Gtk::ComboBox cb_frame_interval;
        //----------------------------------
        Glib::RefPtr<Gtk::ListStore> ls_input;
        Glib::RefPtr<Gtk::ListStore> ls_standard;
        Glib::RefPtr<Gtk::ListStore> ls_format_desc;
        Glib::RefPtr<Gtk::ListStore> ls_frame_size;
        Glib::RefPtr<Gtk::ListStore> ls_frame_interval;
        //----------------------------------

        template<class T> class ModelColumn: public Gtk::TreeModel::ColumnRecord {
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

        //==================================================================
        Gtk::Notebook notebook;
        Gtk::Notebook notebook2;
        //==================================================================

        typedef struct __ctrl_holder {
            struct v4l2_queryctrl qctrl;
            Gtk::Widget * widget;
            sigc::connection con;
        } ControlHolder;
        std::list<ControlHolder> ctrl_list_default;

        void __make_controls();
        void __make_control_list_default();

        void __make_control_table(std::list<ControlHolder>& list, const char * title);

        void __update_control_widgets(std::list<ControlHolder>& list);
        void __block_control_signals(std::list<ControlHolder>& list, bool block);

        bool __set_control_hscale(int type, double val, std::list<ControlHolder> * list, Gtk::Widget * wctrl);
        void __set_control(std::list<ControlHolder> * list, Gtk::Widget * wctrl);

};

}

#endif /* V4LINTERFACE_HPP_ */
