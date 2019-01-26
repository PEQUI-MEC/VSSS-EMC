/*
* v4lgui.cpp
*
*  Created on: Feb 1, 2014
*      Author: gustavo
*/

#include "V4LInterface.hpp"
#include "RobotGUI.hpp"

#define DEFAULT_STR " - "

using capture::V4LInterface;

bool V4LInterface::get_start_game_flag() {
	return start_game_flag;
}

void V4LInterface::__init_combo_boxes() {

	ls_input = Gtk::ListStore::create(model_input);
	cb_input.set_model(ls_input);
	cb_input.pack_start(model_input.m_col_name);

	ls_format_desc = Gtk::ListStore::create(model_format_desc);
	cb_format_desc.set_model(ls_format_desc);
	cb_format_desc.pack_start(model_format_desc.m_col_name);

	ls_standard = Gtk::ListStore::create(model_standard);
	cb_standard.set_model(ls_standard);
	cb_standard.pack_start(model_standard.m_col_name);

	ls_frame_size = Gtk::ListStore::create(model_frame_size);
	cb_frame_size.set_model(ls_frame_size);
	cb_frame_size.pack_start(model_frame_size.m_col_name);

	ls_frame_interval = Gtk::ListStore::create(model_frame_interval);
	cb_frame_interval.set_model(ls_frame_interval);
	cb_frame_interval.pack_start(model_frame_interval.m_col_name);
}

void V4LInterface::createQuickActionsFrame() {
	auto *box = new Gtk::Box;
	auto *quick_sl_box = new Gtk::VBox;
	auto *sl_box = new Gtk::VBox;

	if (isLowRes)
		box->set_orientation(Gtk::ORIENTATION_HORIZONTAL);
	else
		box->set_orientation(Gtk::ORIENTATION_VERTICAL);


	info_hbox.pack_start(frm_quick_actions, false, true, 5);

	frm_quick_actions.add(*box);
	box->pack_start(*quick_sl_box, false, true, 5);
	box->pack_start(*sl_box, false, true, 5);
	frm_quick_actions.set_label("Save/Load");

	box->set_halign(Gtk::ALIGN_CENTER);
	box->set_valign(Gtk::ALIGN_CENTER);
	box->set_margin_top(10);
	box->set_margin_bottom(10);
	box->set_margin_left(10);
	box->set_margin_right(10);
	box->set_homogeneous(true);

	bt_quick_save.set_label("Quick Save");
	quick_sl_box->pack_start(bt_quick_save, false, true, 5);
	bt_quick_load.set_label("Quick Load");
	quick_sl_box->pack_start(bt_quick_load, false, true, 5);

	bt_save.set_label("Save");
	sl_box->pack_start(bt_save, false, true, 5);
	bt_load.set_label("Load");
	sl_box->pack_start(bt_load, false, true, 5);
}

void V4LInterface::__create_frm_device_info() {
	Gtk::HBox *hbox;
	Gtk::VBox *vbox;
	Gtk::Label *label;
	Gtk::Table *table;

	frm_device_info.set_label("Device Information");

	vbox = new Gtk::VBox();
	frm_device_info.add(*vbox);

	hbox = new Gtk::HBox();
	vbox->pack_start(*hbox, false, true, 0);

	label = new Gtk::Label("Device: ");
	label->set_alignment(1.0, .5);
	hbox->pack_start(*label, false, true, 0);

	hbox->pack_start(cb_device, false, true, 0);

	bt_start.set_label("start");
	hbox->pack_start(bt_start, false, true, 0);

	hbox = new Gtk::HBox();
	vbox->pack_start(*hbox, false, true, 0);

	table = new Gtk::Table(2, 4, false);

	lb_device_name.set_text(DEFAULT_STR);
	lb_device_driver.set_text(DEFAULT_STR);
	lb_device_card.set_text(DEFAULT_STR);
	lb_device_bus.set_text(DEFAULT_STR);

	label = new Gtk::Label("Device: ");
	label->set_alignment(1.0, .5);
	table->attach(*label, 0, 1, 0, 1);
	lb_device_name.set_alignment(0, .5);
	table->attach(lb_device_name, 1, 2, 0, 1);

	label = new Gtk::Label("Card: ");
	label->set_alignment(1.0, .5);
	table->attach(*label, 0, 1, 1, 2);
	lb_device_card.set_alignment(0, .5);
	table->attach(lb_device_card, 1, 2, 1, 2);

	label = new Gtk::Label("Driver: ");
	label->set_alignment(1.0, .5);
	table->attach(*label, 2, 3, 0, 1);
	lb_device_driver.set_alignment(0, .5);
	table->attach(lb_device_driver, 3, 4, 0, 1);

	label = new Gtk::Label("Bus: ");
	label->set_alignment(1.0, .5);
	table->attach(*label, 2, 3, 1, 2);
	lb_device_bus.set_alignment(0, .5);
	table->attach(lb_device_bus, 3, 4, 1, 2);

	hbox->pack_start(*table, true, true, 0);
}

void V4LInterface::__create_frm_device_properties() {

	Gtk::Label *label;
	Gtk::Table *table;
	Gtk::HBox *hbox;
	Gtk::VBox *vbox;

	// Adiciona a vbox (principal) no frame
	vbox = new Gtk::VBox();


	// Primeira Hbox com oos botões Warp, Reset, Adjust
	hbox = new Gtk::HBox();
	hbox->set_border_width(5);
	hbox->set_halign(Gtk::ALIGN_CENTER);

	vbox->pack_start(*hbox, false, true, 0);
	frm_device_prop.add(*vbox);
	vbox->pack_start(notebook, false, true, 5);

	table = new Gtk::Table(4, 4, false);

	label = new Gtk::Label("Input: ");
	label->set_alignment(1, .5);
	table->attach(*label, 0, 1, 0, 1, Gtk::FILL, Gtk::EXPAND, 0, 0);
	table->attach(cb_input, 1, 2, 0, 1, Gtk::FILL, Gtk::EXPAND, 0, 0);
	cb_input.set_border_width(0);
	cb_input.set_size_request(100, -1);

	label = new Gtk::Label("Standard: ");
	label->set_alignment(1, .5);
	table->attach(*label, 3, 4, 0, 1, Gtk::FILL, Gtk::EXPAND, 0, 0);
	table->attach(cb_standard, 5, 6, 0, 1, Gtk::FILL, Gtk::EXPAND, 0, 0);
	cb_standard.set_size_request(100, -1);

	label = new Gtk::Label("Format: ");
	label->set_alignment(1, .5);
	table->attach(*label, 0, 1, 1, 2, Gtk::FILL, Gtk::EXPAND, 0, 0);
	table->attach(cb_format_desc, 1, 2, 1, 2, Gtk::FILL, Gtk::EXPAND, 0, 0);

	label = new Gtk::Label("Frame size: ");
	label->set_alignment(1, .5);
	table->attach(*label, 3, 4, 1, 2, Gtk::FILL, Gtk::EXPAND, 0, 0);
	table->attach(cb_frame_size, 5, 6, 1, 2, Gtk::FILL, Gtk::EXPAND, 0, 0);

	label = new Gtk::Label("Intervals: ");
	label->set_alignment(1, .5);
	table->attach(*label, 0, 1, 3, 4, Gtk::FILL, Gtk::EXPAND, 0, 0);
	table->attach(cb_frame_interval, 1, 2, 3, 4, Gtk::FILL, Gtk::EXPAND, 0, 0);

	table->attach(sp_width, 3, 4, 3, 4, Gtk::FILL, Gtk::EXPAND, 0, 0);
	table->attach(sp_height, 5, 6, 3, 4, Gtk::FILL, Gtk::EXPAND, 0, 0);

	notebook.append_page(*table, "Properties");

	frm_device_prop.set_label("Device Prop");
}

void V4LInterface::__create_frm_warp() {
	warp_label[0].set_label("Fit field on image:");
	warp_label[1].set_label("Include goals:");
	warp_label[2].set_label("Remove corners:");
	warp_label[3].set_label("Switch field sides:");
	warp_label[4].set_label("Left");
	warp_label[5].set_label("Right");

	for (auto &label : warp_label)
		label.set_halign(Gtk::ALIGN_END);

	bt_warp_start.set_label("Start");
	bt_warp_apply.set_label("Apply");
	bt_adjust_start.set_label("Start");
	bt_adjust_apply.set_label("Apply");
	bt_reset_warp.set_label("Reset to original image");
	bt_invert_field.set_label("Switch field sides (invert image)");
	bt_warp_undo.set_label("Undo");
	bt_adjust_undo.set_label("Undo");

	frm_warp.add(warp_grid);
	frm_warp.set_label("Image Warp");

	warp_grid.set_halign(Gtk::ALIGN_CENTER);
	warp_grid.set_border_width(12);
	warp_grid.set_column_spacing(3);
	warp_grid.set_row_spacing(6);

	warp_grid.attach(warp_label[0], 0, 0, 1, 1);
	warp_grid.attach(bt_warp_start, 1, 0, 1, 1);
	warp_grid.attach(bt_warp_apply, 2, 0, 1, 1);
	warp_grid.attach(bt_warp_undo, 4, 0, 1, 1);

	warp_grid.attach(warp_label[1], 0, 1, 1, 1);
	warp_grid.attach(warp_label[4], 1, 1, 1, 1);
	warp_grid.attach(HScale_offsetL, 2, 1, 1, 1);
	warp_grid.attach(warp_label[5], 3, 1, 1, 1);
	warp_grid.attach(HScale_offsetR, 4, 1, 1, 1);

	warp_grid.attach(warp_label[2], 0, 2, 1, 1);
	warp_grid.attach(bt_adjust_start, 1, 2, 1, 1);
	warp_grid.attach(bt_adjust_apply, 2, 2, 1, 1);
	warp_grid.attach(bt_adjust_undo, 4, 2, 1, 1);

	//warp_grid.attach(warp_label[3], 0, 3, 1, 1);
	warp_grid.attach(bt_invert_field, 1, 3, 4, 1);

	warp_grid.attach(bt_reset_warp, 1, 4, 4, 1);

	// Configurações da barra do offset right
	HScale_offsetR.set_digits(0);
	HScale_offsetR.set_increments(1, 1);
	HScale_offsetR.set_range(0, 100);
	HScale_offsetR.set_value_pos(Gtk::POS_RIGHT);
	HScale_offsetR.set_draw_value();
	HScale_offsetR.set_size_request(80, -1);


	// Configurações da barra do offset left
	HScale_offsetL.set_digits(0);
	HScale_offsetL.set_increments(1, 1);
	HScale_offsetL.set_range(0, 100);
	HScale_offsetL.set_value_pos(Gtk::POS_RIGHT);
	HScale_offsetL.set_draw_value();
	HScale_offsetL.set_size_request(80, -1);

	// Inactivate buttons
	bt_warp_start.set_state(Gtk::STATE_INSENSITIVE);
	bt_warp_apply.set_state(Gtk::STATE_INSENSITIVE);
	bt_adjust_start.set_state(Gtk::STATE_INSENSITIVE);
	bt_adjust_apply.set_state(Gtk::STATE_INSENSITIVE);
	bt_reset_warp.set_state(Gtk::STATE_INSENSITIVE);
	bt_invert_field.set_state(Gtk::STATE_INSENSITIVE);
	bt_warp_undo.set_state(Gtk::STATE_INSENSITIVE);
	bt_adjust_undo.set_state(Gtk::STATE_INSENSITIVE);
	HScale_offsetR.set_state(Gtk::STATE_INSENSITIVE);
	HScale_offsetL.set_state(Gtk::STATE_INSENSITIVE);

	// Signals
	bt_warp_start.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_bt_warp_start_clicked));
	bt_warp_apply.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_bt_warp_apply_clicked));
	bt_adjust_start.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_bt_adjust_start_clicked));
	bt_adjust_apply.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_bt_adjust_apply_clicked));
	bt_reset_warp.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_bt_reset_warp_clicked));
	bt_invert_field.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_bt_invert_field_clicked));
	bt_warp_undo.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_bt_warp_undo_clicked));
	bt_adjust_undo.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_bt_adjust_undo_clicked));
	HScale_offsetL.signal_value_changed().connect(
			sigc::mem_fun(*this, &V4LInterface::HScale_offsetL_value_changed));
	HScale_offsetR.signal_value_changed().connect(
			sigc::mem_fun(*this, &V4LInterface::HScale_offsetR_value_changed));
}

void V4LInterface::__update_cb_device() {

	char device[16];

	if (capture::v4lcap::enum_device_name(device, 20, true)) {
		do {
			cb_device.append(device);
		} while (capture::v4lcap::enum_device_name(device, 20));
	}
}

void V4LInterface::__update_cb_input() {

	cb_input_signal.block(true);
	ls_input->clear();

	struct v4l2_input input;
	unsigned int index;

	vcap.get_input(&input);
	index = input.index;

	if (vcap.enum_video_input(&input, 0, true)) {
		do {

			Gtk::TreeModel::Row row = *(ls_input->append());
			Glib::ustring name = (const char *) input.name;
			row[model_input.m_col_name] = name;
			row[model_input.m_col_data] = input;

			if (input.index == index) {
				cb_input.set_active(row);
			}
		} while (vcap.enum_video_input(&input));
	}

	cb_input_signal.block(false);
}

void V4LInterface::__update_cb_standard() {

	cb_standard_signal.block(true);

	ls_standard->clear();

	struct v4l2_standard standard{};

	vcap.get_standard(&standard);
	unsigned long index = standard.id;

	if (vcap.enum_video_standard(&standard, 0, true)) {
		do {
			Gtk::TreeModel::Row row = *(ls_standard->append());
			Glib::ustring name = (const char *) standard.name;
			row[model_standard.m_col_name] = name;
			row[model_standard.m_col_data] = standard;

			if (standard.id == index) {
				cb_standard.set_active(row);
			}
		} while (vcap.enum_video_standard(&standard));
	}

	cb_standard_signal.block(false);
}

void V4LInterface::__update_cb_format_desc() {

	cb_format_desc_signal.block(true);

	ls_format_desc->clear();

	struct v4l2_format fmt{};
	struct v4l2_fmtdesc fmt_desc{};
	vcap.get_format(&fmt, V4L2_BUF_TYPE_VIDEO_CAPTURE);

	unsigned int index = fmt.fmt.pix.pixelformat;

	if (vcap.enum_image_format(&fmt_desc, 0, true)) {
		do {
			Gtk::TreeModel::Row row = *(ls_format_desc->append());
			Glib::ustring name = (const char *) fmt_desc.description;

			if (fmt_desc.flags & V4L2_FMT_FLAG_EMULATED) name = name + "(Emulated)";

			row[model_format_desc.m_col_name] = name;
			row[model_format_desc.m_col_data] = fmt_desc;

			if (fmt_desc.pixelformat == index) {
				cb_format_desc.set_active(row);
			}
		} while (vcap.enum_image_format(&fmt_desc));
	}

	cb_format_desc_signal.block(false);
}

void V4LInterface::__update_cb_frame_size() {

	cb_frame_size_signal.block(true);

	ls_frame_size->clear();

	struct v4l2_format fmt{};
	vcap.get_format(&fmt, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	sp_width.set_value((double) fmt.fmt.pix.width);
	sp_height.set_value((double) fmt.fmt.pix.height);

	struct v4l2_frmsizeenum frmsize{};

	if (vcap.enum_frame_size(&frmsize, fmt.fmt.pix.pixelformat, 0, true)) {
		do {
			Gtk::TreeModel::Row row = *(ls_frame_size->append());
			Glib::ustring name = Glib::ustring::compose("%1x%2", frmsize.discrete.width, frmsize.discrete.height);
			row[model_frame_size.m_col_name] = name;
			row[model_frame_size.m_col_data] = frmsize;

			if (frmsize.discrete.width == fmt.fmt.pix.width && frmsize.discrete.height == fmt.fmt.pix.height) {
				cb_frame_size.set_active(row);
			}
		} while (vcap.enum_frame_size(&frmsize, fmt.fmt.pix.pixelformat));
	}
	cb_frame_size_signal.block(false);
}

void V4LInterface::__update_cb_frame_interval() {

	cb_frame_interval_signal.block(true);

	ls_frame_interval->clear();

	struct v4l2_format fmt{};
	unsigned int &width = fmt.fmt.pix.width;
	unsigned int &height = fmt.fmt.pix.height;

	vcap.get_format(&fmt, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	sp_width.set_value((double) width);
	sp_height.set_value((double) height);

	struct v4l2_frmivalenum frminterval{};

	struct v4l2_streamparm streamparm{};
	vcap.get_stream_parameter(&streamparm, V4L2_BUF_TYPE_VIDEO_CAPTURE);

	if (vcap.enum_frame_interval(&frminterval, fmt.fmt.pix.pixelformat, width, height, 0, true)) {
		do {
			Gtk::TreeModel::Row row = *(ls_frame_interval->append());
			float fps = (float) frminterval.discrete.denominator / (float) frminterval.discrete.numerator;
			Glib::ustring name = Glib::ustring::compose("%1/%2 = %3 fps", frminterval.discrete.numerator,
														frminterval.discrete.denominator, fps);
			row[model_frame_interval.m_col_name] = name;
			row[model_frame_interval.m_col_data] = frminterval;

			if (streamparm.parm.capture.timeperframe.numerator == frminterval.discrete.numerator
				&& streamparm.parm.capture.timeperframe.denominator == frminterval.discrete.denominator) {
				cb_frame_interval.set_active(row);
			}
		} while (vcap.enum_frame_interval(&frminterval, fmt.fmt.pix.pixelformat, width, height));
	}
	cb_frame_interval_signal.block(false);
}

void V4LInterface::__update_all() {
	if(init_frame){
		bool r = vcap.set_frame_size(640, 480,
									 V4L2_BUF_TYPE_VIDEO_CAPTURE);
		if (!r) std::cout << "Can't set frame size!" << std::endl;
		init_frame = false;
	}
	__update_cb_input();
	__update_cb_standard();
	__update_cb_format_desc();
	__update_cb_frame_size();
	__update_cb_frame_interval();
}

void V4LInterface::__make_control_list_default() {

	ctrl_list_default.clear();
	struct v4l2_queryctrl qctrl{};
	struct v4l2_querymenu qmenu{};

	if (vcap.enum_control_default(&qctrl, true)) {
		do {

			Gtk::Widget *wctrl = 0;
			ControlHolder hold;
			Glib::RefPtr<Gtk::ListStore> lstore;

			switch (qctrl.type) {
				case V4L2_CTRL_TYPE_INTEGER:
				case V4L2_CTRL_TYPE_INTEGER64:

					wctrl = new Gtk::HScale();
					dynamic_cast<Gtk::HScale *>(wctrl)->set_value_pos(Gtk::POS_RIGHT);
					dynamic_cast<Gtk::HScale *>(wctrl)->set_range(qctrl.minimum, qctrl.maximum);
					dynamic_cast<Gtk::HScale *>(wctrl)->set_increments(qctrl.step, 10 * qctrl.step);
					hold.con = dynamic_cast<Gtk::HScale *>(wctrl)->signal_change_value().connect(
							sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(
									sigc::mem_fun(*this, &V4LInterface::__set_control_hscale),
									&ctrl_list_default, wctrl));
					break;

				case V4L2_CTRL_TYPE_BOOLEAN:

					wctrl = new Gtk::CheckButton((const char *) qctrl.name);
					hold.con = dynamic_cast<Gtk::CheckButton *>(wctrl)->signal_clicked().connect(
							sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(
									sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_default,
									wctrl));
					break;

				case V4L2_CTRL_TYPE_BUTTON:

					wctrl = new Gtk::Button((const char *) qctrl.name);
					hold.con = dynamic_cast<Gtk::Button *>(wctrl)->signal_clicked().connect(
							sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(
									sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_default,
									wctrl));
					break;

				case V4L2_CTRL_TYPE_STRING:

					wctrl = new Gtk::Label((const char *) qctrl.name);
					break;

				case V4L2_CTRL_TYPE_MENU:
				case V4L2_CTRL_TYPE_INTEGER_MENU:

					wctrl = new Gtk::ComboBox();
					lstore = Gtk::ListStore::create(model_control_menu);
					dynamic_cast<Gtk::ComboBox *>(wctrl)->set_model(lstore);
					dynamic_cast<Gtk::ComboBox *>(wctrl)->pack_start(model_control_menu.m_col_name);

					if (vcap.enum_control_menu(&qmenu, qctrl, 0, true)) {
						do {
							Gtk::TreeModel::Row row = *(lstore->append());
							Glib::ustring name = (const char *) qmenu.name;
							row[model_control_menu.m_col_name] = name;
							row[model_control_menu.m_col_data] = qmenu;
						} while (vcap.enum_control_menu(&qmenu, qctrl));
					}

					hold.con = dynamic_cast<Gtk::ComboBox *>(wctrl)->signal_changed().connect(
							sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(
									sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_default,
									wctrl));
					break;

				case V4L2_CTRL_TYPE_CTRL_CLASS:
				case V4L2_CTRL_TYPE_BITMASK:
				default:
					wctrl = nullptr;
					break;
			}

			if (wctrl) {
				hold.qctrl = qctrl;
				hold.widget = wctrl;
				ctrl_list_default.push_back(hold);
			}
		} while (vcap.enum_control_default(&qctrl));
	}
}

void V4LInterface::__make_control_table(std::list<ControlHolder> &list, const char *title) {

	// create new controls =====================================================
	Gtk::Label *label = nullptr;
	Gtk::Table *table = nullptr;
	Gtk::Widget *wctrl = nullptr;

	guint ROWS = 4;
	guint COLS = 2;
	guint r, c;
	int tab_count = 0;
	int count = 0;

	struct v4l2_queryctrl qctrl{};
	std::list<ControlHolder>::iterator iter;

	for (iter = list.begin(); iter != list.end(); ++iter) {
		qctrl = (*iter).qctrl;
		wctrl = (*iter).widget;

		// create a new page of controls
		if (count % (ROWS * COLS) == 0) {
			r = 0;
			c = 0;
			count = 0;
			table = new Gtk::Table(ROWS, COLS * 2, false);
			notebook.append_page(*table, Glib::ustring::compose("%1 %2", title, tab_count++), true);
		}

		r = count / COLS;
		c = count % COLS;

		c = c * 2;

		wctrl->set_size_request(120, -1);

		if (qctrl.type == V4L2_CTRL_TYPE_BOOLEAN || qctrl.type == V4L2_CTRL_TYPE_BUTTON) {

			table->attach(*wctrl, c, c + 2, r, r + 1, Gtk::FILL, Gtk::FILL, 0, 0);
		} else {

			label = new Gtk::Label((const char *) qctrl.name);

			table->attach(*label, c, c + 1, r, r + 1, Gtk::FILL, Gtk::FILL, 0, 0);
			table->attach(*wctrl, c + 1, c + 2, r, r + 1, Gtk::FILL, Gtk::FILL, 0, 0);

			label->set_size_request(-1, -1);
			label->set_max_width_chars(10);
			label->set_alignment(1.0, 0.5);
		}

		count++;
	}

	notebook.show_all();
}

void V4LInterface::__block_control_signals(std::list<ControlHolder> &list, bool block) {

	std::list<ControlHolder>::iterator iter;

	for (iter = list.begin(); iter != list.end(); ++iter) {
		(*iter).con.block(block);
	}
}

void V4LInterface::__update_control_widgets(std::list<ControlHolder> &ctrl_list) {

	Gtk::Widget *wctrl = nullptr;
	struct v4l2_queryctrl qctrl{};
	struct v4l2_control control{};

	__block_control_signals(ctrl_list, true);

	std::list<ControlHolder>::iterator iter;

	for (iter = ctrl_list.begin(); iter != ctrl_list.end(); ++iter) {
		qctrl = (*iter).qctrl;
		wctrl = (*iter).widget;

		vcap.get_control(&control, qctrl.id);

		switch (qctrl.type) {
			case V4L2_CTRL_TYPE_INTEGER:
			case V4L2_CTRL_TYPE_INTEGER64:

				dynamic_cast<Gtk::HScale *>(wctrl)->set_value(control.value);
				break;

			case V4L2_CTRL_TYPE_BOOLEAN:

				dynamic_cast<Gtk::CheckButton *>(wctrl)->set_active(control.value == 1);
				break;

			case V4L2_CTRL_TYPE_BUTTON:

				break;

			case V4L2_CTRL_TYPE_STRING:

				break;

			case V4L2_CTRL_TYPE_MENU:
			case V4L2_CTRL_TYPE_INTEGER_MENU:

				Glib::RefPtr<Gtk::TreeModel> model = dynamic_cast<Gtk::ComboBox *>(wctrl)->get_model();

				Gtk::TreeModel::Children child = model->children();

				int i = 0;
				for (Gtk::TreeModel::Children::iterator treeIter = child.begin();
					 treeIter != child.end(); ++treeIter, ++i) {

					Gtk::TreeModel::Row row = *treeIter;
					struct v4l2_querymenu qm{};
					qm = row[model_control_menu.m_col_data];

					if (control.value < 0) return;

					if (control.value == (int) qm.index) {
						dynamic_cast<Gtk::ComboBox *>(wctrl)->set_active(treeIter);
						break;
					}
				}

				break;
		}
	}
	__block_control_signals(ctrl_list, false);
}

void V4LInterface::updateRobotLabels() {
	for (int i = 0; i < 3; i++) {
		Robot2 *robot = robots[i];
		auto pose = robot->get_pose();
		std::stringstream ss;
		ss << '(' << robot->tag + 1 << ", " << robot->ID << ") ("
		   << std::fixed << std::setprecision(2)
		   << pose.position.x << ", " << pose.position.y << ", "
		   << std::fixed << std::setprecision(1)
		   << pose.orientation * (180 / PI) << ")";
		robot_pos_lb_list[i].set_text(ss.str());
	}
}

void V4LInterface::update_ball_position(Geometry::Point ball) {
	std::stringstream ss;
	ss << "(" << std::fixed << std::setprecision(2) << ball.x << "," << ball.y << ")";
	ball_pos_lb.set_text(ss.str());
}

void V4LInterface::updateFPS(int fps) {
	std::stringstream aux;
	aux << "FPS: " << fps;
	fps_label->set_text(aux.str());
}

void V4LInterface::createPositionsAndButtonsFrame() {
	Gtk::Label *label;
	info_hbox.pack_start(robots_pos_buttons_vbox, false, true, 5);

	robots_pos_fm.set_label("Positions");
	robots_pos_buttons_vbox.pack_start(robots_pos_fm, false, true, 0);
	robots_pos_fm.add(robots_pos_vbox);
	robots_pos_vbox.set_size_request(250, -1);

	for (unsigned long i = 0; i < 3; i++) {
		label = new Gtk::Label(robots[i]->get_role_name() + ':');
		robot_pos_lb_list[i].set_text("-");
		robots_pos_hbox[i].pack_start(*label, false, true, 5);
		robots_pos_hbox[i].pack_start(robot_pos_lb_list[i], false, true, 0);
		robots_pos_vbox.pack_start(robots_pos_hbox[i], false, true, 5);
	}

	label = new Gtk::Label("Ball:");
	ball_pos_lb.set_text("-");
	robots_pos_hbox[3].pack_start(*label, false, true, 5);
	robots_pos_hbox[3].pack_start(ball_pos_lb, false, true, 0);
	robots_pos_vbox.pack_start(robots_pos_hbox[3], false, true, 5);

	robots_pos_buttons_vbox.pack_start(robots_buttons_fm, false, true, 5);
	robots_buttons_fm.add(fps_hbox);

	fps_hbox.set_margin_top(7);
	fps_hbox.set_margin_bottom(7);
	fps_hbox.set_halign(Gtk::ALIGN_CENTER);
	fps_label = new Gtk::Label("FPS: -");
	fps_hbox.pack_start(*fps_label, false, true, 5);
}

void V4LInterface::initInterface() {
	cb_device.set_state(Gtk::STATE_NORMAL);
	cb_input.set_state(Gtk::STATE_NORMAL);
	cb_standard.set_state(Gtk::STATE_NORMAL);
	cb_frame_size.set_state(Gtk::STATE_NORMAL);
	cb_format_desc.set_state(Gtk::STATE_NORMAL);
	sp_width.set_state(Gtk::STATE_NORMAL);
	sp_height.set_state(Gtk::STATE_NORMAL);
	cb_frame_interval.set_state(Gtk::STATE_NORMAL);
	bt_warp_start.set_state(Gtk::STATE_INSENSITIVE);
	bt_reset_warp.set_state(Gtk::STATE_INSENSITIVE);
	bt_quick_save.set_state(Gtk::STATE_INSENSITIVE);
	bt_quick_load.set_state(Gtk::STATE_INSENSITIVE);

	bt_save.set_state(Gtk::STATE_INSENSITIVE);
	bt_load.set_state(Gtk::STATE_INSENSITIVE);

	bt_adjust_start.set_state(Gtk::STATE_INSENSITIVE);

	m_signal_start.emit(false);

	HScale_offsetR.set_state(Gtk::STATE_INSENSITIVE);
	HScale_offsetL.set_state(Gtk::STATE_INSENSITIVE);

	notebook.set_scrollable(true);
	sp_width.set_range(0, 2000);
	sp_width.set_increments(1, 10);
	sp_height.set_range(0, 2000);
	sp_height.set_increments(1, 10);
	sp_width.set_tooltip_text("Width");
	sp_height.set_tooltip_text("Height");

	__init_combo_boxes();

	pack_start(capture_vbox);

	capture_vbox.pack_start(frm_device_info, false, false, 5);
	__create_frm_device_info();

	capture_vbox.pack_start(frm_device_prop, false, false, 10);
	__create_frm_device_properties();

	capture_vbox.pack_start(frm_warp, false, false, 10);
	__create_frm_warp();

	capture_vbox.pack_start(frm_cam_calib, false, false, 10);
	__create_frm_cam_calib();

	__update_cb_device();

	if (isLowRes) {
		red_button_pressed.set("img/pause_small.png");
		red_button_released.set("img/play_small.png");
	} else {
		red_button_pressed.set("img/pause.png");
		red_button_released.set("img/play.png");
	}

	createPositionsAndButtonsFrame();
	createQuickActionsFrame();
	if (!isLowRes)
		info_hbox.pack_start(robotGUI, false, true, 5);

	auto *vbox = new Gtk::VBox();
	vbox->set_halign(Gtk::ALIGN_CENTER);
	vbox->set_valign(Gtk::ALIGN_CENTER);

	record_video_checkbox.set_label("Recording");
	record_video_checkbox.set_can_focus(false);
	record_video_checkbox.set_margin_bottom(5);
	record_video_checkbox.set_active(true);

	vbox->pack_start(start_game_bt, false, true, 0);
	vbox->pack_start(record_video_checkbox, false, true, 0);

	info_hbox.pack_end(buttons_vbox, false, true, 5);
	buttons_vbox.pack_start(start_game_hbox, false, true, 5);
	start_game_hbox.pack_start(*vbox, false, true, 5);
	buttons_vbox.set_valign(Gtk::ALIGN_CENTER);
	start_game_bt.property_always_show_image();
	start_game_bt.set_size_request(50, 100);
	start_game_bt.set_image(red_button_released);

	record_video_checkbox.signal_clicked().connect(
			sigc::mem_fun(*this, &capture::V4LInterface::event_toggle_enable_video_record));
	start_game_bt.signal_clicked().connect(
			sigc::mem_fun(*this, &capture::V4LInterface::event_start_game_bt_signal_clicked));

	bt_quick_save.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_quick_save_clicked));
	bt_quick_load.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_quick_load_clicked));
	bt_save.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_save_clicked));
	bt_load.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_load_clicked));

	bt_start.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_start_clicked));

	cb_input_signal = cb_device.signal_changed().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_cb_device_changed));
	cb_input_signal = cb_input.signal_changed().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_cb_input_changed));
	cb_standard_signal = cb_standard.signal_changed().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_cb_standard_changed));
	cb_format_desc_signal = cb_format_desc.signal_changed().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_cb_format_desc_changed));
	cb_frame_size_signal = cb_frame_size.signal_changed().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_cb_frame_size_changed));
	cb_frame_interval_signal = cb_frame_interval.signal_changed().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_cb_frame_interval_changed));
}

void V4LInterface::__create_frm_cam_calib() {

	Gtk::Grid *grid;
	Gtk::Grid *grid2;
	Gtk::Label *label;
	Gtk::VBox *vbox;
	Gtk::VBox *vbox2;
	Gtk::HBox *hbox;
	Gtk::HBox *hbox2;
	Gtk::HBox *hbox3;

	grid = new Gtk::Grid();
	grid2 = new Gtk::Grid();
	vbox = new Gtk::VBox();
	vbox2 = new Gtk::VBox();
	hbox = new Gtk::HBox();
	hbox2 = new Gtk::HBox();
	hbox3 = new Gtk::HBox();

	frm_cam_calib.set_label("Camera calibration");
	fr_camCalib_offline.set_label("Offline Calibration");

	frm_cam_calib.add(*vbox);
	vbox->set_halign(Gtk::ALIGN_CENTER);
	vbox->set_valign(Gtk::ALIGN_CENTER);
	hbox->set_border_width(5);
	hbox->set_halign(Gtk::ALIGN_CENTER);
	hbox->pack_start(*grid, false, true, 5);
	hbox->set_halign(Gtk::ALIGN_CENTER);
	hbox->set_valign(Gtk::ALIGN_CENTER);
	label = new Gtk::Label("Calibration Mode: ");
	grid->attach(*label, 0, 0, 1, 1);
	calib_online.set_label("Online");
	grid->attach(calib_online, 1, 0, 1, 1);
	calib_offline.set_label("Offline");
	calib_offline.join_group(calib_online);
	grid->attach(calib_offline, 2, 0, 1, 1);
	vbox->pack_start(*hbox, false, true, 0);

	fr_camCalib_offline.set_label("Offline Calibration");
	hbox2->set_border_width(2);
	hbox2->set_halign(Gtk::ALIGN_CENTER);
	btn_camCalib_offline_start.set_label("Start");
	hbox2->pack_start(btn_camCalib_offline_start, true, true, 5);
	fr_camCalib_offline.add(*hbox2);
	vbox->pack_start(fr_camCalib_offline, false, false, 10);

	fr_camCalib_online.set_label("Online Calibration");
	hbox3->set_border_width(2);
	hbox3->set_halign(Gtk::ALIGN_CENTER);
	btn_camCalib.set_label("Cam calib.");
	hbox3->pack_start(btn_camCalib, false, false, 5);
	vbox2->pack_start(*hbox3, false, true, 0);
	grid2->set_margin_left(10);
	grid2->set_margin_right(10);
	grid2->set_margin_top(5);
	grid2->set_margin_bottom(5);
	grid2->set_column_spacing(5);
	grid2->set_column_homogeneous(true);
	btn_camCalib_collect.set_label("Collect");
	grid2->attach(btn_camCalib_collect, 0, 0, 1, 1);
	btn_camCalib_pop.set_label("Pop(0)");
	grid2->attach(btn_camCalib_pop, 1, 0, 1, 1);
	btn_camCalib_reset.set_label("Reset");
	grid2->attach(btn_camCalib_reset, 2, 0, 1, 1);
	btn_camCalib_start.set_label("Start");
	grid2->attach(btn_camCalib_start, 3, 0, 1, 1);
	vbox2->pack_start(*grid2, false, true, 0);
	fr_camCalib_online.add(*vbox2);
	vbox->pack_start(fr_camCalib_online, false, false, 10);

	btn_camCalib_collect.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib_pop.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib_reset.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib_start.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib.set_state(Gtk::STATE_INSENSITIVE);
	btn_camCalib_offline_start.set_state(Gtk::STATE_INSENSITIVE);
	calib_offline.set_state(Gtk::STATE_INSENSITIVE);
	calib_online.set_state(Gtk::STATE_INSENSITIVE);

	btn_camCalib_collect.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_camCalib_online_collect_clicked));
	btn_camCalib_pop.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_camCalib_online_pop_clicked));
	btn_camCalib_reset.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_camCalib_online_reset_clicked));
	btn_camCalib_start.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_camCalib_online_start_clicked));
	btn_camCalib.signal_toggled().connect(sigc::mem_fun(*this, &V4LInterface::__event_camCalib_pressed));
	btn_camCalib_offline_start.signal_clicked().connect(
			sigc::mem_fun(*this, &V4LInterface::__event_camCalib_offline_start_clicked));
	calib_offline.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_camCalib_mode_clicked));
	calib_online.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_camCalib_mode_clicked));
}

// Constructor
V4LInterface::V4LInterface(Messenger *messenger_ptr, const std::array<Robot2 *, 3> &robots_ref, RobotGUI &robot_gui,
						   bool isLow)
		:
		Gtk::VBox(false, 0),
		isLowRes(isLow),
		robots(robots_ref),
		robotGUI(robot_gui) {

	messenger = messenger_ptr;
	initInterface();
}
