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
	bool V4LInterface::on_button_press_event(GdkEventButton *event){
		
		// Check if the event is a left(1) button click.
    if( (event->type == GDK_BUTTON_PRESS) )
    {
     //  std::cout<<"click"<<std::endl;
        
        return true;
    }else  return false;
		
		}

	void V4LInterface::HScale_Amin_value_changed(){
  
  Amin[Img_id]=HScale_Amin.get_value();

}		
	void V4LInterface::HScale_offsetR_value_changed(){
  
  offsetR=HScale_offsetR.get_value();

}
void V4LInterface::HScale_offsetL_value_changed(){
  
   offsetL=HScale_offsetL.get_value();

}
	void V4LInterface::HScale_Hmin_value_changed(){
  
   H[Img_id][0]=HScale_Hmin.get_value();
  // std::cout<<H[Img_id][0]<<std::endl;

}
	void V4LInterface::HScale_Smin_value_changed(){
  
   S[Img_id][0]=HScale_Smin.get_value();
  //  std::cout<<S[Img_id][0]<<std::endl;

}
	void V4LInterface::HScale_Vmin_value_changed(){
  
   V[Img_id][0]=HScale_Vmin.get_value();
   // std::cout<<V[Img_id][0]<<std::endl;

}
	void V4LInterface::HScale_Hmax_value_changed(){
  
	H[Img_id][1]=HScale_Hmax.get_value();


}
	void V4LInterface::HScale_Smax_value_changed(){
  
   S[Img_id][1]=HScale_Smax.get_value();

}
	void V4LInterface::HScale_Vmax_value_changed(){
  
    V[Img_id][1]=HScale_Vmax.get_value();
	
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

	void V4LInterface::__create_frm_device_info() {
		Gtk::HBox * hbox;
		Gtk::VBox * vbox;
		Gtk::Label * label;
		Gtk::Table * table;

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

		Gtk::Label * label;
		Gtk::Table * table;
		Gtk::HBox * hbox;
		Gtk::VBox * vbox;
		
		// Adiciona a vbox (principal) no frame
		vbox = new Gtk::VBox();
		
		
		// Primeira Hbox com oos botões Warp, Reset, Save, Load, Adjust
		hbox = new Gtk::HBox();
		hbox->set_border_width(5);
		hbox->set_halign(Gtk::ALIGN_CENTER);
		
		
		
		bt_save_cam_prop.set_label("Save");
		hbox->pack_start(bt_save_cam_prop, false, true, 5);
		bt_load_cam_prop.set_label("Load");
		hbox->pack_start(bt_load_cam_prop, false, true, 5);
		bt_quick_save.set_label("Quick Save");
		hbox->pack_start(bt_quick_save, false, true, 5);
		bt_quick_load.set_label("Quick Load");
		hbox->pack_start(bt_quick_load, false, true, 5);
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
	void V4LInterface::__create_frm_warp(){
		Gtk::HBox * hbox;
		Gtk::HBox * hbox2;
		Gtk::HBox * hbox3;
		Gtk::VBox * vbox;
		Gtk::Label * label;
		
		
		// Adiciona a vbox (principal) no frame
		vbox = new Gtk::VBox();
		frm_warp.add(*vbox);
		frm_warp.set_label("Warp");
		
		// Primeira Hbox com oos botões Warp, Reset, Save, Load, Adjust
		hbox = new Gtk::HBox();
		hbox->set_border_width(5);
		hbox->set_halign(Gtk::ALIGN_CENTER);
		vbox->pack_start(*hbox, false, true, 0);
		bt_warp.set_label("Warp");
		hbox->pack_start(bt_warp, false, true, 5);
		bt_reset_warp.set_label("Reset");
		hbox->pack_start(bt_reset_warp, false, true, 5);
		bt_save_warp.set_label("Save");
		hbox->pack_start(bt_save_warp, false, true, 5);
		bt_load_warp.set_label("Load");
		hbox->pack_start(bt_load_warp, false, true, 5);
		bt_adjust.set_label("Adjust");
		hbox->pack_start(bt_adjust, false, true, 5);
		bt_invert_image.set_label("Invert Image");
  		hbox->pack_start(bt_invert_image, false, true, 5);
  		bt_invert_image.signal_clicked().connect(sigc::mem_fun(*this, 
  			&V4LInterface::__event_bt_invert_image_signal_clicked));
		
		
		// Terceira Hbox com as barras de offset e suas labels		
		hbox2 = new Gtk::HBox();
		hbox2->set_border_width(5);
		hbox2->set_halign(Gtk::ALIGN_CENTER);
		label = new Gtk::Label("Offset L");
		hbox2->pack_start(*label, Gtk::PACK_SHRINK, 5);
		hbox2->pack_start(HScale_offsetL, false, true, 5);
		label = new Gtk::Label("Offset R");
		hbox2->pack_start(*label, Gtk::PACK_SHRINK, 5);
		hbox2->pack_start(HScale_offsetR, false, true, 5);
		vbox->pack_start(*hbox2, false, true, 0);
		
		
		// Configurações da barra do offset right
		HScale_offsetR.set_digits(0);
		HScale_offsetR.set_increments(1,1);
		HScale_offsetR.set_range(0,100);
		HScale_offsetR.set_value_pos(Gtk::POS_TOP);
		HScale_offsetR.set_draw_value();
		HScale_offsetR.set_size_request(100,-1);
		HScale_offsetR.signal_value_changed().connect(sigc::mem_fun(*this,
    &V4LInterface::HScale_offsetR_value_changed));
		
    
		// Configurações da barra do offset left
		HScale_offsetL.set_digits(0);
		HScale_offsetL.set_increments(1,1);
		HScale_offsetL.set_range(0,100);
		HScale_offsetL.set_value_pos(Gtk::POS_TOP);
		HScale_offsetL.set_draw_value();
		HScale_offsetL.set_size_request(100,-1);
		HScale_offsetL.signal_value_changed().connect(sigc::mem_fun(*this,
    &V4LInterface::HScale_offsetL_value_changed));
		
		
	}
	
	void V4LInterface::__create_frm_calibration() {
		Gtk::HBox * hbox;
		Gtk::HBox * hbox2;
		Gtk::HBox * hbox3;
		Gtk::VBox * vbox;
		Gtk::HBox * hboxH;
		Gtk::HBox * hboxS;
		Gtk::HBox * hboxV;
		Gtk::HBox * hboxA;
		Gtk::Label * label;
		
		vbox = new Gtk::VBox();
		frm_calibration.add(*vbox);
		
		frm_calibration.set_label("Calibration");
		
		hbox2 = new Gtk::HBox();
		hbox2->set_border_width(10);
		hbox2->set_halign(Gtk::ALIGN_CENTER);
		vbox->pack_start(*hbox2, false, true, 0);
		hbox2->pack_start(bt_HSV_calib, false, true, 10);
		bt_HSV_calib.set_label("HSV Calib.");
		
		
		hbox2->pack_start(bt_save_HSV_calib, false, true, 2);
		bt_save_HSV_calib.set_label("Save");
		hbox2->pack_start(bt_load_HSV_calib, false, true, 2);
		bt_load_HSV_calib.set_label("Load");
		bt_auto_calib.set_label("AUTO Calib.");
		hbox2->pack_start(bt_auto_calib, false, true, 10);

		
		
		hbox3 = new Gtk::HBox();
		hbox3->set_halign(Gtk::ALIGN_CENTER);
		vbox->pack_start(*hbox3, false, true, 0);
		hbox3->pack_start(bt_HSV_left, false, true, 10);
		bt_HSV_left.set_label(" < ");
		bt_HSV_left.set_alignment(.5, .5);
		HSV_label.set_text("Principal");
		//HSV_label.set_alignment(0, .5);
		hbox3->pack_start(HSV_label, false, true, 10);
		hbox3->pack_start(bt_HSV_right, false, true, 10);
		bt_HSV_right.set_label(" > ");
		
		
		
		HScale_Amin.set_digits(0);
		HScale_Amin.set_increments(1,1);
		HScale_Amin.set_range(0,1000);
		HScale_Amin.set_value_pos(Gtk::POS_TOP);
		HScale_Amin.set_draw_value();
		HScale_Amin.signal_value_changed().connect(sigc::mem_fun(*this,
    &V4LInterface::HScale_Amin_value_changed));
		
		HScale_Hmin.set_digits(0);
		HScale_Hmin.set_increments(1,1);
		HScale_Hmin.set_range(-1,256);
		HScale_Hmin.set_value_pos(Gtk::POS_TOP);
		HScale_Hmin.set_draw_value();
		HScale_Hmin.signal_value_changed().connect(sigc::mem_fun(*this,
    &V4LInterface::HScale_Hmin_value_changed));
    
    
		HScale_Smin.set_digits(0);
		HScale_Smin.set_increments(1,1);
		HScale_Smin.set_range(-1,256);
		HScale_Smin.set_value_pos(Gtk::POS_TOP);
		HScale_Smin.set_draw_value();
		HScale_Smin.signal_value_changed().connect(sigc::mem_fun(*this,
    &V4LInterface::HScale_Smin_value_changed));
    
		HScale_Vmin.set_digits(0);
		HScale_Vmin.set_increments(1,1);
		HScale_Vmin.set_range(-1,256);
		HScale_Vmin.set_value_pos(Gtk::POS_TOP);
		HScale_Vmin.set_draw_value();
		HScale_Vmin.signal_value_changed().connect(sigc::mem_fun(*this,
    &V4LInterface::HScale_Vmin_value_changed));
    
		
    	HScale_Hmax.set_digits(0);
		HScale_Hmax.set_increments(1,1);
		HScale_Hmax.set_range(-1,256);
		HScale_Hmax.set_value(255);
		HScale_Hmax.set_value_pos(Gtk::POS_TOP);
		HScale_Hmax.set_draw_value();
		HScale_Hmax.signal_value_changed().connect(sigc::mem_fun(*this,
    &V4LInterface::HScale_Hmax_value_changed));
    
		HScale_Smax.set_value(254);
		HScale_Smax.set_digits(0);
		HScale_Smax.set_increments(1,1);
		HScale_Smax.set_range(-1,256);
		HScale_Smax.set_value(255);
		HScale_Smax.set_value_pos(Gtk::POS_TOP);
		HScale_Smax.set_draw_value();
		HScale_Smax.signal_value_changed().connect(sigc::mem_fun(*this,
    &V4LInterface::HScale_Smax_value_changed));
    
		
		HScale_Vmax.set_digits(0);
		HScale_Vmax.set_increments(1,1);
		HScale_Vmax.set_range(-1,256);
		HScale_Vmax.set_value(255);
		HScale_Vmax.set_value_pos(Gtk::POS_TOP);
		HScale_Vmax.set_draw_value();
		HScale_Vmax.signal_value_changed().connect(sigc::mem_fun(*this,
    &V4LInterface::HScale_Vmax_value_changed));
    
		
		
    	hboxH = new Gtk::HBox();
    	hboxH->set_halign(Gtk::ALIGN_CENTER);
		vbox->pack_start(*hboxH);
		label = new Gtk::Label("Hmin:");
		label->set_alignment(1.0, 1.0);
		hboxH->pack_start(*label, false, true, 5);
		hboxH->pack_start(HScale_Hmin, false, true, 5);
		label = new Gtk::Label("Hmax:");
		label->set_alignment(1.0, 1.0);
		hboxH->pack_start(*label, false, true, 5);
		hboxH->pack_start(HScale_Hmax, false, true, 5);
		
		hboxS = new Gtk::HBox();
		hboxS->set_halign(Gtk::ALIGN_CENTER);
		vbox->pack_start(*hboxS);
		label = new Gtk::Label("Smin:");
		label->set_alignment(1.0, 1.0);
		hboxS->pack_start(*label, false, true, 5);
		hboxS->pack_start(HScale_Smin, false, true, 5);
		label = new Gtk::Label("Smax:");
		label->set_alignment(1.0, 1.0);
		hboxS->pack_start(*label, false, true, 5);
		hboxS->pack_start(HScale_Smax, false, true, 5);
		
		hboxV = new Gtk::HBox();
		hboxV->set_halign(Gtk::ALIGN_CENTER);
		vbox->pack_start(*hboxV);
		label = new Gtk::Label("Vmin:");
		label->set_alignment(1.0, 1.0);
		hboxV->pack_start(*label, false, true, 5);
		hboxV->pack_start(HScale_Vmin, true, true, 5);
		label = new Gtk::Label("Vmax:");
		label->set_alignment(1.0, 1.0);
		hboxV->pack_start(*label, false, true, 5);
		hboxV->pack_start(HScale_Vmax, false, true, 5);
		
		hboxA = new Gtk::HBox();
		hboxA->set_halign(Gtk::ALIGN_CENTER);
		vbox->pack_start(*hboxA);
		label = new Gtk::Label("Amin:");
		label->set_alignment(1.0, 1.0);
		hboxA->pack_start(*label, false, true, 5);
		hboxA->pack_start(HScale_Amin, true, true, 5);
		
		
		HScale_Hmax.set_size_request(130,-1);
		HScale_Hmin.set_size_request(130,-1);
		HScale_Smax.set_size_request(130,-1);
		HScale_Smin.set_size_request(130,-1);
		HScale_Vmax.set_size_request(130,-1);
		HScale_Vmin.set_size_request(130,-1);
		HScale_Amin.set_size_request(260,-1);
		
		
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

		struct v4l2_standard standard;

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

		struct v4l2_format fmt;
		struct v4l2_fmtdesc fmt_desc;
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

		struct v4l2_format fmt;
		vcap.get_format(&fmt, V4L2_BUF_TYPE_VIDEO_CAPTURE);
		sp_width.set_value((double) fmt.fmt.pix.width);
		sp_height.set_value((double) fmt.fmt.pix.height);

		struct v4l2_frmsizeenum frmsize;

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

		struct v4l2_format fmt;
		unsigned int & width = fmt.fmt.pix.width;
		unsigned int & height = fmt.fmt.pix.height;

		vcap.get_format(&fmt, V4L2_BUF_TYPE_VIDEO_CAPTURE);
		sp_width.set_value((double) width);
		sp_height.set_value((double) height);

		struct v4l2_frmivalenum frminterval;

		struct v4l2_streamparm streamparm;
		vcap.get_stream_parameter(&streamparm, V4L2_BUF_TYPE_VIDEO_CAPTURE);

		if (vcap.enum_frame_interval(&frminterval, fmt.fmt.pix.pixelformat, width, height, 0, true)) {
			do {
				Gtk::TreeModel::Row row = *(ls_frame_interval->append());
				float fps = (float) frminterval.discrete.denominator / (float) frminterval.discrete.numerator;
				Glib::ustring name = Glib::ustring::compose("%1/%2 = %3 fps", frminterval.discrete.numerator, frminterval.discrete.denominator, fps);
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
		__update_cb_input();
		__update_cb_standard();
		__update_cb_format_desc();
		__update_cb_frame_size();
		__update_cb_frame_interval();
	}

	void V4LInterface::__make_control_list_default() {

		ctrl_list_default.clear();
		struct v4l2_queryctrl qctrl;
		struct v4l2_querymenu qmenu;

		if (vcap.enum_control_default(&qctrl, true)) {
			do {

				Gtk::Widget * wctrl = 0;
				ControlHolder hold;
				Glib::RefPtr<Gtk::ListStore> lstore;

				switch (qctrl.type) {
					case V4L2_CTRL_TYPE_INTEGER:
					case V4L2_CTRL_TYPE_INTEGER64:

						wctrl = new Gtk::HScale();
						static_cast<Gtk::HScale *>(wctrl)->set_value_pos(Gtk::POS_RIGHT);
						static_cast<Gtk::HScale *>(wctrl)->set_range(qctrl.minimum, qctrl.maximum);
						static_cast<Gtk::HScale *>(wctrl)->set_increments(qctrl.step, 10 * qctrl.step);
						hold.con = static_cast<Gtk::HScale *>(wctrl)->signal_change_value().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control_hscale),
						                &ctrl_list_default, wctrl));
						break;

					case V4L2_CTRL_TYPE_BOOLEAN:

						wctrl = new Gtk::CheckButton((const char *) qctrl.name);
						hold.con = static_cast<Gtk::CheckButton *>(wctrl)->signal_clicked().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_default,
						                wctrl));
						break;

					case V4L2_CTRL_TYPE_BUTTON:

						wctrl = new Gtk::Button((const char *) qctrl.name);
						hold.con = static_cast<Gtk::Button *>(wctrl)->signal_clicked().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_default,
						                wctrl));
						break;

					case V4L2_CTRL_TYPE_STRING:

						wctrl = new Gtk::Label((const char *) qctrl.name);
						break;

					case V4L2_CTRL_TYPE_MENU:
					case V4L2_CTRL_TYPE_INTEGER_MENU:

						wctrl = new Gtk::ComboBox();
						lstore = Gtk::ListStore::create(model_control_menu);
						static_cast<Gtk::ComboBox *>(wctrl)->set_model(lstore);
						static_cast<Gtk::ComboBox *>(wctrl)->pack_start(model_control_menu.m_col_name);

						if (vcap.enum_control_menu(&qmenu, qctrl, 0, true)) {
							do {
								Gtk::TreeModel::Row row = *(lstore->append());
								Glib::ustring name = (const char *) qmenu.name;
								row[model_control_menu.m_col_name] = name;
								row[model_control_menu.m_col_data] = qmenu;
							} while (vcap.enum_control_menu(&qmenu, qctrl));
						}

						hold.con = static_cast<Gtk::ComboBox *>(wctrl)->signal_changed().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_default,
						                wctrl));
						break;

					case V4L2_CTRL_TYPE_CTRL_CLASS:
					case V4L2_CTRL_TYPE_BITMASK:
					default:
						wctrl = 0;
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
/*
	void V4LInterface::__make_control_list_user() {

		ctrl_list_user.clear();
		struct v4l2_queryctrl qctrl;
		struct v4l2_querymenu qmenu;

		if (vcap.enum_control_default(&qctrl, true)) {
			do {

				Gtk::Widget * wctrl = 0;
				ControlHolder hold;
				Glib::RefPtr<Gtk::ListStore> lstore;

				switch (qctrl.type) {
					case V4L2_CTRL_TYPE_INTEGER:
					case V4L2_CTRL_TYPE_INTEGER64:

						wctrl = new Gtk::HScale();
						static_cast<Gtk::HScale *>(wctrl)->set_value_pos(Gtk::POS_RIGHT);
						static_cast<Gtk::HScale *>(wctrl)->set_range(qctrl.minimum, qctrl.maximum);
						static_cast<Gtk::HScale *>(wctrl)->set_increments(qctrl.step, 10 * qctrl.step);
						hold.con = static_cast<Gtk::HScale *>(wctrl)->signal_change_value().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control_hscale),
						                &ctrl_list_user, wctrl));
						break;

					case V4L2_CTRL_TYPE_BOOLEAN:

						wctrl = new Gtk::CheckButton((const char *) qctrl.name);
						hold.con = static_cast<Gtk::CheckButton *>(wctrl)->signal_clicked().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_user,
						                wctrl));
						break;

					case V4L2_CTRL_TYPE_BUTTON:

						wctrl = new Gtk::Button((const char *) qctrl.name);
						hold.con = static_cast<Gtk::Button *>(wctrl)->signal_clicked().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_user,
						                wctrl));
						break;

					case V4L2_CTRL_TYPE_STRING:

						wctrl = new Gtk::Label((const char *) qctrl.name);
						break;

					case V4L2_CTRL_TYPE_MENU:
					case V4L2_CTRL_TYPE_INTEGER_MENU:

						wctrl = new Gtk::ComboBox();
						lstore = Gtk::ListStore::create(model_control_menu);
						static_cast<Gtk::ComboBox *>(wctrl)->set_model(lstore);
						static_cast<Gtk::ComboBox *>(wctrl)->pack_start(model_control_menu.m_col_name);

						if (vcap.enum_control_menu(&qmenu, qctrl, 0, true)) {
							do {
								Gtk::TreeModel::Row row = *(lstore->append());
								Glib::ustring name = (const char *) qmenu.name;
								row[model_control_menu.m_col_name] = name;
								row[model_control_menu.m_col_data] = qmenu;
							} while (vcap.enum_control_menu(&qmenu, qctrl));
						}

						hold.con = static_cast<Gtk::ComboBox *>(wctrl)->signal_changed().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_user,
						                wctrl));
						break;

					case V4L2_CTRL_TYPE_CTRL_CLASS:
					case V4L2_CTRL_TYPE_BITMASK:
					default:
						wctrl = 0;
						break;

				}

				if (wctrl) {
					hold.qctrl = qctrl;
					hold.widget = wctrl;
					ctrl_list_user.push_back(hold);
				}

			} while (vcap.enum_control_default(&qctrl));
		}

	}
*/
/*
	void V4LInterface::__make_control_list_private() {

		ctrl_list_private.clear();
		struct v4l2_queryctrl qctrl;
		struct v4l2_querymenu qmenu;

		if (vcap.enum_control_default(&qctrl, true)) {
			do {

				Gtk::Widget * wctrl = 0;
				ControlHolder hold;
				Glib::RefPtr<Gtk::ListStore> lstore;

				switch (qctrl.type) {
					case V4L2_CTRL_TYPE_INTEGER:
					case V4L2_CTRL_TYPE_INTEGER64:

						wctrl = new Gtk::HScale();
						static_cast<Gtk::HScale *>(wctrl)->set_value_pos(Gtk::POS_RIGHT);
						static_cast<Gtk::HScale *>(wctrl)->set_range(qctrl.minimum, qctrl.maximum);
						static_cast<Gtk::HScale *>(wctrl)->set_increments(qctrl.step, 10 * qctrl.step);
						hold.con = static_cast<Gtk::HScale *>(wctrl)->signal_change_value().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control_hscale),
						                &ctrl_list_private, wctrl));
						break;

					case V4L2_CTRL_TYPE_BOOLEAN:

						wctrl = new Gtk::CheckButton((const char *) qctrl.name);
						hold.con = static_cast<Gtk::CheckButton *>(wctrl)->signal_clicked().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_private,
						                wctrl));
						break;

					case V4L2_CTRL_TYPE_BUTTON:

						wctrl = new Gtk::Button((const char *) qctrl.name);
						hold.con = static_cast<Gtk::Button *>(wctrl)->signal_clicked().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_private,
						                wctrl));
						break;

					case V4L2_CTRL_TYPE_STRING:

						wctrl = new Gtk::Label((const char *) qctrl.name);
						break;

					case V4L2_CTRL_TYPE_MENU:
					case V4L2_CTRL_TYPE_INTEGER_MENU:

						wctrl = new Gtk::ComboBox();
						lstore = Gtk::ListStore::create(model_control_menu);
						static_cast<Gtk::ComboBox *>(wctrl)->set_model(lstore);
						static_cast<Gtk::ComboBox *>(wctrl)->pack_start(model_control_menu.m_col_name);

						if (vcap.enum_control_menu(&qmenu, qctrl, 0, true)) {
							do {
								Gtk::TreeModel::Row row = *(lstore->append());
								Glib::ustring name = (const char *) qmenu.name;
								row[model_control_menu.m_col_name] = name;
								row[model_control_menu.m_col_data] = qmenu;
							} while (vcap.enum_control_menu(&qmenu, qctrl));
						}

						hold.con = static_cast<Gtk::ComboBox *>(wctrl)->signal_changed().connect(
						        sigc::bind<std::list<ControlHolder> *, Gtk::Widget *>(sigc::mem_fun(*this, &V4LInterface::__set_control), &ctrl_list_private,
						                wctrl));
						break;

					case V4L2_CTRL_TYPE_CTRL_CLASS:
					case V4L2_CTRL_TYPE_BITMASK:
					default:
						wctrl = 0;
						break;

				}

				if (wctrl) {
					hold.qctrl = qctrl;
					hold.widget = wctrl;
					ctrl_list_private.push_back(hold);
				}

			} while (vcap.enum_control_default(&qctrl));
		}

	}
*/
	void V4LInterface::__make_control_table(std::list<ControlHolder>& list, const char * title) {

		// create new controls =====================================================
		Gtk::Label * label = NULL;
		Gtk::Table * table = NULL;
		Gtk::Widget * wctrl = NULL;

		int ROWS = 4;
		int COLS = 2;
		int r, c;
		int tab_count = 0;
		int count = 0;

		struct v4l2_queryctrl qctrl;
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

	void V4LInterface::__block_control_signals(std::list<ControlHolder>& list, bool block) {

		std::list<ControlHolder>::iterator iter;

		for (iter = list.begin(); iter != list.end(); ++iter) {
			(*iter).con.block(block);
		}
	}

	void V4LInterface::__update_control_widgets(std::list<ControlHolder>& ctrl_list) {

		Gtk::Widget * wctrl = NULL;
		struct v4l2_queryctrl qctrl;
		struct v4l2_control control;

		__block_control_signals(ctrl_list, true);

		std::list<ControlHolder>::iterator iter;

		for (iter = ctrl_list.begin(); iter != ctrl_list.end(); ++iter) {
			qctrl = (*iter).qctrl;
			wctrl = (*iter).widget;

			vcap.get_control(&control, qctrl.id);

			switch (qctrl.type) {
				case V4L2_CTRL_TYPE_INTEGER:
				case V4L2_CTRL_TYPE_INTEGER64:

					static_cast<Gtk::HScale *>(wctrl)->set_value(control.value);
					break;

				case V4L2_CTRL_TYPE_BOOLEAN:

					static_cast<Gtk::CheckButton *>(wctrl)->set_active(control.value == 1);
					break;

				case V4L2_CTRL_TYPE_BUTTON:

					break;

				case V4L2_CTRL_TYPE_STRING:

					break;

				case V4L2_CTRL_TYPE_MENU:
				case V4L2_CTRL_TYPE_INTEGER_MENU:

					Glib::RefPtr<Gtk::TreeModel> model = static_cast<Gtk::ComboBox *>(wctrl)->get_model();

					Gtk::TreeModel::Children child = model->children();

					int i = 0;
					for (Gtk::TreeModel::Children::iterator iter = child.begin(); iter != child.end(); ++iter, ++i) {

						Gtk::TreeModel::Row row = *iter;
						struct v4l2_querymenu qm;
						qm = row[model_control_menu.m_col_data];

						if (control.value < 0) return;

						if (control.value == (int) qm.index) {
							static_cast<Gtk::ComboBox *>(wctrl)->set_active(iter);
							break;
						}

					}

					break;

			}
		}
		__block_control_signals(ctrl_list, false);

	}

	// Constructor

	V4LInterface::V4LInterface() :
			Gtk::VBox(false, 0) {
			Img_id=0;
			warp_event_flag=false;
			save_warp_flag=false;
			load_warp_flag=false;
			reset_warp_flag=false;
			offsetL=0;
			offsetR=0;
			HSV_calib_event_flag=false;
			save_HSV_calib_flag=false;
			load_HSV_calib_flag=false;

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
			bt_save_cam_prop.set_sensitive(false);
			bt_load_cam_prop.set_sensitive(false);
			bt_reset_warp.set_sensitive(false);
			bt_load_warp.set_sensitive(false);
			bt_save_warp.set_sensitive(false);
			//bt_invert_image.set_sensitive(false);
			bt_quick_save.set_sensitive(false);
			bt_quick_load.set_sensitive(false);
			m_signal_start.emit(false);

			
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
			bt_auto_calib.set_state(Gtk::STATE_INSENSITIVE);
			bt_adjust.set_state(Gtk::STATE_INSENSITIVE);
			

			for(int i =0; i<6; i++){
				
				H[i][0] = 0;
				S[i][0] = 0;
				V[i][0] = 0;
				H[i][1] = 255;
				S[i][1] = 255;
				V[i][1] = 255;
				Amin[i] = 10;
			}
				
				
				
				
		notebook.set_scrollable(true);
		warp_event_flag = false;
		adjust_event_flag = false;
		sp_width.set_range(0, 2000);
		sp_width.set_increments(1, 10);
		sp_height.set_range(0, 2000);
		sp_height.set_increments(1, 10);
		sp_width.set_tooltip_text("Width");
		sp_height.set_tooltip_text("Height");

		__init_combo_boxes();

		pack_start(frm_device_info, false, false, 10);
		__create_frm_device_info();

		pack_start(frm_device_prop, false, false, 10);
		__create_frm_device_properties();
		
		pack_start(frm_warp, false, false, 10);
		__create_frm_warp();
		
		pack_start(frm_calibration, false, false, 10);
		__create_frm_calibration();

		__update_cb_device();


		bt_save_cam_prop.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_save_cam_prop_clicked));
		bt_load_cam_prop.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_load_cam_prop_clicked));
		bt_quick_save.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_quick_save_clicked));
		bt_quick_load.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_quick_load_clicked));

		bt_start.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_start_clicked));
		bt_warp.signal_pressed().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_warp_clicked));
		bt_reset_warp.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_reset_warp_clicked));
		bt_adjust.signal_pressed().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_adjust_pressed));
		bt_load_warp.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_load_warp_clicked));
		bt_save_warp.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_save_warp_clicked));
		
		bt_HSV_calib.signal_pressed().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_HSV_calib_pressed));
		bt_auto_calib.signal_pressed().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_auto_calib_pressed));
		bt_load_HSV_calib.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_load_HSV_calib_clicked));
		bt_save_HSV_calib.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_save_HSV_calib_clicked));
		bt_HSV_right.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_right_HSV_calib_clicked));
		bt_HSV_left.signal_clicked().connect(sigc::mem_fun(*this, &V4LInterface::__event_bt_left_HSV_calib_clicked));
		
		cb_input_signal = cb_device.signal_changed().connect(sigc::mem_fun(*this, &V4LInterface::__event_cb_device_changed));
		cb_input_signal = cb_input.signal_changed().connect(sigc::mem_fun(*this, &V4LInterface::__event_cb_input_changed));
		cb_standard_signal = cb_standard.signal_changed().connect(sigc::mem_fun(*this, &V4LInterface::__event_cb_standard_changed));
		cb_format_desc_signal = cb_format_desc.signal_changed().connect(sigc::mem_fun(*this, &V4LInterface::__event_cb_format_desc_changed));
		cb_frame_size_signal = cb_frame_size.signal_changed().connect(sigc::mem_fun(*this, &V4LInterface::__event_cb_frame_size_changed));
		cb_frame_interval_signal = cb_frame_interval.signal_changed().connect(sigc::mem_fun(*this, &V4LInterface::__event_cb_frame_interval_changed));
		
	}
}

