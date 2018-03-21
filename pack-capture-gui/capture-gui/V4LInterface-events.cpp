/*
* v4lgui.cpp
*
*  Created on: Feb 1, 2014
*      Author: gustavo
*/

#include "V4LInterface.hpp"
#include "../../cc/filechooser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <jsonSaveManager.h>
#include <CamCap.hpp>

#define DEFAULT_STR " - "

namespace capture {

    bool V4LInterface::on_button_press_event(GdkEventButton *event) {

        // Check if the event is a left(1) button click.
        if ((event->type == GDK_BUTTON_PRESS)) {
            //  std::cout<<"click"<<std::endl;

            return true;
        } else
            return false;

    }

    void V4LInterface::HScale_offsetR_value_changed() {

        offsetR = HScale_offsetR.get_value();

    }

    void V4LInterface::HScale_offsetL_value_changed() {

        offsetL = HScale_offsetL.get_value();

    }

    // signals
    void V4LInterface::__event_bt_quick_save_clicked() {
        std::cout << "QUICK SAVE" << std::endl;
        jsonSaveManager config(this);
		config.save();
    }

    void V4LInterface::__event_bt_save_clicked() {
        std::cout << "SAVE" << std::endl;
        FileChooser loadWindow;

        if (loadWindow.result == Gtk::RESPONSE_OK){
            jsonSaveManager config(this);
			config.save(loadWindow.fileName);
        }
    }

    void V4LInterface::__event_bt_quick_load_clicked() {
        std::cout << "QUICK LOAD" << std::endl;
        jsonSaveManager config(this);
		config.load();
        update_interface_robots();
		update_interface_camera();
    }

    void V4LInterface::__event_bt_load_clicked() {
        std::cout << "LOAD" << std::endl;
        FileChooser loadWindow;

        if (loadWindow.result == Gtk::RESPONSE_OK) {
            jsonSaveManager config(this);
            config.load(loadWindow.fileName);
            update_interface_robots();
            update_interface_camera();
        }
    }


    void V4LInterface::__event_bt_start_clicked() {

        if (!vcap.is_opened()) return;

        Glib::ustring label = bt_start.get_label();


        if (0 == label.compare("start")) {

            bool r;
            // = Pre-configure device ======================================

            // set frame size
            r = vcap.set_frame_size(sp_width.get_value_as_int(), sp_height.get_value_as_int(),
                                    V4L2_BUF_TYPE_VIDEO_CAPTURE);
            if (!r) std::cout << "Can't set frame size!" << std::endl;

            // = Init memory map buffers ===================================
            //			if (vcap.start_capturing(v4lcap::MEM_READ, 2)) {
            //				std::cout << "Can't map device memory!" << std::endl;
            //				return;
            //			}

            if (!vcap.init_mmap(2)) {
                std::cout << "Can't map device memory!" << std::endl;
                return;
            }

            if (!vcap.start_capturing()) {
                std::cout << "Can't start device!" << std::endl;
                return;
            }

            // = Actualize the displayed frame size ========================
            struct v4l2_format format;
            vcap.get_format(&format, V4L2_BUF_TYPE_VIDEO_CAPTURE);

            sp_width.set_value(format.fmt.pix.width);
            sp_height.set_value(format.fmt.pix.height);

            bt_start.set_label("stop");
            // Botão Stop desabilitado até que arrume o bug do malloc do threshold
            bt_start.set_state(Gtk::STATE_INSENSITIVE);
            cb_device.set_state(Gtk::STATE_INSENSITIVE);
            cb_input.set_state(Gtk::STATE_INSENSITIVE);
            cb_standard.set_state(Gtk::STATE_INSENSITIVE);
            cb_frame_size.set_state(Gtk::STATE_INSENSITIVE);
            cb_format_desc.set_state(Gtk::STATE_INSENSITIVE);
            sp_width.set_state(Gtk::STATE_INSENSITIVE);
            sp_height.set_state(Gtk::STATE_INSENSITIVE);
            cb_frame_interval.set_state(Gtk::STATE_INSENSITIVE);
            bt_warp.set_state(Gtk::STATE_NORMAL);
            bt_quick_save.set_state(Gtk::STATE_NORMAL);
            bt_quick_load.set_state(Gtk::STATE_NORMAL);
            bt_save.set_state(Gtk::STATE_NORMAL);
            bt_load.set_state(Gtk::STATE_NORMAL);
            HScale_offsetR.set_state(Gtk::STATE_NORMAL);
            HScale_offsetL.set_state(Gtk::STATE_NORMAL);
            visionGUI.rb_split_view.set_state(Gtk::STATE_NORMAL);
            visionGUI.rb_original_view.set_state(Gtk::STATE_NORMAL);
            visionGUI.bt_HSV_calib.set_state(Gtk::STATE_NORMAL);
            visionGUI.rb_mode_GMM.set_state(Gtk::STATE_NORMAL);
            visionGUI.rb_mode_HSV.set_state(Gtk::STATE_NORMAL);
            visionGUI.bt_record_video.set_state(Gtk::STATE_NORMAL);
            visionGUI.bt_save_picture.set_state(Gtk::STATE_NORMAL);
            visionGUI.en_video_name.set_state(Gtk::STATE_NORMAL);
            visionGUI.en_picture_name.set_state(Gtk::STATE_NORMAL);
            calib_offline.set_state(Gtk::STATE_NORMAL);
            calib_online.set_state(Gtk::STATE_NORMAL);
            btn_camCalib.set_state(Gtk::STATE_NORMAL);
            m_signal_start.emit(true);

        } else {

            if (!vcap.stop_capturing()) {
                std::cout << "Can't stop device!" << std::endl;
                return;
            }

            if (!vcap.uninit_mmap()) {
                std::cout << "Can't unmmap device memory!" << std::endl;
                return;
            }

            bt_start.set_label("start");
            cb_device.set_state(Gtk::STATE_NORMAL);
            cb_input.set_state(Gtk::STATE_NORMAL);
            cb_standard.set_state(Gtk::STATE_NORMAL);
            cb_frame_size.set_state(Gtk::STATE_NORMAL);
            cb_format_desc.set_state(Gtk::STATE_NORMAL);
            sp_width.set_state(Gtk::STATE_NORMAL);
            sp_height.set_state(Gtk::STATE_NORMAL);
            cb_frame_interval.set_state(Gtk::STATE_NORMAL);
            visionGUI.bt_HSV_calib.set_state(Gtk::STATE_INSENSITIVE);
            bt_warp.set_state(Gtk::STATE_INSENSITIVE);
            bt_quick_save.set_state(Gtk::STATE_INSENSITIVE);
            bt_quick_load.set_state(Gtk::STATE_INSENSITIVE);
            bt_save.set_state(Gtk::STATE_INSENSITIVE);
            bt_load.set_state(Gtk::STATE_INSENSITIVE);
            m_signal_start.emit(false);

        }

        return;

    }

    void V4LInterface::__event_bt_warp_clicked() {
        std::cout << "Warp drive engaged" << std::endl;
        if (!imageView.warp_event_flag) {
            imageView.warp_event_flag = true;
            bt_reset_warp.set_state(Gtk::STATE_NORMAL);
            bt_quick_load.set_state(Gtk::STATE_NORMAL);
            bt_quick_save.set_state(Gtk::STATE_NORMAL);
            bt_load.set_state(Gtk::STATE_NORMAL);
            bt_save.set_state(Gtk::STATE_NORMAL);
            //bt_invert_image.set_state(Gtk::STATE_NORMAL);
        } else {
            imageView.warp_event_flag = false;
            bt_reset_warp.set_state(Gtk::STATE_INSENSITIVE);
            bt_quick_load.set_state(Gtk::STATE_INSENSITIVE);
            bt_quick_save.set_state(Gtk::STATE_INSENSITIVE);
            bt_load.set_state(Gtk::STATE_INSENSITIVE);
            bt_save.set_state(Gtk::STATE_INSENSITIVE);
            reset_warp_flag = true;
            //bt_invert_image.set_state(Gtk::STATE_INSENSITIVE);
        }
    }

    void V4LInterface::__event_bt_adjust_pressed() {

        if (!adjust_event_flag) {
            adjust_event_flag = true;
            std::cout << "ADJUST = TRUE" << std::endl;
        } else {
            adjust_event_flag = false;
            std::cout << "ADJUST = FALSE" << std::endl;

        }
    }


    void V4LInterface::__event_bt_reset_warp_clicked() {
        std::cout << "Resetting warp matrix." << std::endl;
        warped = false;
        bt_warp.set_state(Gtk::STATE_NORMAL);
        bt_adjust.set_active(false);
        bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
        adjust_event_flag = false;
        imageView.adjust_rdy = false;
        offsetL = 0;
        offsetR = 0;
        HScale_offsetL.set_value(0);
        HScale_offsetR.set_value(0);
        reset_warp_flag = true;
    }

    void V4LInterface::__event_bt_invert_image_signal_clicked() {
        if (!invert_image_flag) {
            invert_image_flag = true;
            std::cout << "imageView >>>>>>>INVERTED<<<<<<<" << std::endl;
        } else {
            invert_image_flag = false;
            std::cout << "imageView >>>>>>>NORMAL<<<<<<<" << std::endl;
        }
    }

    void V4LInterface::__event_cb_device_changed() {
        if (vcap.is_opened()) {
            vcap.close_device();
        }


        Glib::ustring dev = cb_device.get_active_text();

        if (dev.size() < 1) return;

        if (vcap.open_device(dev.data(), true)) {
            struct v4l2_capability cap;
            vcap.query_capability(&cap);

			camera_card = std::string((const char *) cap.card);

            lb_device_name.set_text(dev.data());
            lb_device_card.set_text((const char *) cap.card);
            lb_device_driver.set_text((const char *) cap.driver);
            lb_device_bus.set_text((const char *) cap.bus_info);

        } else {
            std::cout << "Ooops!" << std::endl;
        }

        __update_all();

        // free memory =============================================================
        Gtk::Widget *page;

        page = notebook.get_nth_page(1);

        while (page) {
            notebook.remove_page(1);
            delete page;
            page = notebook.get_nth_page(1);
        }

        __make_control_list_default();

        __event_bt_quick_load_clicked();

        __make_control_table(ctrl_list_default, "Cam Configs");

        __update_control_widgets(ctrl_list_default);

    }


    void V4LInterface::__event_cb_input_changed() {

        if (cb_input.get_active_row_number() == -1) return;

        Gtk::TreeModel::iterator it = cb_input.get_active();

        Gtk::TreeModel::Row row = *it;
        struct v4l2_input input = row[model_input.m_col_data];

        int r = vcap.set_input(input.index);
        if (!r) std::cout << "Can't set input!" << std::endl;

        __update_all();
    }

    void V4LInterface::__event_cb_standard_changed() {

        if (cb_standard.get_active_row_number() == -1) return;

        Gtk::TreeModel::iterator it = cb_standard.get_active();


        Gtk::TreeModel::Row row = *it;
        struct v4l2_standard standard = row[model_standard.m_col_data];

        int r = vcap.set_standard(standard.id);

        if (!r) std::cout << "Can't set standard!" << std::endl;

        __update_all();

    }

    void V4LInterface::__event_cb_format_desc_changed() {


        if (cb_format_desc.get_active_row_number() == -1) return;

        Gtk::TreeModel::iterator it = cb_format_desc.get_active();

        Gtk::TreeModel::Row row = *it;
        struct v4l2_fmtdesc fmtdesc = row[model_format_desc.m_col_data];

        //int r = vcap.set_pixel_format(fmtdesc.pixelformat, V4L2_BUF_TYPE_VIDEO_CAPTURE);
        int r = vcap.set_pixel_format(fmtdesc.pixelformat);
        if (!r) std::cout << "Can't set format!" << std::endl;


        __update_all();
    }

    void V4LInterface::__event_cb_frame_size_changed() {

        if (cb_frame_size.get_active_row_number() == -1) return;

        Gtk::TreeModel::iterator it = cb_frame_size.get_active();

        Gtk::TreeModel::Row row = *it;
        struct v4l2_frmsizeenum frmsize = row[model_frame_size.m_col_data];

        int r = vcap.set_frame_size(frmsize.discrete.width, frmsize.discrete.height, V4L2_BUF_TYPE_VIDEO_CAPTURE);
        if (!r) std::cout << "Can't set frame size!" << std::endl;


        __update_all();

    }

    void V4LInterface::__event_cb_frame_interval_changed() {

        if (cb_frame_interval.get_active_row_number() == -1) return;

        Gtk::TreeModel::iterator it = cb_frame_interval.get_active();

        Gtk::TreeModel::Row row = *it;
        struct v4l2_frmivalenum frame_interval = row[model_frame_interval.m_col_data];

        int r = vcap.set_frame_interval(frame_interval.discrete);
        if (!r) std::cout << "Can't set frame interval!" << std::endl;

        __update_all();

    }

    bool V4LInterface::__set_control_hscale(int type, double val, std::list<ControlHolder> *list, Gtk::Widget *wctrl) {
        std::list<ControlHolder>::iterator iter;

        for (iter = list->begin(); iter != list->end(); ++iter) {
            if ((*iter).widget == wctrl) break;
        }

        int value = static_cast<Gtk::HScale *>(wctrl)->get_value();
        struct v4l2_queryctrl qctrl = (*iter).qctrl;

        if (!vcap.set_control(qctrl.id, value)) {
            std::cout << "Can not update control [" << qctrl.name << "] with value " << value << std::endl;
            return false;
        }

        struct v4l2_control ctrl;
        if (!vcap.get_control(&ctrl, qctrl.id)) return false;

        __update_control_widgets(ctrl_list_default);

        return true;

    }

    void V4LInterface::__set_control(std::list<ControlHolder> *list, Gtk::Widget *wctrl) {

        std::list<ControlHolder>::iterator iter;
        for (iter = list->begin(); iter != list->end(); ++iter) {
            if ((*iter).widget == wctrl) break;
        }

        int value;
        struct v4l2_queryctrl qctrl = (*iter).qctrl;

        switch (qctrl.type) {
            case V4L2_CTRL_TYPE_INTEGER:
            case V4L2_CTRL_TYPE_INTEGER64:
            case V4L2_CTRL_TYPE_CTRL_CLASS:
            case V4L2_CTRL_TYPE_BITMASK:
            default:
                break;


            case V4L2_CTRL_TYPE_BOOLEAN:
                value = static_cast<Gtk::CheckButton *>(wctrl)->get_active();
                if (!vcap.set_control(qctrl.id, value)) {
                    std::cout << "Can not update control [" << qctrl.name << "] with value " << value << std::endl;
                }
                break;

            case V4L2_CTRL_TYPE_BUTTON:
                if (!vcap.set_control(qctrl.id, 1)) {
                    std::cout << "Can not update control [" << qctrl.name << "] with value " << 1 << std::endl;
                }
                break;

            case V4L2_CTRL_TYPE_STRING:
                break;

            case V4L2_CTRL_TYPE_MENU:
            case V4L2_CTRL_TYPE_INTEGER_MENU:

                Gtk::TreeModel::Children::iterator iter = static_cast<Gtk::ComboBox *>(wctrl)->get_active();
                Gtk::TreeModel::Row row = *iter;
                struct v4l2_querymenu qmenu;
                qmenu = row[model_control_menu.m_col_data];
                if (!vcap.set_control(qctrl.id, qmenu.index)) {
                    std::cout << "Can not update control [" << qctrl.name << "] with value " << qmenu.name << std::endl;
                }
                break;

        }

        __update_control_widgets(ctrl_list_default);

    }

    void V4LInterface::event_robots_id_edit_bt_signal_pressed() {
        if (!robots_id_edit_flag) {
            robots_id_edit_flag = true;
            robots_id_edit_bt.set_label("Cancel");
            robots_id_box[0].set_state(Gtk::STATE_NORMAL);
            robots_id_box[1].set_state(Gtk::STATE_NORMAL);
            robots_id_box[2].set_state(Gtk::STATE_NORMAL);
            robots_id_done_bt.set_state(Gtk::STATE_NORMAL);
            robots_id_tmp[0] = robots_id_box[0].get_active_row_number();
            robots_id_tmp[1] = robots_id_box[1].get_active_row_number();
            robots_id_tmp[2] = robots_id_box[2].get_active_row_number();

        } else {
            robots_id_edit_flag = false;
            robots_id_edit_bt.set_label("Edit");
            robots_id_box[0].set_state(Gtk::STATE_INSENSITIVE);
            robots_id_box[1].set_state(Gtk::STATE_INSENSITIVE);
            robots_id_box[2].set_state(Gtk::STATE_INSENSITIVE);
            robots_id_done_bt.set_state(Gtk::STATE_INSENSITIVE);
            robots_id_box[0].set_active(robots_id_tmp[0]);
            robots_id_box[1].set_active(robots_id_tmp[1]);
            robots_id_box[2].set_active(robots_id_tmp[2]);
        }
    }

    void V4LInterface::event_robots_id_done_bt_signal_clicked() {


        const char *id;
        id = robots_id_box[0].get_active_text().c_str();
        robot_list[0].ID = *id;
        id = robots_id_box[1].get_active_text().c_str();
        robot_list[1].ID = *id;
        id = robots_id_box[2].get_active_text().c_str();
        robot_list[2].ID = *id;

        robots_id_edit_flag = false;
        robots_id_edit_bt.set_label("Edit");
        robots_id_box[0].set_state(Gtk::STATE_INSENSITIVE);
        robots_id_box[1].set_state(Gtk::STATE_INSENSITIVE);
        robots_id_box[2].set_state(Gtk::STATE_INSENSITIVE);
        robots_id_done_bt.set_state(Gtk::STATE_INSENSITIVE);

    }

    void V4LInterface::event_robots_speed_edit_bt_signal_pressed() {
        if (!robots_speed_edit_flag) {
            robots_speed_edit_flag = true;
            robots_speed_edit_bt.set_label("Cancel");
            robots_speed_done_bt.set_state(Gtk::STATE_NORMAL);
            robots_speed_hscale[0].set_state(Gtk::STATE_NORMAL);
            robots_speed_hscale[1].set_state(Gtk::STATE_NORMAL);
            robots_speed_hscale[2].set_state(Gtk::STATE_NORMAL);
            robots_speed_tmp[0] = robots_speed_hscale[0].get_value();
            robots_speed_tmp[1] = robots_speed_hscale[1].get_value();
            robots_speed_tmp[2] = robots_speed_hscale[2].get_value();
        } else {
            robots_speed_edit_flag = false;
            robots_speed_edit_bt.set_label("Edit");
            robots_speed_done_bt.set_state(Gtk::STATE_INSENSITIVE);
            robots_speed_hscale[0].set_state(Gtk::STATE_INSENSITIVE);
            robots_speed_hscale[1].set_state(Gtk::STATE_INSENSITIVE);
            robots_speed_hscale[2].set_state(Gtk::STATE_INSENSITIVE);
            robots_speed_hscale[0].set_value(robots_speed_tmp[0]);
            robots_speed_hscale[1].set_value(robots_speed_tmp[1]);
            robots_speed_hscale[2].set_value(robots_speed_tmp[2]);

        }
    }

    void V4LInterface::event_robots_speed_done_bt_signal_clicked() {

        robot_list[0].vdefault = (float) robots_speed_hscale[0].get_value();
        robot_list[1].vdefault = (float) robots_speed_hscale[1].get_value();
        robot_list[2].vdefault = (float) robots_speed_hscale[2].get_value();
        robot_list[0].vmax = robot_list[0].vdefault;
        robot_list[1].vmax = robot_list[1].vdefault;
        robot_list[2].vmax = robot_list[2].vdefault;
        robots_speed_edit_flag = false;
        robots_speed_edit_bt.set_label("Edit");
        robots_speed_done_bt.set_state(Gtk::STATE_INSENSITIVE);
        robots_speed_hscale[0].set_state(Gtk::STATE_INSENSITIVE);
        robots_speed_hscale[1].set_state(Gtk::STATE_INSENSITIVE);
        robots_speed_hscale[2].set_state(Gtk::STATE_INSENSITIVE);
    }

    void V4LInterface::event_start_game_bt_signal_clicked() {
        if (!start_game_flag) {
            start_game_flag = true;
            start_game_bt.set_image(red_button_pressed);
			btn_camCalib.set_state(Gtk::STATE_INSENSITIVE);
			btn_camCalib.set_active(false);
			btn_camCalib_colect.set_state(Gtk::STATE_INSENSITIVE);
			btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
			btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);
			btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);
            robot_list[0].status = 0;
            robot_list[1].status = 0;
            robot_list[2].status = 0;

            std::string dateString;
            time_t tt;
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            tt = std::chrono::system_clock::to_time_t(now);
            dateString.append(std::string(ctime(&tt)).substr(0, 24));

            if (visionGUI.vision->isRecording()) {
                visionGUI.vision->finishVideo();
                visionGUI.bt_record_video.set_label("REC");
            }
            visionGUI.bt_record_video.set_state(Gtk::STATE_INSENSITIVE);
            visionGUI.en_video_name.set_state(Gtk::STATE_INSENSITIVE);
            visionGUI.en_video_name.set_text(dateString);
            visionGUI.vision->startNewVideo(dateString);
        } else {
            btn_camCalib.set_state(Gtk::STATE_NORMAL);
            visionGUI.vision->finishVideo();
            visionGUI.bt_record_video.set_state(Gtk::STATE_NORMAL);
            visionGUI.en_video_name.set_state(Gtk::STATE_NORMAL);
            visionGUI.en_video_name.set_text("");
            start_game_flag = false;
            start_game_bt.set_image(red_button_released);
            for (int i = 0; i < 3; i++) {
                robot_list.at(i).cmdType = 0; // Position
                robot_list.at(i).vmax = 0;
            }
        }
    }

    void V4LInterface::event_robots_function_edit_bt_signal_clicked() {
        if (!robots_function_edit_flag) {
            robots_function_edit_flag = true;
            robots_function_edit_bt.set_label("Cancel");
            robots_function_edit_bt.set_state(Gtk::STATE_NORMAL);
            cb_robot_function[0].set_state(Gtk::STATE_NORMAL);
            cb_robot_function[1].set_state(Gtk::STATE_NORMAL);
            cb_robot_function[2].set_state(Gtk::STATE_NORMAL);
            robots_function_done_bt.set_state(Gtk::STATE_NORMAL);
            robots_function_tmp[0] = cb_robot_function[0].get_active_row_number();
            robots_function_tmp[1] = cb_robot_function[1].get_active_row_number();
            robots_function_tmp[2] = cb_robot_function[2].get_active_row_number();

        } else {
            robots_function_edit_flag = false;
            robots_function_edit_bt.set_label("Edit");
            cb_robot_function[0].set_state(Gtk::STATE_INSENSITIVE);
            cb_robot_function[1].set_state(Gtk::STATE_INSENSITIVE);
            cb_robot_function[2].set_state(Gtk::STATE_INSENSITIVE);
            robots_function_done_bt.set_state(Gtk::STATE_INSENSITIVE);
            cb_robot_function[0].set_active(robots_function_tmp[0]);
            cb_robot_function[1].set_active(robots_function_tmp[1]);
            cb_robot_function[2].set_active(robots_function_tmp[2]);
        }
    }


    void V4LInterface::event_robots_function_done_bt_signal_clicked() {
        std::string s[3];

        for (int i = 0; i < 3; i++) {
            s[i] = cb_robot_function[i].get_active_text();

            if (s[i].compare("Goalkeeper") == 0) {
                std::cout << "Robot " << i + 1 << ": Goalkeeper." << std::endl;
                robot_list[i].role = 0;
            } else if (s[i].compare("Defense") == 0) {
                std::cout << "Robot " << i + 1 << ": Defense." << std::endl;
                robot_list[i].role = 1;
            } else if (s[i].compare("Attack") == 0) {
                std::cout << "Robot " << i + 1 << ": Attack." << std::endl;
                robot_list[i].role = 2;
            } else if (s[i].compare("Opponent") == 0) {
                std::cout << "Robot " << i + 1 << ": Opponent." << std::endl;
                robot_list[i].role = 3;
            } else {
                std::cout << "Error: not possible to set robot " << i + 1 << " function." << std::endl;
            }


        }

        robots_function_edit_flag = false;
        robots_function_edit_bt.set_label("Edit");
        cb_robot_function[0].set_state(Gtk::STATE_INSENSITIVE);
        cb_robot_function[1].set_state(Gtk::STATE_INSENSITIVE);
        cb_robot_function[2].set_state(Gtk::STATE_INSENSITIVE);
        robots_function_done_bt.set_state(Gtk::STATE_INSENSITIVE);

    }

    void V4LInterface::update_interface_robots() {
        for (int i = 0; i < 3; i++) {
            Robot robot = robot_list[i];
			robots_id_box[i].set_active_text(&robot.ID);
			cb_robot_function[i].set_active(robot.role);
            robots_speed_hscale[i].set_value(robot.vdefault);
            robots_speed_progressBar[i].set_fraction(robot.vmax/1.4);
			std::ostringstream vmax;
            vmax << round(robot.vmax*100)/100;
            robots_speed_progressBar[i].set_text(vmax.str());
        }
    }

    void V4LInterface::update_interface_camera() {
        visionGUI.__event_cb_convertType_changed();

        if(warped) {
            bt_warp.set_state(Gtk::STATE_INSENSITIVE);
            if(imageView.adjust_rdy)
                bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
            else bt_adjust.set_state(Gtk::STATE_NORMAL);
        } else {
            bt_warp.set_state(Gtk::STATE_NORMAL);
            bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
        }

        if(warped || imageView.adjust_rdy)
            bt_reset_warp.set_state(Gtk::STATE_NORMAL);
        else bt_reset_warp.set_state(Gtk::STATE_INSENSITIVE);

        HScale_offsetL.set_value(offsetL);
        HScale_offsetR.set_value(offsetR);

        __update_control_widgets(ctrl_list_default);
    }

    void V4LInterface::__event_camCalib_mode_clicked() {
        if(calib_online.get_active()){
            std::cout << "Online active" << std::endl;
            fr_camCalib_offline.hide();
            fr_camCalib_online.show();
        }else{
            std::cout << "Offline active" << std::endl;
            fr_camCalib_online.hide();
            fr_camCalib_offline.show();
        }
    }

    void V4LInterface::__event_camCalib_online_collect_clicked() {

        if(!get_start_game_flag() && visionGUI.vision->foundChessBoardCorners()){
            visionGUI.vision->saveCamCalibFrame();
			btn_camCalib_pop.set_state(Gtk::STATE_NORMAL);
			btn_camCalib_reset.set_state(Gtk::STATE_NORMAL);
        }
        std::string text = "Pop (" + std::to_string(visionGUI.vision->getCamCalibFrames().size()) + ")";
        btn_camCalib_pop.set_label(text);

        if(visionGUI.vision->getCamCalibFrames().size()>15)
            btn_camCalib_start.set_state(Gtk::STATE_NORMAL);
        else
            btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);

        visionGUI.vision->setFlagCamCalibrated(false);
    }

    void V4LInterface::__event_camCalib_online_pop_clicked() {
        visionGUI.vision->popCamCalibFrames();
        std::string text = "Pop (" + std::to_string(visionGUI.vision->getCamCalibFrames().size()) + ")";
        btn_camCalib_pop.set_label(text);
        if(visionGUI.vision->getCamCalibFrames().size()<=0){
            btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);
            btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
        }else if(visionGUI.vision->getCamCalibFrames().size()<=15)
            btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);

    }

    void V4LInterface::__event_camCalib_online_reset_clicked() {
        btn_camCalib_colect.set_state(Gtk::STATE_NORMAL);
        btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);
        btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
        btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);

    }

    void V4LInterface::__event_camCalib_online_start_clicked() {
        btn_camCalib_colect.set_state(Gtk::STATE_INSENSITIVE);
        btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);
        btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
        btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);
        visionGUI.vision->cameraCalibration();

    }

    void V4LInterface::__event_camCalib_pressed() {

        if(btn_camCalib.get_active()){
            btn_camCalib_colect.set_state(Gtk::STATE_NORMAL);
            CamCalib_flag_event = true;
        }else{
            btn_camCalib_colect.set_state(Gtk::STATE_INSENSITIVE);
            btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);
            btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
            btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);
            CamCalib_flag_event = false;
        }

    }

}
