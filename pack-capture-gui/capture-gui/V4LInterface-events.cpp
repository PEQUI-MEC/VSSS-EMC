/*
 * v4lgui.cpp
 *
 *  Created on: Feb 1, 2014
 *      Author: gustavo
 */

#include "V4LInterface.hpp"

#include <iostream>

#define DEFAULT_STR " - "

namespace capture {

	// signals
	
	void V4LInterface::__event_bt_save_cam_prop_clicked() {
		std::cout<<"saving cam prop"<<std::endl;
			std::ofstream txtFile;
			struct v4l2_queryctrl qctrl;
			struct v4l2_control control;
			txtFile.open("Cam_calib.txt");
			std::list<ControlHolder>::iterator iter;
			
			for (iter = ctrl_list_default.begin(); iter != ctrl_list_default.end(); ++iter) {
			qctrl = (*iter).qctrl;
			vcap.get_control(&control, qctrl.id);
			txtFile <<qctrl.id<<std::endl<<control.value<<std::endl;
		}
		txtFile.close();
		
		}
	void V4LInterface::__event_bt_load_cam_prop_clicked() {
		std::cout<<"loading cam prop"<<std::endl;
			std::ifstream txtFile;
			std::string linha;
			txtFile.open("Cam_calib.txt");
			
			struct v4l2_queryctrl qctrl;
			struct v4l2_control control;
			std::list<ControlHolder>::iterator iter;
			
			for (iter = ctrl_list_default.begin(); iter != ctrl_list_default.end(); ++iter) {
				getline(txtFile, linha); qctrl.id = atoi(linha.c_str());
				getline(txtFile, linha); control.value=atoi(linha.c_str());
				if (!vcap.set_control(qctrl.id, control.value)) {
					std::cout << "Can not load control [ " << qctrl.id << " ] with value " << control.value << std::endl;
				}
		}
		txtFile.close();
		
		__update_control_widgets(ctrl_list_default);
		
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
			cb_device.set_sensitive(false);
			cb_input.set_sensitive(false);
			cb_standard.set_sensitive(false);
			cb_frame_size.set_sensitive(false);
			cb_format_desc.set_sensitive(false);
			sp_width.set_sensitive(false);
			sp_height.set_sensitive(false);
			cb_frame_interval.set_sensitive(false);
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
			m_signal_start.emit(false);
		}

		return;

	}
	
	void V4LInterface::__event_bt_warp_clicked(){
	std::cout<<"Warp drive engaged"<<std::endl;
	if (bt_warp.get_state() == Gtk::STATE_ACTIVE){
	warp_event_flag=true;
	}else{
	warp_event_flag=false;
}
		}
		
	void V4LInterface::__event_bt_adjust_pressed() {
		std::cout<<"Adjusting"<<std::endl;
		if (bt_adjust.get_state() == Gtk::STATE_ACTIVE){
		adjust_event_flag=true;

		}else{
		adjust_event_flag=false;		
		
	}
	}
		
		void V4LInterface::__event_bt_save_warp_clicked(){
	std::cout<<"Saving warp matrix."<<std::endl;
	save_warp_flag=true;
		}
			
		void V4LInterface::__event_bt_load_warp_clicked(){
	std::cout<<"Loading warp matrix"<<std::endl;
	load_warp_flag=true;
		}
		
		void V4LInterface::__event_bt_reset_warp_clicked(){
	std::cout<<"Resetting warp matrix."<<std::endl;
	reset_warp_flag=true;
		}
		
		void V4LInterface::__event_bt_HSV_calib_pressed(){
			
		if (HSV_calib_event_flag) {
			HSV_calib_event_flag=false;
			HScale_Hmin.set_state(Gtk::STATE_INSENSITIVE);
			HScale_Smin.set_state(Gtk::STATE_INSENSITIVE);
			HScale_Vmin.set_state(Gtk::STATE_INSENSITIVE);
			HScale_Hmax.set_state(Gtk::STATE_INSENSITIVE);
			HScale_Smax.set_state(Gtk::STATE_INSENSITIVE);
			HScale_Vmax.set_state(Gtk::STATE_INSENSITIVE);
			HScale_Amin.set_state(Gtk::STATE_INSENSITIVE);
			bt_HSV_left.set_state(Gtk::STATE_INSENSITIVE);
			bt_HSV_right.set_state(Gtk::STATE_INSENSITIVE);
			bt_save_HSV_calib.set_state(Gtk::STATE_INSENSITIVE);
			bt_load_HSV_calib.set_state(Gtk::STATE_INSENSITIVE);
		} else {
			HSV_calib_event_flag=true;
			HScale_Hmin.set_state(Gtk::STATE_ACTIVE);
			HScale_Smin.set_state(Gtk::STATE_ACTIVE);
			HScale_Vmin.set_state(Gtk::STATE_ACTIVE);
			HScale_Hmax.set_state(Gtk::STATE_ACTIVE);
			HScale_Smax.set_state(Gtk::STATE_ACTIVE);
			HScale_Vmax.set_state(Gtk::STATE_ACTIVE);
			HScale_Amin.set_state(Gtk::STATE_ACTIVE);
			bt_HSV_left.set_state(Gtk::STATE_NORMAL);
			bt_HSV_right.set_state(Gtk::STATE_NORMAL);
			bt_save_HSV_calib.set_state(Gtk::STATE_NORMAL);
			bt_load_HSV_calib.set_state(Gtk::STATE_NORMAL);
		}
		
		

	}
		
		void V4LInterface::__event_bt_save_HSV_calib_clicked(){
	std::cout<<"Saving HSV calibs."<<std::endl;
	save_HSV_calib_flag=true;
	std::cout<<save_HSV_calib_flag<<std::endl;
		}
			
		void V4LInterface::__event_bt_load_HSV_calib_clicked(){
	std::cout<<"Loading HSV calibs"<<std::endl;
	load_HSV_calib_flag=true;
		}
		
		void V4LInterface::__event_bt_right_HSV_calib_clicked(){
			
			Img_id=Img_id+1;
			
			if(Img_id>5) Img_id = 0;
			HScale_Amin.set_value(Amin[Img_id]);
				switch(Img_id){
				case 0:
				HSV_label.set_text("Primaria");
				HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;	
				case 1:
				HSV_label.set_text("Secundaria 1");
				HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;
				case 2:
				HSV_label.set_text("Secundaria 2");
				HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;
				case 3:
				HSV_label.set_text("Secundaria 3");
				HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;
				case 4:
				HSV_label.set_text("Bola");
				HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;
				case 5:
				HSV_label.set_text("Adversario");
				HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;
				}
			}
			
		void V4LInterface::__event_bt_left_HSV_calib_clicked(){
			
			Img_id=Img_id-1;
			if(Img_id<0) Img_id = 5;
			HScale_Amin.set_value(Amin[Img_id]);
				switch(Img_id){
				case 0:
				HSV_label.set_text("Primaria");
				HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;	
				case 1:
				HSV_label.set_text("Secundaria 1");
				HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;
				case 2:
				HSV_label.set_text("Secundaria 2");
				HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;
				case 3:
				HSV_label.set_text("Secundaria 3");
					HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;
				case 4:
				HSV_label.set_text("Bola");
				HScale_Hmin.set_value(H[Img_id][0]);
				HScale_Hmax.set_value(H[Img_id][1]);
				
				HScale_Smin.set_value(S[Img_id][0]);
				HScale_Smax.set_value(S[Img_id][1]);
			
				HScale_Vmin.set_value(V[Img_id][0]);
				HScale_Vmax.set_value(V[Img_id][1]);
				break;
				case 5:
				HSV_label.set_text("Adversario");
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
		//__make_control_list_user();
		//__make_control_list_private();

		__make_control_table(ctrl_list_default, "Cam Configs");
		//__make_control_table(ctrl_list_user, "User");
		//__make_control_table(ctrl_list_private, "Private");

		__update_control_widgets(ctrl_list_default);
		//__update_control_widgets(ctrl_list_user);
		//__update_control_widgets(ctrl_list_private);

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
		//__update_control_widgets(ctrl_list_user);
		//__update_control_widgets(ctrl_list_private);

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

//				value = static_cast<Gtk::HScale *>(wctrl)->get_value();
//				if (!vd.set_control(qctrl.id, value)) {
//					std::cout << "Can not update control [" << qctrl.name << "] with value " << value << std::endl;
//				}
//				break;

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
		//__update_control_widgets(ctrl_list_user);
		//__update_control_widgets(ctrl_list_private);

	}

	/*bool V4LInterface::  on_button_press_event(GdkEventButton *event){
	
		if (event->button == 1)
	{
		// Memorize pointer position
		lastXMouse=event->x;
		lastYMouse=event->y;
		
		return true;
	}
		}*/
}

