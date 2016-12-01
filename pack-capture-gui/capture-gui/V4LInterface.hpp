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

namespace capture {

class V4LInterface: public Gtk::VBox {

    public:

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
        bool warp_event_flag;
        bool adjust_event_flag;
        bool save_warp_flag;
        bool load_warp_flag;
        bool reset_warp_flag;
        bool invert_image_flag = false;


        bool HSV_calib_event_flag;
        bool save_HSV_calib_flag;
        bool load_HSV_calib_flag;
        bool auto_calib_flag = false;
        bool quick_save_flag = false;
        bool quick_load_flag = false;
        bool save_robots_info_flag = false;
        bool load_robots_info_flag = false;

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
        //std::list<ControlHolder> ctrl_list_user;
        //std::list<ControlHolder> ctrl_list_private;

        void __make_controls();
        void __make_control_list_default();
        //void __make_control_list_user();
        //void __make_control_list_private();

        void __make_control_table(std::list<ControlHolder>& list, const char * title);

        void __update_control_widgets(std::list<ControlHolder>& list);
        void __block_control_signals(std::list<ControlHolder>& list, bool block);

        bool __set_control_hscale(int type, double val, std::list<ControlHolder> * list, Gtk::Widget * wctrl);
        void __set_control(std::list<ControlHolder> * list, Gtk::Widget * wctrl);

};

}

#endif /* V4LINTERFACE_HPP_ */
