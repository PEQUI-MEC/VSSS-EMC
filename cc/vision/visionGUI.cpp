#include "visionGUI.hpp"


using namespace vision;
using namespace date;

std::string date::generate_date_str() {
	std::string date;
	time_t tt;
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	tt = std::chrono::system_clock::to_time_t(now);
	date.append(std::string(ctime(&tt)).substr(0, 24));

	return date;
}

void VisionGUI::__create_frm_drawing_options() {
	Gtk::Frame *frame;
	Gtk::HBox *hbox;

	hbox = new Gtk::HBox();
	frame = new Gtk::Frame();

	pack_start(*frame, false, true, 5);
	frame->add(*hbox);
	frame->set_label("Drawing Options");
	hbox->set_halign(Gtk::ALIGN_CENTER);
	hbox->pack_start(draw_info_checkbox, false, true, 5);
	draw_info_checkbox.set_label("Draw Indicators on Image");
	draw_info_checkbox.set_can_focus(false);
	draw_info_checkbox.set_margin_bottom(5);
	draw_info_checkbox.set_active(true);

	draw_info_checkbox.signal_clicked().connect(
			sigc::mem_fun(*this, &VisionGUI::event_draw_info_checkbox_signal_clicked));
}

bool VisionGUI::getIsDrawing() {
	return !draw_info_flag;
}

void VisionGUI::__create_frm_split_view() {
	Gtk::VBox *vbox;
	Gtk::Grid *grid;
	Gtk::Label *label;

	vbox = new Gtk::VBox();
	grid = new Gtk::Grid();

	pack_start(fr_splitView, false, false, 5);

	fr_splitView.add(*vbox);
	vbox->pack_start(*grid, false, true, 5);
	vbox->set_halign(Gtk::ALIGN_CENTER);
	vbox->set_valign(Gtk::ALIGN_CENTER);

	fr_splitView.set_label("Split View");

	grid->set_border_width(5);
	grid->set_column_spacing(10);
	grid->set_row_spacing(5);

	label = new Gtk::Label("Select Mode: ");
	grid->attach(*label, 0, 0, 1, 1);
	rb_original_view.set_label("Original");
	grid->attach(rb_original_view, 1, 0, 1, 1);
	rb_split_view.set_label("Split");
	rb_split_view.join_group(rb_original_view);
	grid->attach(rb_split_view, 2, 0, 1, 1);

	rb_original_view.set_state(Gtk::STATE_INSENSITIVE);
	rb_split_view.set_state(Gtk::STATE_INSENSITIVE);

	rb_original_view.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_rb_split_mode_clicked));
	rb_split_view.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_rb_split_mode_clicked));
}

void VisionGUI::__event_rb_split_mode_clicked() {
	isSplitView = rb_split_view.get_active();
}

bool VisionGUI::getIsSplitView() {
	if (disableSplitView) return false;
	else return isSplitView;
}

void VisionGUI::__create_frm_capture() {
	Gtk::VBox *vbox;
	Gtk::Grid *grid;
	Gtk::Label *label;
	Gtk::Frame *frame;

	vbox = new Gtk::VBox();
	grid = new Gtk::Grid();
	frame = new Gtk::Frame();

	pack_start(*frame, false, false, 5);

	frame->add(*vbox);
	vbox->pack_start(*grid, false, true, 5);
	vbox->set_halign(Gtk::ALIGN_CENTER);
	vbox->set_valign(Gtk::ALIGN_CENTER);

	frame->set_label("Video/Image Capture");

	grid->set_border_width(5);
	grid->set_column_spacing(10);
	grid->set_row_spacing(5);

	label = new Gtk::Label("Picture Name:");
	label->set_alignment(1.0);
	grid->attach(*label, 0, 0, 1, 1);
	label = new Gtk::Label("Video Name:");
	label->set_alignment(1.0);
	grid->attach(*label, 0, 1, 1, 1);

	grid->attach(en_picture_name, 1, 0, 1, 1);
	grid->attach(en_video_name, 1, 1, 1, 1);

	bt_save_picture.set_label("Save");
	bt_record_video.set_label("REC");
	grid->attach(bt_save_picture, 2, 0, 1, 1);
	grid->attach(bt_record_video, 2, 1, 1, 1);

	en_video_name.set_state(Gtk::STATE_INSENSITIVE);
	en_picture_name.set_state(Gtk::STATE_INSENSITIVE);
	bt_record_video.set_state(Gtk::STATE_INSENSITIVE);
	bt_save_picture.set_state(Gtk::STATE_INSENSITIVE);

	bt_record_video.signal_pressed().connect(sigc::mem_fun(*this, &VisionGUI::bt_record_video_pressed));
	bt_save_picture.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::bt_save_picture_clicked));
}

void VisionGUI::bt_record_video_pressed() {
	if (recorder.is_recording()) {
		recorder.finish_video();
		bt_record_video.set_label("REC");
		en_video_name.set_text("");
		en_video_name.set_state(Gtk::STATE_NORMAL);
	} else {
		std::string name = en_video_name.get_text();

		bt_record_video.set_label("Stop");
		en_video_name.set_state(Gtk::STATE_INSENSITIVE);
		if (name.empty()) {
			std::string date = generate_date_str();
			en_video_name.set_text(date);
			recorder.start_new_video(date);
		} else {
			recorder.start_new_video(name);
		}
	}
}

void VisionGUI::bt_save_picture_clicked() {
	std::string name = en_picture_name.get_text();
	if (name.empty()) {
		std::string date = generate_date_str();
		recorder.save_picture(date);
	} else {
		recorder.save_picture(name);
	}
	en_picture_name.set_text("");
}

void VisionGUI::__create_frm_cielab() {
	Gtk::VBox *vbox;
	Gtk::Grid *grid;
	Gtk::Label *label;

	vbox = new Gtk::VBox();
	grid = new Gtk::Grid();

	pack_start(fr_CIELAB, false, false, 5);

	fr_CIELAB.add(*vbox);
	vbox->pack_start(*grid, false, true, 5);
	vbox->set_halign(Gtk::ALIGN_CENTER);
	vbox->set_valign(Gtk::ALIGN_CENTER);

	fr_CIELAB.set_label("CIELAB Calibration");

	grid->set_border_width(5);
	grid->set_column_spacing(15);
	grid->set_row_spacing(0);
	// grid->set_column_homogeneous(true);

	bt_LAB_calib.set_label("CIELAB Calib.");
	grid->attach(bt_LAB_calib, 0, 0, 1, 1);

	bt_CIELAB_left.set_label(" < ");
	grid->attach(bt_CIELAB_left, 2, 0, 1, 1);
	CIELAB_label.set_text("Main");
	grid->attach(CIELAB_label, 3, 0, 1, 1);
	bt_CIELAB_right.set_label(" > ");
	grid->attach(bt_CIELAB_right, 4, 0, 1, 1);

	bt_switchMainAdv.set_label("Main <-> Adv");
	grid->attach(bt_switchMainAdv, 6, 0, 1, 1);

	grid = new Gtk::Grid();
	grid->set_border_width(5);
	grid->set_column_spacing(15);
	grid->set_row_spacing(0);
	grid->set_column_homogeneous(true);
	vbox->pack_start(*grid, false, true, 5);

	lb_cieL_min.set_text("Black");
	lb_cieL_min.set_alignment(1.0, 1.0);
	grid->attach(lb_cieL_min, 0, 0, 1, 1);

	HScale_cieL_min.set_digits(0);
	HScale_cieL_min.set_increments(1, 1);
	HScale_cieL_min.set_range(0, 255);
	HScale_cieL_min.set_value_pos(Gtk::POS_TOP);
	HScale_cieL_min.set_draw_value();
	grid->attach(HScale_cieL_min, 1, 0, 2, 1);

	lb_cieL_max.set_text("White");
	lb_cieL_max.set_alignment(1.0, 1.0);
	grid->attach(lb_cieL_max, 3, 0, 1, 1);

	HScale_cieL_max.set_digits(0);
	HScale_cieL_max.set_increments(1, 1);
	HScale_cieL_max.set_range(0, 255);
	HScale_cieL_max.set_value_pos(Gtk::POS_TOP);
	HScale_cieL_max.set_draw_value();
	grid->attach(HScale_cieL_max, 4, 0, 2, 1);

	lb_cieA_min.set_text("Green");
	lb_cieA_min.set_alignment(1.0, 1.0);
	grid->attach(lb_cieA_min, 0, 1, 1, 1);

	HScale_cieA_min.set_digits(0);
	HScale_cieA_min.set_increments(1, 1);
	HScale_cieA_min.set_range(0, 255);
	HScale_cieA_min.set_value_pos(Gtk::POS_TOP);
	HScale_cieA_min.set_draw_value();
	grid->attach(HScale_cieA_min, 1, 1, 2, 1);

	lb_cieA_max.set_text("Red");
	lb_cieA_max.set_alignment(1.0, 1.0);
	grid->attach(lb_cieA_max, 3, 1, 1, 1);

	HScale_cieA_max.set_digits(0);
	HScale_cieA_max.set_increments(1, 1);
	HScale_cieA_max.set_range(0, 255);
	HScale_cieA_max.set_value_pos(Gtk::POS_TOP);
	HScale_cieA_max.set_draw_value();

	grid->attach(HScale_cieA_max, 4, 1, 2, 1);

	lb_cieB_min.set_text("Blue");
	lb_cieB_min.set_alignment(1.0, 1.0);
	grid->attach(lb_cieB_min, 0, 2, 1, 1);

	HScale_cieB_min.set_digits(0);
	HScale_cieB_min.set_increments(1, 1);
	HScale_cieB_min.set_range(0, 255);
	HScale_cieB_min.set_value_pos(Gtk::POS_TOP);
	HScale_cieB_min.set_draw_value();
	grid->attach(HScale_cieB_min, 1, 2, 2, 1);

	lb_cieB_max.set_text("Yellow");
	lb_cieB_max.set_alignment(1.0, 1.0);
	grid->attach(lb_cieB_max, 3, 2, 1, 1);

	HScale_cieB_max.set_digits(0);
	HScale_cieB_max.set_increments(1, 1);
	HScale_cieB_max.set_range(0, 255);
	HScale_cieB_max.set_value_pos(Gtk::POS_TOP);
	HScale_cieB_max.set_draw_value();

	grid->attach(HScale_cieB_max, 4, 2, 2, 1);

	label = new Gtk::Label("Erode:");
	label->set_alignment(1.0, 1.0);
	grid->attach(*label, 0, 3, 1, 1);

	HScale_Erode.set_digits(0);
	HScale_Erode.set_increments(1, 1);
	HScale_Erode.set_range(0, 10);
	HScale_Erode.set_value_pos(Gtk::POS_TOP);
	HScale_Erode.set_draw_value();
	grid->attach(HScale_Erode, 1, 3, 2, 1);

	label = new Gtk::Label("Dilate:");
	label->set_alignment(1.0, 1.0);
	grid->attach(*label, 3, 3, 1, 1);

	HScale_Dilate.set_digits(0);
	HScale_Dilate.set_increments(1, 1);
	HScale_Dilate.set_range(0, 10);
	HScale_Dilate.set_value_pos(Gtk::POS_TOP);
	HScale_Dilate.set_draw_value();
	grid->attach(HScale_Dilate, 4, 3, 2, 1);

	label = new Gtk::Label("Blur:");
	label->set_alignment(1.0, 1.0);
	grid->attach(*label, 0, 4, 1, 1);

	HScale_Blur.set_digits(0);
	HScale_Blur.set_increments(1, 1);
	HScale_Blur.set_range(0, 9);
	HScale_Blur.set_value_pos(Gtk::POS_TOP);
	HScale_Blur.set_draw_value();
	grid->attach(HScale_Blur, 1, 4, 2, 1);

	label = new Gtk::Label("Amin:");
	label->set_alignment(1.0, 1.0);
	grid->attach(*label, 3, 4, 1, 1);

	HScale_Amin.set_digits(0);
	HScale_Amin.set_increments(1, 1);
	HScale_Amin.set_range(0, 200);
	HScale_Amin.set_value_pos(Gtk::POS_TOP);
	HScale_Amin.set_draw_value();

	grid->attach(HScale_Amin, 4, 4, 2, 1);

	bt_LAB_calib.set_state(Gtk::STATE_INSENSITIVE);
	bt_switchMainAdv.set_state(Gtk::STATE_INSENSITIVE);
	HScale_cieL_min.set_state(Gtk::STATE_INSENSITIVE);
	HScale_cieA_min.set_state(Gtk::STATE_INSENSITIVE);
	HScale_cieB_min.set_state(Gtk::STATE_INSENSITIVE);
	HScale_cieL_max.set_state(Gtk::STATE_INSENSITIVE);
	HScale_cieA_max.set_state(Gtk::STATE_INSENSITIVE);
	HScale_cieB_max.set_state(Gtk::STATE_INSENSITIVE);
	HScale_Dilate.set_state(Gtk::STATE_INSENSITIVE);
	HScale_Erode.set_state(Gtk::STATE_INSENSITIVE);
	HScale_Blur.set_state(Gtk::STATE_INSENSITIVE);
	HScale_Amin.set_state(Gtk::STATE_INSENSITIVE);
	bt_CIELAB_left.set_state(Gtk::STATE_INSENSITIVE);
	bt_CIELAB_right.set_state(Gtk::STATE_INSENSITIVE);

	bt_LAB_calib.signal_pressed().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_CIELAB_calib_pressed));
	bt_CIELAB_right.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_right_CIELAB_clicked));
	bt_CIELAB_left.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_left_CIELAB_clicked));
	bt_switchMainAdv.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_switchMainAdv_clicked));
	HScale_cieL_min.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_cieL_min_value_changed));
	HScale_cieL_max.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_cieL_max_value_changed));
	HScale_cieA_min.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_cieA_min_value_changed));
	HScale_cieA_max.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_cieA_max_value_changed));
	HScale_cieB_min.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_cieB_min_value_changed));
	HScale_cieB_max.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_cieB_max_value_changed));
	HScale_Erode.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Erode_value_changed));
	HScale_Dilate.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Dilate_value_changed));
	HScale_Blur.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Blur_value_changed));
	HScale_Amin.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Amin_value_changed));
}

void VisionGUI::__event_bt_switchMainAdv_clicked() {
	vision->switchMainWithAdv();
	HScale_cieL_min.set_value(vision->getCIE_L(Img_id, 0));
	HScale_cieL_max.set_value(vision->getCIE_L(Img_id, 1));
	HScale_cieA_min.set_value(vision->getCIE_A(Img_id, 0));
	HScale_cieA_max.set_value(vision->getCIE_A(Img_id, 1));
	HScale_cieB_min.set_value(vision->getCIE_B(Img_id, 0));
	HScale_cieB_max.set_value(vision->getCIE_B(Img_id, 1));
	HScale_Amin.set_value(vision->getAmin(Img_id));
	HScale_Blur.set_value(vision->getBlur(Img_id));
	HScale_Erode.set_value(vision->getErode(Img_id));
	HScale_Dilate.set_value(vision->getDilate(Img_id));
}

void VisionGUI::update_vision_hscale_values() {
	HScale_Erode.set_value(vision->getErode(Img_id));
	HScale_Dilate.set_value(vision->getDilate(Img_id));
	HScale_Blur.set_value(vision->getBlur(Img_id));
	HScale_Amin.set_value(vision->getAmin(Img_id));

	HScale_cieL_min.set_value(vision->getCIE_L(Img_id, 0));
	HScale_cieL_max.set_value(vision->getCIE_L(Img_id, 1));
	HScale_cieA_min.set_value(vision->getCIE_A(Img_id, 0));
	HScale_cieA_max.set_value(vision->getCIE_A(Img_id, 1));
	HScale_cieB_min.set_value(vision->getCIE_B(Img_id, 0));
	HScale_cieB_max.set_value(vision->getCIE_B(Img_id, 1));
}

void VisionGUI::HScale_cieL_min_value_changed() {
	vision->setCIE_L(Img_id, 0, static_cast<int>(HScale_cieL_min.get_value()));
}

void VisionGUI::HScale_cieA_min_value_changed() {
	vision->setCIE_A(Img_id, 0, static_cast<int>(HScale_cieA_min.get_value()));
}

void VisionGUI::HScale_cieB_min_value_changed() {
	vision->setCIE_B(Img_id, 0, static_cast<int>(HScale_cieB_min.get_value()));
}

void VisionGUI::HScale_cieL_max_value_changed() {
	vision->setCIE_L(Img_id, 1, static_cast<int>(HScale_cieL_max.get_value()));
}

void VisionGUI::HScale_cieA_max_value_changed() {
	vision->setCIE_A(Img_id, 1, static_cast<int>(HScale_cieA_max.get_value()));
}

void VisionGUI::HScale_cieB_max_value_changed() {
	vision->setCIE_B(Img_id, 1, static_cast<int>(HScale_cieB_max.get_value()));
}

void VisionGUI::HScale_Amin_value_changed() {
	vision->setAmin(Img_id, static_cast<int>(HScale_Amin.get_value()));
}

void VisionGUI::HScale_Dilate_value_changed() {

	if (HScale_Dilate.get_value() < 0) {
		vision->setDilate(Img_id, 0);
	} else {
		vision->setDilate(Img_id, static_cast<int>(HScale_Dilate.get_value()));
	}
}

void VisionGUI::HScale_Erode_value_changed() {

	if (HScale_Erode.get_value() < 0) {
		vision->setErode(Img_id, 0);
	} else {
		vision->setErode(Img_id, static_cast<int>(HScale_Erode.get_value()));
	}
}

void VisionGUI::HScale_Blur_value_changed() {
	vision->setBlur(Img_id, static_cast<int>(HScale_Blur.get_value()));
}

void VisionGUI::__event_bt_CIELAB_calib_pressed() {

	if (CIELAB_calib_event_flag) {
		CIELAB_calib_event_flag = false;
		// VisionGUI::__event_auto_save();
		HScale_cieL_min.set_state(Gtk::STATE_INSENSITIVE);
		HScale_cieA_min.set_state(Gtk::STATE_INSENSITIVE);
		HScale_cieB_min.set_state(Gtk::STATE_INSENSITIVE);
		HScale_cieL_max.set_state(Gtk::STATE_INSENSITIVE);
		HScale_cieA_max.set_state(Gtk::STATE_INSENSITIVE);
		HScale_cieB_max.set_state(Gtk::STATE_INSENSITIVE);
		HScale_Dilate.set_state(Gtk::STATE_INSENSITIVE);
		HScale_Erode.set_state(Gtk::STATE_INSENSITIVE);
		HScale_Blur.set_state(Gtk::STATE_INSENSITIVE);
		HScale_Amin.set_state(Gtk::STATE_INSENSITIVE);
		bt_CIELAB_right.set_state(Gtk::STATE_INSENSITIVE);
		bt_CIELAB_left.set_state(Gtk::STATE_INSENSITIVE);
		bt_switchMainAdv.set_state(Gtk::STATE_INSENSITIVE);
	} else {
		CIELAB_calib_event_flag = true;
		HScale_cieL_min.set_state(Gtk::STATE_NORMAL);
		HScale_cieA_min.set_state(Gtk::STATE_NORMAL);
		HScale_cieB_min.set_state(Gtk::STATE_NORMAL);
		HScale_cieL_max.set_state(Gtk::STATE_NORMAL);
		HScale_cieA_max.set_state(Gtk::STATE_NORMAL);
		HScale_cieB_max.set_state(Gtk::STATE_NORMAL);
		HScale_Dilate.set_state(Gtk::STATE_NORMAL);
		HScale_Erode.set_state(Gtk::STATE_NORMAL);
		HScale_Blur.set_state(Gtk::STATE_NORMAL);
		HScale_Amin.set_state(Gtk::STATE_NORMAL);
		bt_CIELAB_right.set_state(Gtk::STATE_NORMAL);
		bt_CIELAB_left.set_state(Gtk::STATE_NORMAL);
		bt_switchMainAdv.set_state(Gtk::STATE_NORMAL);
	}
}

void VisionGUI::selectFrame(int sector) {
	switch (sector) {
		case 0:
			rb_original_view.set_active(true);
			__event_rb_split_mode_clicked();
			break;

		case 1:
			Img_id = 3;
			rb_original_view.set_active(true);
			__event_rb_split_mode_clicked();
			__event_bt_right_CIELAB_clicked();
			if (!bt_LAB_calib.get_active()) {
				bt_LAB_calib.set_active(true);
				__event_bt_CIELAB_calib_pressed();
			}
			break;
		case 2:
			Img_id = 0;
			rb_original_view.set_active(true);
			__event_rb_split_mode_clicked();
			__event_bt_right_CIELAB_clicked();
			if (!bt_LAB_calib.get_active()) {
				bt_LAB_calib.set_active(true);
				__event_bt_CIELAB_calib_pressed();
			}
			break;
		case 3:
			Img_id = 1;
			rb_original_view.set_active(true);
			__event_rb_split_mode_clicked();
			__event_bt_right_CIELAB_clicked();
			if (!bt_LAB_calib.get_active()) {
				bt_LAB_calib.set_active(true);
				__event_bt_CIELAB_calib_pressed();
			}
			break;
		default:
			break;
	}
}

void VisionGUI::__event_bt_right_CIELAB_clicked() {

	if (Img_id == 3)
		Img_id = 0;
	else
		Img_id++;

	HScale_cieL_min.set_value(vision->getCIE_L(Img_id, 0));
	HScale_cieL_max.set_value(vision->getCIE_L(Img_id, 1));

	HScale_cieA_min.set_value(vision->getCIE_A(Img_id, 0));
	HScale_cieA_max.set_value(vision->getCIE_A(Img_id, 1));

	HScale_cieB_min.set_value(vision->getCIE_B(Img_id, 0));
	HScale_cieB_max.set_value(vision->getCIE_B(Img_id, 1));


	HScale_Dilate.set_value(vision->getDilate(Img_id));
	HScale_Erode.set_value(vision->getErode(Img_id));

	HScale_Blur.set_value(vision->getBlur(Img_id));
	HScale_Amin.set_value(vision->getAmin(Img_id));

	switch (Img_id) {
		case 0:
			CIELAB_label.set_text("Main");
			break;
		case 1:
			CIELAB_label.set_text("Green");
			break;
		case 2:
			CIELAB_label.set_text("Ball");
			break;
		case 3:
			CIELAB_label.set_text("Opp.");
			break;
		default:break;
	}
}

void VisionGUI::__event_bt_left_CIELAB_clicked() {

	if (Img_id == 0)
		Img_id = 3;
	else
		Img_id--;

	HScale_cieL_min.set_value(vision->getCIE_L(Img_id, 0));
	HScale_cieL_max.set_value(vision->getCIE_L(Img_id, 1));

	HScale_cieA_min.set_value(vision->getCIE_A(Img_id, 0));
	HScale_cieA_max.set_value(vision->getCIE_A(Img_id, 1));

	HScale_cieB_min.set_value(vision->getCIE_B(Img_id, 0));
	HScale_cieB_max.set_value(vision->getCIE_B(Img_id, 1));


	HScale_Dilate.set_value(vision->getDilate(Img_id));
	HScale_Erode.set_value(vision->getErode(Img_id));

	HScale_Blur.set_value(vision->getBlur(Img_id));
	HScale_Amin.set_value(vision->getAmin(Img_id));

	switch (Img_id) {
		case 0:
			CIELAB_label.set_text("Main");
			break;
		case 1:
			CIELAB_label.set_text("Green");
			break;
		case 2:
			CIELAB_label.set_text("Ball");
			break;
		case 3:
			CIELAB_label.set_text("Opp.");
			break;
		default:break;
	}
}

void VisionGUI::event_draw_info_checkbox_signal_clicked() {
	draw_info_flag = !draw_info_flag;
}

void VisionGUI::setFrameSize(int inWidth, int inHeight) {
	vision->setFrameSize(inWidth, inHeight);
}

VisionGUI::VisionGUI() :
		vision(std::make_unique<Vision>(640, 480)),
		recorder(640, 480),
		Img_id(0),
		CIELAB_calib_event_flag(false),
		draw_info_flag(false),
		isSplitView(false),
		disableSplitView(false) {

	__create_frm_capture();
	__create_frm_drawing_options();
	__create_frm_split_view();
	__create_frm_cielab();
}