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

#define DEFAULT_STR " - "

namespace capture {

// signals

	void V4LInterface::__event_bt_quick_save_clicked()
	{
		std::cout << "QUICK SAVE" << std::endl;
		
		if(!V4LInterface::__core_save("quicksave.txt"))
		{
			std::cout<<"Error: could not quick save."<<std::endl;
		}
	}

	void V4LInterface::__event_bt_save_clicked()
	{
		std::cout << "SAVE" << std::endl;

		FileChooser loadWindow;

		if (loadWindow.result == Gtk::RESPONSE_OK)
		    if(!V4LInterface::__core_save(loadWindow.filename.c_str()))
			{
				std::cout<<"Error: could not save."<<std::endl;
			}
		else
		    return;
	}

	void V4LInterface::__event_bt_quick_load_clicked()
	{
		std::cout << "QUICK LOAD" << std::endl;

		if (!V4LInterface::__core_load("quicksave.txt"))
		{
		  std::cout << "Error: could not quick load. Maybe a quicksave.txt file does not exist." << std::endl;
		}
	}

	void V4LInterface::__event_bt_load_clicked()
	{
		std::cout << "LOAD" << std::endl;
		
		FileChooser loadWindow;

		if (loadWindow.result == Gtk::RESPONSE_OK)
			if (!V4LInterface::__core_load(loadWindow.filename.c_str()))
			{
			  std::cout << "Error: could not quick load. The requested file could not be opened." << std::endl;
			}
		else
		    return;
	}


	void V4LInterface::__event_bt_start_clicked() {

		if (!vcap.is_opened()) return;

		Glib::ustring label = bt_start.get_label();


		if (0 == label.compare("start")) {

		    bool r;
		    // = Pre-configure device ======================================

		    // set frame size
		    r = vcap.set_frame_size(sp_width.get_value_as_int(), sp_height.get_value_as_int(), V4L2_BUF_TYPE_VIDEO_CAPTURE);
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
		    bt_start.set_sensitive(false);
		    cb_device.set_sensitive(false);
		    cb_input.set_sensitive(false);
		    cb_standard.set_sensitive(false);
		    cb_frame_size.set_sensitive(false);
		    cb_format_desc.set_sensitive(false);
		    sp_width.set_sensitive(false);
		    sp_height.set_sensitive(false);
		    cb_frame_interval.set_sensitive(false);
		    bt_HSV_calib.set_sensitive(true);
		    bt_warp.set_sensitive(true);
		    bt_quick_save.set_sensitive(true);
		    bt_quick_load.set_sensitive(true);
		    bt_save.set_sensitive(true);
		    bt_load.set_sensitive(true);
		    bt_auto_calib.set_sensitive(true);
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
		    cb_device.set_sensitive(true);
		    cb_input.set_sensitive(true);
		    cb_standard.set_sensitive(true);
		    cb_frame_size.set_sensitive(true);
		    cb_format_desc.set_sensitive(true);
		    sp_width.set_sensitive(true);
		    sp_height.set_sensitive(true);
		    cb_frame_interval.set_sensitive(true);
		    bt_HSV_calib.set_sensitive(false);
		    bt_warp.set_sensitive(false);
		    bt_quick_save.set_sensitive(false);
		    bt_quick_load.set_sensitive(false);
		    bt_save.set_sensitive(false);
		    bt_load.set_sensitive(false);
		    bt_auto_calib.set_sensitive(false);
		    m_signal_start.emit(false);

		}

		return;

	}

	void V4LInterface::__event_bt_warp_clicked() {
		std::cout<<"Warp drive engaged"<<std::endl;
		if (!imageView.warp_event_flag) {
		    imageView.warp_event_flag=true;
		    bt_reset_warp.set_sensitive(true);
		    bt_quick_load.set_sensitive(true);
		    bt_quick_save.set_sensitive(true);
		    bt_load.set_sensitive(true);
		    bt_save.set_sensitive(true);
		    //bt_invert_image.set_sensitive(true);
		} else {
		    imageView.warp_event_flag=false;
		    bt_reset_warp.set_sensitive(false);
		    bt_quick_load.set_sensitive(false);
		    bt_quick_save.set_sensitive(false);
		    bt_load.set_sensitive(false);
		    bt_save.set_sensitive(false);
		    //bt_invert_image.set_sensitive(false);
		}
	}

	void V4LInterface::__event_bt_adjust_pressed() {

		if (!adjust_event_flag) {
		    adjust_event_flag=true;
		    std::cout<<"ADJUST = TRUE"<<std::endl;
		} else {
		    adjust_event_flag=false;
		    std::cout<<"ADJUST = FALSE"<<std::endl;

		}
	}


	void V4LInterface::__event_bt_reset_warp_clicked() {
		std::cout<<"Resetting warp matrix."<<std::endl;
		warped=false;
		bt_warp.set_state(Gtk::STATE_NORMAL);
		bt_adjust.set_active(false);
		bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
		adjust_event_flag = false;
		imageView.adjust_rdy=false;
		offsetL = 0;
		offsetR = 0;
		HScale_offsetL.set_value(0);
		HScale_offsetR.set_value(0);
	}

	void V4LInterface::__event_bt_invert_image_signal_clicked() {
		if (!invert_image_flag)
		{
		    invert_image_flag = true;
		    std::cout << "imageView >>>>>>>INVERTED<<<<<<<" << std::endl;
		}
		else
		{
		    invert_image_flag = false;
		    std::cout << "imageView >>>>>>>NORMAL<<<<<<<" << std::endl;
		}
	}

	void V4LInterface::__event_bt_HSV_calib_pressed() {

		if (HSV_calib_event_flag) {
		    HSV_calib_event_flag=false;
		    HScale_Hmin.set_state(Gtk::STATE_INSENSITIVE);
		    HScale_Smin.set_state(Gtk::STATE_INSENSITIVE);
		    HScale_Vmin.set_state(Gtk::STATE_INSENSITIVE);
		    HScale_Hmax.set_state(Gtk::STATE_INSENSITIVE);
		    HScale_Smax.set_state(Gtk::STATE_INSENSITIVE);
		    HScale_Vmax.set_state(Gtk::STATE_INSENSITIVE);
		    HScale_Amin.set_state(Gtk::STATE_INSENSITIVE);
		    //bt_HSV_left.set_state(Gtk::STATE_INSENSITIVE);
		    //bt_HSV_right.set_state(Gtk::STATE_INSENSITIVE);
		    bt_quick_save.set_state(Gtk::STATE_INSENSITIVE);
		    bt_quick_load.set_state(Gtk::STATE_INSENSITIVE);
			bt_save.set_state(Gtk::STATE_INSENSITIVE);
		    bt_load.set_state(Gtk::STATE_INSENSITIVE);
		    
		    //Auto calib
		    //bt_auto_calib.set_state(Gtk::STATE_INSENSITIVE);
		    //bt_auto_calib.set_active(false);
		    //auto_calib_flag = false;
		    //__event_bt_auto_calib_pressed();

		} else {
		    HSV_calib_event_flag=true;
		    HScale_Hmin.set_state(Gtk::STATE_ACTIVE);
		    HScale_Smin.set_state(Gtk::STATE_ACTIVE);
		    HScale_Vmin.set_state(Gtk::STATE_ACTIVE);
		    HScale_Hmax.set_state(Gtk::STATE_ACTIVE);
		    HScale_Smax.set_state(Gtk::STATE_ACTIVE);
		    HScale_Vmax.set_state(Gtk::STATE_ACTIVE);
		    HScale_Amin.set_state(Gtk::STATE_ACTIVE);
		    //bt_HSV_left.set_state(Gtk::STATE_NORMAL);
		    //bt_HSV_right.set_state(Gtk::STATE_NORMAL);
		    bt_quick_save.set_state(Gtk::STATE_NORMAL);
		    //bt_auto_calib.set_state(Gtk::STATE_NORMAL);
		    bt_quick_load.set_state(Gtk::STATE_NORMAL);
		    bt_save.set_state(Gtk::STATE_NORMAL);
		    bt_load.set_state(Gtk::STATE_NORMAL);
		}



	}

	void V4LInterface::__event_bt_auto_calib_pressed()
	{
		if (!auto_calib_flag)
		{
		    std::cout << "AUTO CALIB ENGAGED" << std::endl;
		    //bt_HSV_left.set_state(Gtk::STATE_NORMAL);
		    //bt_HSV_right.set_state(Gtk::STATE_NORMAL);
		    auto_calib_flag = true;
		}
		else
		{
		    std::cout << "auto calib deactivated" << std::endl;
		    //bt_HSV_left.set_state(Gtk::STATE_INSENSITIVE);
		    //bt_HSV_right.set_state(Gtk::STATE_INSENSITIVE);
		    auto_calib_flag = false;
		}
	}

	void V4LInterface::__event_bt_right_HSV_calib_clicked() {

		Img_id=Img_id+1;

		if(Img_id>5) Img_id = 0;
		HScale_Amin.set_value(Amin[Img_id]);
		switch(Img_id) {
		case 0:
		    HSV_label.set_text("Main");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		case 1:
		    HSV_label.set_text("Secondary 1");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		case 2:
		    HSV_label.set_text("Secondary 2");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		case 3:
		    HSV_label.set_text("Secondary 3");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		case 4:
		    HSV_label.set_text("Ball");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		case 5:
		    HSV_label.set_text("Opponent");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		}
	}

	void V4LInterface::__event_bt_left_HSV_calib_clicked() {

		Img_id=Img_id-1;
		if(Img_id<0) Img_id = 5;
		HScale_Amin.set_value(Amin[Img_id]);
		switch(Img_id) {
		case 0:
		    HSV_label.set_text("Main");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		case 1:
		    HSV_label.set_text("Secondary 1");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		case 2:
		    HSV_label.set_text("Secondary 2");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		case 3:
		    HSV_label.set_text("Secondary 3");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		case 4:
		    HSV_label.set_text("Ball");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
		case 5:
		    HSV_label.set_text("Opponent");
		    HScale_Hmin.set_value(H[Img_id][0]);
		    HScale_Hmax.set_value(H[Img_id][1]);

		    HScale_Smin.set_value(S[Img_id][0]);
		    HScale_Smax.set_value(S[Img_id][1]);

		    HScale_Vmin.set_value(V[Img_id][0]);
		    HScale_Vmax.set_value(V[Img_id][1]);
		    break;
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

		    lb_device_name.set_text(dev.data());
		    lb_device_card.set_text((const char *) cap.card);
		    lb_device_driver.set_text((const char *) cap.driver);
		    lb_device_bus.set_text((const char *) cap.bus_info);

		} else {
		    std::cout << "Ooops!" << std::endl;
		}

		__update_all();

		// free memory =============================================================
		Gtk::Widget * page;

		page = notebook.get_nth_page(1);

		while (page) {
		    notebook.remove_page(1);
		    delete page;
		    page = notebook.get_nth_page(1);
		}

		__make_control_list_default();

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

	bool V4LInterface::__set_control_hscale(int type, double val, std::list<ControlHolder> * list, Gtk::Widget * wctrl) {
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

	void V4LInterface::__set_control(std::list<ControlHolder> * list, Gtk::Widget * wctrl) {

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

	void V4LInterface::event_draw_info_checkbox_signal_clicked(){
		draw_info_flag = !draw_info_flag;
	}

	void V4LInterface::event_robots_id_edit_bt_signal_pressed(){
        if (!robots_id_edit_flag)
        {
            robots_id_edit_flag = true;
            robots_id_edit_bt.set_label("Cancel");
            robots_id_box[0].set_state(Gtk::STATE_NORMAL);
            robots_id_box[1].set_state(Gtk::STATE_NORMAL);
            robots_id_box[2].set_state(Gtk::STATE_NORMAL);
            robots_id_done_bt.set_state(Gtk::STATE_NORMAL);
            robots_id_tmp[0] = robots_id_box[0].get_text();
            robots_id_tmp[1] = robots_id_box[1].get_text();
            robots_id_tmp[2] = robots_id_box[2].get_text();

        }
        else
        {
            robots_id_edit_flag = false;
            robots_id_edit_bt.set_label("Edit");
            robots_id_box[0].set_state(Gtk::STATE_INSENSITIVE);
            robots_id_box[1].set_state(Gtk::STATE_INSENSITIVE);
            robots_id_box[2].set_state(Gtk::STATE_INSENSITIVE);
            robots_id_done_bt.set_state(Gtk::STATE_INSENSITIVE);
            robots_id_box[0].set_text(robots_id_tmp[0]);
            robots_id_box[1].set_text(robots_id_tmp[1]);
            robots_id_box[2].set_text(robots_id_tmp[2]);
        }
    }

    void V4LInterface::event_robots_id_done_bt_signal_clicked(){
        std::string str;
        str = robots_id_box[0].get_text();
        robot_list[0].ID = str[0];
        str = robots_id_box[1].get_text();
        robot_list[1].ID = str[0];
        str = robots_id_box[2].get_text();
        robot_list[2].ID = str[0];

        robots_id_edit_flag = false;
        robots_id_edit_bt.set_label("Edit");
        robots_id_box[0].set_state(Gtk::STATE_INSENSITIVE);
        robots_id_box[1].set_state(Gtk::STATE_INSENSITIVE);
        robots_id_box[2].set_state(Gtk::STATE_INSENSITIVE);
        robots_id_done_bt.set_state(Gtk::STATE_INSENSITIVE);

    }

    void V4LInterface::event_robots_speed_edit_bt_signal_pressed(){
        if (!robots_speed_edit_flag)
        {
            robots_speed_edit_flag = true;
            robots_speed_edit_bt.set_label("Cancel");
            robots_speed_done_bt.set_state(Gtk::STATE_NORMAL);
            robots_speed_hscale[0].set_state(Gtk::STATE_NORMAL);
            robots_speed_hscale[1].set_state(Gtk::STATE_NORMAL);
            robots_speed_hscale[2].set_state(Gtk::STATE_NORMAL);
            robots_speed_tmp[0] = robots_speed_hscale[0].get_value();
            robots_speed_tmp[1] = robots_speed_hscale[1].get_value();
            robots_speed_tmp[2] = robots_speed_hscale[2].get_value();
        }
        else
        {
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

    void V4LInterface::event_robots_speed_done_bt_signal_clicked(){
        robot_list[0].vmax = (float) robots_speed_hscale[0].get_value();
        robot_list[1].vmax = (float) robots_speed_hscale[1].get_value();
        robot_list[2].vmax = (float) robots_speed_hscale[2].get_value();
        robots_speed_edit_flag = false;
        robots_speed_edit_bt.set_label("Edit");
        robots_speed_done_bt.set_state(Gtk::STATE_INSENSITIVE);
        robots_speed_hscale[0].set_state(Gtk::STATE_INSENSITIVE);
        robots_speed_hscale[1].set_state(Gtk::STATE_INSENSITIVE);
        robots_speed_hscale[2].set_state(Gtk::STATE_INSENSITIVE);

    }

    void V4LInterface::event_start_game_bt_signal_clicked(){
        if (!start_game_flag)
        {
            start_game_flag = true;
            start_game_bt.set_image(red_button_pressed);
        }
        else
        {
            start_game_flag = false;
            start_game_bt.set_image(red_button_released);
        }

        for(int i=0; i<3; i++)
            robot_list[i].histWipe();
    }

    void V4LInterface::event_robots_function_edit_bt_signal_clicked(){
        if (!robots_function_edit_flag)
        {
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

        }
        else
        {
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

    void V4LInterface::event_robots_function_done_bt_signal_clicked(){
        std::string s[3];

        for (int i = 0; i < 3; i++)
        {
            s[i] = cb_robot_function[i].get_active_text();

            if (s[i].compare("Goalkeeper") == 0)
            {
                std::cout << "Robot " << i+1 << ": Goalkeeper." << std::endl;
                robot_list[i].role = 0;
            }
            else if (s[i].compare("Defense") == 0)
            {
                std::cout << "Robot " << i+1 << ": Defense." << std::endl;
                robot_list[i].role = 1;
            }
            else if (s[i].compare("Attack") == 0)
            {
                std::cout << "Robot " << i+1 << ": Attack." << std::endl;
                robot_list[i].role = 2;
            }
            else if (s[i].compare("Opponent") == 0)
            {
                std::cout << "Robot " << i+1 << ": Opponent." << std::endl;
                robot_list[i].role = 3;
            }
            else
            {
                std::cout << "Error: not possible to set robot " << i+1 << " function." << std::endl;
            }


        }

        robots_function_edit_flag = false;
        robots_function_edit_bt.set_label("Edit");
        cb_robot_function[0].set_state(Gtk::STATE_INSENSITIVE);
        cb_robot_function[1].set_state(Gtk::STATE_INSENSITIVE);
        cb_robot_function[2].set_state(Gtk::STATE_INSENSITIVE);
        robots_function_done_bt.set_state(Gtk::STATE_INSENSITIVE);

    }
    
    
    bool V4LInterface::__core_save(const char * txtFileName)
	{
		std::ofstream txtFile;
		txtFile.open(txtFileName);
		

		if (txtFile.is_open())
		{
			// !BEGIN_INFO
			std::cout<<"Saving robots info..."<<std::endl;
			
			// sempre é salvo 9 linhas
			for (int i = 0; i < 3; i++) {
				txtFile << robots_id_box[i].get_text() <<std::endl;
				txtFile << cb_robot_function[i].get_active_row_number() <<std::endl;
				txtFile << robots_speed_hscale[i].get_value() <<std::endl;
			}
			// !END_INFO
		
			// !BEGIN_HSV
			std::cout<<"Saving HSV calibs..."<<std::endl;
			// salva sempre 42 linhas linhas
			for(int i=0; i<6; i++) {
			  txtFile <<H[i][0]<<std::endl<<H[i][1]<<std::endl;
			  txtFile <<S[i][0]<<std::endl<<S[i][1]<<std::endl;
			  txtFile <<V[i][0]<<std::endl<<V[i][1]<<std::endl;
			  txtFile <<Amin[i]<<std::endl;
			}
			// !END_HSV
		
			// !BEGIN_WARP
			std::cout<<"Saving warp matrix..."<<std::endl;
			// salva sempre 18 linhas
			txtFile << imageView.warp_mat[0][0] <<std::endl<<imageView.warp_mat[0][1] <<std::endl;
			txtFile << imageView.warp_mat[1][0] <<std::endl<<imageView.warp_mat[1][1] <<std::endl;
			txtFile << imageView.warp_mat[2][0] <<std::endl<<imageView.warp_mat[2][1] <<std::endl;
			txtFile << imageView.warp_mat[3][0] <<std::endl<<imageView.warp_mat[3][1] <<std::endl;
			txtFile << offsetL <<std::endl<<offsetR <<std::endl;
			txtFile << imageView.adjust_mat[0][0] <<std::endl<<imageView.adjust_mat[0][1] <<std::endl;
			txtFile << imageView.adjust_mat[1][0] <<std::endl<<imageView.adjust_mat[1][1] <<std::endl;
			txtFile << imageView.adjust_mat[2][0] <<std::endl<<imageView.adjust_mat[2][1] <<std::endl;
			txtFile << imageView.adjust_mat[3][0] <<std::endl<<imageView.adjust_mat[3][1] <<std::endl;
			// !END_WARP
			
			// !BEGIN_CAM
			std::cout<<"Saving cam prop..."<<std::endl;
		
			struct v4l2_queryctrl qctrl;
			struct v4l2_control control;
			std::list<ControlHolder>::iterator iter;
			
			// sempre salva um id e seu valor
			// como o número de linhas é o único que pode ser variável, coloquei por último no arquivo
			for (iter = ctrl_list_default.begin(); iter != ctrl_list_default.end(); ++iter) {
			  qctrl = (*iter).qctrl;
			  vcap.get_control(&control, qctrl.id);
			  txtFile <<qctrl.id<<std::endl<<control.value<<std::endl;
			}
			// !END_CAM
		
			txtFile.close();
			
			return true;
		}
		else
		{
			return false;
		}
	}
	
	bool V4LInterface::__core_load(const char * txtFileName)
	{
		std::ifstream txtFile;
		txtFile.open(txtFileName);
		std::string line;
		
		if (txtFile.is_open())
		{
			// !BEGIN_INFO
			std::cout<<"Loading robots info..."<<std::endl;
			
			// lê os três valores para cada robô -- sempre 9 linhas
			for (int i = 0; i < 3; i++) {
			  getline(txtFile, line);
			  robots_id_box[i].set_text(line.c_str());
			  robot_list[i].ID = line.c_str()[0];

			  getline(txtFile, line);
			  cb_robot_function[i].set_active(atoi(line.c_str()));
			  if (cb_robot_function[i].get_active_row_number() == 0)
			  {
				  std::cout << "Robot " << i+1 << ": Goalkeeper." << std::endl;
				  robot_list[i].role = 0;
			  }
			  else if (cb_robot_function[i].get_active_row_number() == 1)
			  {
				  std::cout << "Robot " << i+1 << ": Defense." << std::endl;
				  robot_list[i].role = 1;
			  }
			  else if (cb_robot_function[i].get_active_row_number() == 2)
			  {
				  std::cout << "Robot " << i+1 << ": Attack." << std::endl;
				  robot_list[i].role = 2;
			  }
			   else if (cb_robot_function[i].get_active_row_number() == 3)
			  {
				  std::cout << "Robot " << i+1 << ": Opponent." << std::endl;
				  robot_list[i].role = 3;
			  }
			  else
			  {
				  std::cout << "Error: not possible to set robot " << i+1 << " function." << std::endl;
			  }
			  
			  getline(txtFile, line);
			  double value = atof(line.c_str());
			  std::cout << "ATOF " << atof(line.c_str()) << std::endl;
			  robots_speed_hscale[i].set_value(value);
			  robot_list[i].vmax = (float) value;

			  robots_speed_progressBar[i].set_fraction(robots_speed_hscale[i].get_value()/6);
			  std::ostringstream strs;
			  strs << robots_speed_hscale[i].get_value();
			  std::string str = strs.str();
			  robots_speed_progressBar[i].set_text(str.substr(0,4));
			}
			// !END_INFO
			
			// !BEGIN_HSV
			std::cout<<"Loading HSV calibs..."<<std::endl;
			for(int i=0; i<6; i++) {
			  getline(txtFile, line);
			  H[i][0]=atoi(line.c_str());
			  getline(txtFile, line);
			  H[i][1]=atoi(line.c_str());
			  getline(txtFile, line);
			  S[i][0]=atoi(line.c_str());
			  getline(txtFile, line);
			  S[i][1]=atoi(line.c_str());
			  getline(txtFile, line);
			  V[i][0]=atoi(line.c_str());
			  getline(txtFile, line);
			  V[i][1]=atoi(line.c_str());
			  getline(txtFile, line);
			  Amin[i]=atoi(line.c_str());
			}

			HScale_Hmin.set_value(H[Img_id][0]);
			HScale_Hmax.set_value(H[Img_id][1]);

			HScale_Smin.set_value(S[Img_id][0]);
			HScale_Smax.set_value(S[Img_id][1]);

			HScale_Vmin.set_value(V[Img_id][0]);
			HScale_Vmax.set_value(V[Img_id][1]);
			HScale_Amin.set_value(Amin[Img_id]);
			// !END_HSV
			
			// !BEGIN_WARP
			// não precisa ser modificado, não usa o mesmo stream
			__event_bt_warp_clicked();
			// !END_WARP
			
			// !BEGIN_LOADWARP
			std::cout<<"Loading warp matrix..."<<std::endl;

			getline(txtFile, line);
			imageView.warp_mat[0][0] = atoi(line.c_str());
			getline(txtFile, line);
			imageView.warp_mat[0][1] = atoi(line.c_str());
			//std::cout<< imageView.warp_mat[0][0] <<std::endl<<imageView.warp_mat[0][1] <<std::endl;

			getline(txtFile, line);
			imageView.warp_mat[1][0] = atoi(line.c_str());
			getline(txtFile, line);
			imageView.warp_mat[1][1] = atoi(line.c_str());
			//std::cout<< imageView.warp_mat[1][0] <<std::endl<<imageView.warp_mat[1][1] <<std::endl;

			getline(txtFile, line);
			imageView.warp_mat[2][0] = atoi(line.c_str());
			getline(txtFile, line);
			imageView.warp_mat[2][1] = atoi(line.c_str());
			//std::cout<< imageView.warp_mat[2][0] <<std::endl<<imageView.warp_mat[2][1] <<std::endl;

			getline(txtFile, line);
			imageView.warp_mat[3][0] = atoi(line.c_str());
			getline(txtFile, line);
			imageView.warp_mat[3][1] = atoi(line.c_str());
			//std::cout<< imageView.warp_mat[3][0] <<std::endl<<imageView.warp_mat[3][1] <<std::endl;
			getline(txtFile, line);
			offsetL = atoi(line.c_str());
			getline(txtFile, line);
			offsetR = atoi(line.c_str());

			getline(txtFile, line);
			imageView.adjust_mat[0][0] = atoi(line.c_str());
			getline(txtFile, line);
			imageView.adjust_mat[0][1] = atoi(line.c_str());
			//std::cout<< imageView.warp_mat[0][0] <<std::endl<<imageView.warp_mat[0][1] <<std::endl;

			getline(txtFile, line);
			imageView.adjust_mat[1][0] = atoi(line.c_str());
			getline(txtFile, line);
			imageView.adjust_mat[1][1] = atoi(line.c_str());
			//std::cout<< imageView.warp_mat[1][0] <<std::endl<<imageView.warp_mat[1][1] <<std::endl;

			getline(txtFile, line);
			imageView.adjust_mat[2][0] = atoi(line.c_str());
			getline(txtFile, line);
			imageView.adjust_mat[2][1] = atoi(line.c_str());
			//std::cout<< imageView.warp_mat[2][0] <<std::endl<<imageView.warp_mat[2][1] <<std::endl;

			getline(txtFile, line);
			imageView.adjust_mat[3][0] = atoi(line.c_str());
			getline(txtFile, line);
			imageView.adjust_mat[3][1] = atoi(line.c_str());
			//std::cout<< imageView.warp_mat[3][0] <<std::endl<<imageView.warp_mat[3][1] <<std::endl;

			bt_adjust.set_state(Gtk::STATE_INSENSITIVE);

			warped=true;
			imageView.adjust_rdy = true;
			HScale_offsetL.set_value(offsetL);
			HScale_offsetR.set_value(offsetR);
			imageView.warp_event_flag =false;
			// !END_LOADWARP
			
			// !BEGIN_CAM
			std::cout<<"Loading cam prop..."<<std::endl;

			struct v4l2_queryctrl qctrl;
			struct v4l2_control control;
			std::list<ControlHolder>::iterator iter;
			
			for (iter = ctrl_list_default.begin(); iter != ctrl_list_default.end() && !txtFile.eof(); ++iter) {
			  getline(txtFile, line);
			  qctrl.id = atoi(line.c_str());
			  getline(txtFile, line);
			  control.value=atoi(line.c_str());
			  if (!vcap.set_control(qctrl.id, control.value)) {
				  std::cout << "Can not load control [ " << qctrl.id << " ] with value " << control.value << std::endl;
			  }
			}

			__update_control_widgets(ctrl_list_default);
			// !END_CAM
			
			txtFile.close();
			return true;
		}
		else
		{
		  return false;
		}
	}

}
