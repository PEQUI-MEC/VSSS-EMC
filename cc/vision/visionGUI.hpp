#ifndef VISIONGUI_HPP_
#define VISIONGUI_HPP_

#include <gtkmm.h>
#include "vision.hpp"
#include "VideoRecorder.hpp"
#include "../filechooser.hpp"

namespace date {
	std::string generate_date_str();
}

class VisionGUI : public Gtk::VBox {
	public:
		std::unique_ptr<vision::Vision> vision;
		rec::VideoRecorder recorder;
		Gtk::ToggleButton bt_LAB_calib;

		unsigned long Img_id;

		bool CIELAB_calib_event_flag;

		Gtk::RadioButton rb_original_view, rb_split_view;

		Gtk::Scale HScale_cieL_min;
		Gtk::Scale HScale_cieA_min;
		Gtk::Scale HScale_cieB_min;
		Gtk::Scale HScale_cieL_max;
		Gtk::Scale HScale_cieA_max;
		Gtk::Scale HScale_cieB_max;

		Gtk::Scale HScale_Dilate;
		Gtk::Scale HScale_Erode;
		Gtk::Scale HScale_Blur;
		Gtk::Scale HScale_Amin;

		Gtk::Label lb_cieL_min, lb_cieL_max, lb_cieA_min, lb_cieA_max, lb_cieB_min, lb_cieB_max;

		Gtk::ToggleButton bt_record_video;
		Gtk::Button bt_save_picture;
		Gtk::Entry en_video_name, en_picture_name;

		VisionGUI();

		void update_vision_hscale_values();

		void selectFrame(int sector);

		void setFrameSize(int inWidth, int inHeight);
		bool getIsSplitView();
		bool getIsDrawing();

	private:

		// Frame Drawing Options
		Gtk::CheckButton draw_info_checkbox;
		bool draw_info_flag;

		// Frame Split View
		Gtk::Frame fr_splitView;
		bool isSplitView;
		bool disableSplitView;

		// Frame CIELAB Calibration
		Gtk::Frame fr_CIELAB;
		Gtk::Label CIELAB_label;
		Gtk::Button bt_CIELAB_left;
		Gtk::Button bt_CIELAB_right;
		Gtk::Button bt_switchMainAdv;

		void __event_bt_CIELAB_calib_pressed();
		void __event_bt_right_CIELAB_clicked();
		void __event_bt_left_CIELAB_clicked();
		void __event_bt_switchMainAdv_clicked();

		void __event_rb_split_mode_clicked();

		void bt_save_picture_clicked();
		void bt_record_video_pressed();

		void __create_frm_cielab();
		void __create_frm_capture();
		void __create_frm_split_view();
		void __create_frm_drawing_options();

		void HScale_cieL_min_value_changed();
		void HScale_cieA_min_value_changed();
		void HScale_cieB_min_value_changed();

		void event_draw_info_checkbox_signal_clicked();

		void HScale_cieL_max_value_changed();
		void HScale_cieA_max_value_changed();
		void HScale_cieB_max_value_changed();
		void HScale_Dilate_value_changed();
		void HScale_Erode_value_changed();
		void HScale_Blur_value_changed();
		void HScale_Amin_value_changed();
};

#endif /* VISIONGUI_HPP_ */
