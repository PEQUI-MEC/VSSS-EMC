/*
* v4lgui.cpp
*
*  Created on: Feb 1, 2014
*      Author: gustavo
*/

#include "V4LInterface.hpp"
#include <jsonSaveManager.h>
#include <thread>

#define DEFAULT_STR " - "

using capture::V4LInterface;

bool V4LInterface::on_button_press_event(GdkEventButton *event) {

	// Check if the event is a left(1) button click.
	return event->type == GDK_BUTTON_PRESS;
}

void V4LInterface::HScale_offsetR_value_changed() {

	offsetR = static_cast<int>(HScale_offsetR.get_value());
}

void V4LInterface::HScale_offsetL_value_changed() {

	offsetL = static_cast<int>(HScale_offsetL.get_value());
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

	if (loadWindow.result == Gtk::RESPONSE_OK) {
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
		struct v4l2_format format{};
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
		visionGUI.bt_LAB_calib.set_state(Gtk::STATE_NORMAL);
		visionGUI.rb_mode_GMM.set_state(Gtk::STATE_NORMAL);
		visionGUI.rb_mode_CIELAB.set_state(Gtk::STATE_NORMAL);
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
		visionGUI.bt_LAB_calib.set_state(Gtk::STATE_INSENSITIVE);
		bt_warp.set_state(Gtk::STATE_INSENSITIVE);
		bt_quick_save.set_state(Gtk::STATE_INSENSITIVE);
		bt_quick_load.set_state(Gtk::STATE_INSENSITIVE);
		bt_save.set_state(Gtk::STATE_INSENSITIVE);
		bt_load.set_state(Gtk::STATE_INSENSITIVE);
		m_signal_start.emit(false);
	}
}

void V4LInterface::__event_bt_warp_clicked() {
	if (!imageView.warp_event_flag) {
		imageView.warp_event_flag = true;
		bt_reset_warp.set_state(Gtk::STATE_NORMAL);
		bt_quick_load.set_state(Gtk::STATE_NORMAL);
		bt_quick_save.set_state(Gtk::STATE_NORMAL);
		bt_load.set_state(Gtk::STATE_NORMAL);
		bt_save.set_state(Gtk::STATE_NORMAL);
	} else {
		imageView.warp_event_flag = false;
		bt_reset_warp.set_state(Gtk::STATE_INSENSITIVE);
		bt_quick_load.set_state(Gtk::STATE_INSENSITIVE);
		bt_quick_save.set_state(Gtk::STATE_INSENSITIVE);
		bt_load.set_state(Gtk::STATE_INSENSITIVE);
		bt_save.set_state(Gtk::STATE_INSENSITIVE);
		reset_warp_flag = true;
	}
}

void V4LInterface::__event_bt_adjust_pressed() {

	adjust_event_flag = !adjust_event_flag;
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

	if (dev.empty()) return;

	if (vcap.open_device(dev.data(), true)) {
		struct v4l2_capability cap{};
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

	double value = dynamic_cast<Gtk::HScale *>(wctrl)->get_value();
	struct v4l2_queryctrl qctrl = (*iter).qctrl;

	if (!vcap.set_control(qctrl.id, static_cast<int>(value))) {
		std::cout << "Can not update control [" << qctrl.name << "] with value " << value << std::endl;
		return false;
	}

	struct v4l2_control ctrl{};
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
			value = dynamic_cast<Gtk::CheckButton *>(wctrl)->get_active();
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

			Gtk::TreeModel::Children::iterator treeIter = static_cast<Gtk::ComboBox *>(wctrl)->get_active();
			Gtk::TreeModel::Row row = *treeIter;
			struct v4l2_querymenu qmenu{};
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
		robots_auto_bt.set_state(Gtk::STATE_INSENSITIVE);
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
		robots_auto_bt.set_state(Gtk::STATE_NORMAL);
		robots_id_box[0].set_active(robots_id_tmp[0]);
		robots_id_box[1].set_active(robots_id_tmp[1]);
		robots_id_box[2].set_active(robots_id_tmp[2]);
	}
}

void V4LInterface::discover_robot_ids() {
	// FIXME: Implementar discoberta automatica de IDs para novo Robot
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
}

void V4LInterface::event_robots_auto_bt_signal_pressed() {
	robots_auto_bt.set_state(Gtk::STATE_INSENSITIVE);
	robots_id_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
	std::thread discover_robots_thread(&V4LInterface::discover_robot_ids, this);
	discover_robots_thread.detach();
}

void V4LInterface::event_robots_id_done_bt_signal_clicked() {
	for (Robot2* robot : robots) {
		if(robot->tag < 3) {
			const char * id = robots_id_box[robot->tag].get_active_text().c_str();
			robot->ID = *id;
		}
	}

	robots_id_edit_flag = false;
	robots_id_edit_bt.set_label("Edit");
	robots_id_box[0].set_state(Gtk::STATE_INSENSITIVE);
	robots_id_box[1].set_state(Gtk::STATE_INSENSITIVE);
	robots_id_box[2].set_state(Gtk::STATE_INSENSITIVE);
	robots_id_done_bt.set_state(Gtk::STATE_INSENSITIVE);
	robots_auto_bt.set_state(Gtk::STATE_NORMAL);
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
	for (int i = 0; i < 3; i++) {
		robots[i]->default_target_velocity = (float) robots_speed_hscale[i].get_value();
		robots_speed_hscale[i].set_state(Gtk::STATE_INSENSITIVE);
	}
	robots_speed_edit_flag = false;
	robots_speed_edit_bt.set_label("Edit");
	robots_speed_done_bt.set_state(Gtk::STATE_INSENSITIVE);
}

void V4LInterface::event_disable_video_record(){
	visionGUI.vision->video_rec_enable = !visionGUI.vision->video_rec_enable;
}

void V4LInterface::event_start_game_bt_signal_clicked() {
	if (!start_game_flag) {
		record_video_checkbox.set_sensitive(false);
		start_game_flag = true;
		start_game_bt.set_image(red_button_pressed);
		btn_camCalib.set_state(Gtk::STATE_INSENSITIVE);
		btn_camCalib.set_active(false);
		btn_camCalib_collect.set_state(Gtk::STATE_INSENSITIVE);
		btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
		btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);
		btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);

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
		if(visionGUI.vision->video_rec_enable)
			visionGUI.vision->startNewVideo(dateString);
	} else {
		record_video_checkbox.set_sensitive(true);
		btn_camCalib.set_state(Gtk::STATE_NORMAL);
		if(visionGUI.vision->video_rec_enable)
			visionGUI.vision->finishVideo();
		visionGUI.bt_record_video.set_state(Gtk::STATE_NORMAL);
		visionGUI.en_video_name.set_state(Gtk::STATE_NORMAL);
		visionGUI.en_video_name.set_text("");
		start_game_flag = false;
		start_game_bt.set_image(red_button_released);
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
	for (int i = 0; i < robots.size(); ++i) {
		int tag = cb_robot_function[i].get_active_row_number();
		robots[i]->tag = static_cast<unsigned int>(tag);
		robots[i]->ID = (char) robots_id_box[robots[i]->tag].get_active_row_number() + 'A';
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
		Robot2* robot = robots[i];

		cb_robot_function[i].set_active(robot->tag);
		robots_id_box[robot->tag].set_active(robot->ID - 'A');

		robots_speed_hscale[i].set_value(robot->default_target_velocity);
		robots_speed_progressBar[i].set_fraction(robot->get_target().velocity / 1.4);
		const std::string velocity = std::to_string(round(robot->get_target().velocity * 100) / 100);
		robots_speed_progressBar[i].set_text(velocity);
	}
}

void V4LInterface::update_interface_camera() {
	visionGUI.update_vision_hscale_values();

	if (warped) {
		bt_warp.set_state(Gtk::STATE_INSENSITIVE);
		if (imageView.adjust_rdy)
			bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
		else bt_adjust.set_state(Gtk::STATE_NORMAL);
	} else {
		bt_warp.set_state(Gtk::STATE_NORMAL);
		bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
	}

	if (warped || imageView.adjust_rdy)
		bt_reset_warp.set_state(Gtk::STATE_NORMAL);
	else bt_reset_warp.set_state(Gtk::STATE_INSENSITIVE);

	HScale_offsetL.set_value(offsetL);
	HScale_offsetR.set_value(offsetR);

	__update_control_widgets(ctrl_list_default);
}

void V4LInterface::__event_camCalib_mode_clicked() {
	if (calib_online.get_active()) {
		std::cout << "Online active" << std::endl;
		fr_camCalib_offline.hide();
		fr_camCalib_online.show();
	} else {
		std::cout << "Offline active" << std::endl;
		fr_camCalib_online.hide();
		fr_camCalib_offline.show();
		btn_camCalib_offline_start.set_state(Gtk::STATE_NORMAL);
	}
}

void V4LInterface::__event_camCalib_online_collect_clicked() {

	if (!get_start_game_flag() && visionGUI.vision->foundChessBoardCorners()) {
		visionGUI.vision->saveCamCalibFrame();
		btn_camCalib_pop.set_state(Gtk::STATE_NORMAL);
		btn_camCalib_reset.set_state(Gtk::STATE_NORMAL);
	}
	std::string text = "Pop (" + std::to_string(visionGUI.vision->getCamCalibFrames().size()) + ")";
	btn_camCalib_pop.set_label(text);

	if (visionGUI.vision->getCamCalibFrames().size() > 15)
		btn_camCalib_start.set_state(Gtk::STATE_NORMAL);

	visionGUI.vision->setFlagCamCalibrated(false);
}

void V4LInterface::__event_camCalib_online_pop_clicked() {
	visionGUI.vision->popCamCalibFrames();
	std::string text = "Pop (" + std::to_string(visionGUI.vision->getCamCalibFrames().size()) + ")";
	btn_camCalib_pop.set_label(text);
	if (visionGUI.vision->getCamCalibFrames().empty()) {
		btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);
		btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
	} else if (visionGUI.vision->getCamCalibFrames().size() <= 15)
		btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);
}

void V4LInterface::__event_camCalib_online_reset_clicked() {
	visionGUI.vision->flag_cam_calibrated = false;
	btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib_collect.set_state(Gtk::STATE_NORMAL);
}

void V4LInterface::__event_camCalib_online_start_clicked() {
	btn_camCalib_collect.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);
	visionGUI.vision->cameraCalibration();
	if (visionGUI.vision->flag_cam_calibrated) {
		btn_camCalib_collect.set_state(Gtk::STATE_INSENSITIVE);
		btn_camCalib_offline_start.set_state(Gtk::STATE_INSENSITIVE);
		btn_camCalib_reset.set_state(Gtk::STATE_NORMAL);
		btn_camCalib_pop.set_label("Pop(0)");
	}
}

void V4LInterface::__event_camCalib_offline_start_clicked() {
	std::cout << "Offline active" << std::endl;
	visionGUI.vision->collectImagesForCalibration();
}

void V4LInterface::__event_camCalib_pressed() {

	if (btn_camCalib.get_active()) {
		if (visionGUI.vision->flag_cam_calibrated) {
			btn_camCalib_collect.set_state(Gtk::STATE_INSENSITIVE);
			btn_camCalib_offline_start.set_state(Gtk::STATE_INSENSITIVE);
			btn_camCalib_reset.set_state(Gtk::STATE_NORMAL);
			btn_camCalib_pop.set_label("Pop(0)");
			CamCalib_flag_event = true;
		}
//            visionGUI.vision->flag_cam_calibrated = false;
//            btn_camCalib_collect.set_state(Gtk::STATE_NORMAL);
//            CamCalib_flag_event = true;
	} else {
		btn_camCalib_collect.set_state(Gtk::STATE_INSENSITIVE);
		btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);
		btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
		btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);
		CamCalib_flag_event = false;
	}
}
