#ifndef VISIONGUI_HPP_
#define VISIONGUI_HPP_

#include <gtkmm.h>
#include "vision.hpp"
#include "gmm.hpp"
#include "VideoRecorder.hpp"
#include "../filechooser.hpp"

namespace date {
	std::string generate_date_str();
}

class VisionGUI : public Gtk::VBox {
	public:
		std::unique_ptr<vision::Vision> vision;
		std::unique_ptr<GMM> gmm;
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

		// Frame Calibration Mode
		Gtk::RadioButton rb_mode_GMM, rb_mode_CIELAB;

		VisionGUI();

		void update_vision_hscale_values();

		void selectFrame(int sector);
		void hideGMM();

		void incrementSamples();
		void decrementSamples();

		void quickLoadGMM();

		void setFrameSize(int inWidth, int inHeight);
		bool getSamplesEventFlag();
		bool getDrawSamples();
		bool getGaussiansFrameFlag();
		bool getFinalFrameFlag();
		bool getThresholdFrameFlag();
		unsigned long getGMMColorIndex();
		bool getIsCIELAB();
		bool getIsSplitView();
		bool getIsDrawing();

	private:

		// Frame Calibration Mode
		bool isCIELAB;

		// Frame Drawing Options
		Gtk::CheckButton draw_info_checkbox;
		bool draw_info_flag;

		// Frame Split View
		Gtk::Frame fr_splitView;
		bool isSplitView, disableSplitView;

		// Frame CIELAB Calibration
		Gtk::Frame fr_CIELAB;
		Gtk::Label CIELAB_label;
		Gtk::Button bt_CIELAB_left;
		Gtk::Button bt_CIELAB_right;
		Gtk::Button bt_switchMainAdv;

		// Frame GMM
		Gtk::Frame fr_GMM;
		Gtk::Button bt_GMM_save, bt_GMM_load;
		Gtk::ToggleButton bt_collectSamples;
		Gtk::Button bt_popSample, bt_clearSamples;
		Gtk::Button bt_trainGMM, bt_GMM_match, bt_GMM_done;
		Gtk::ComboBoxText cb_gaussianColor, cb_realColor;
		Gtk::HScale HScale_clusters;
		Gtk::HScale HScale_GMM_blur, HScale_GMM_erode, HScale_GMM_dilate;
		Gtk::RadioButton rb_GMM_original, rb_GMM_gaussians, rb_GMM_final, rb_GMM_threshold;
		Gtk::Button bt_GMM_left, bt_GMM_right;
		Gtk::Label lb_threshold;
		int totalSamples;
		unsigned long colorIndex;
		const std::vector<std::string> realColors{
				"Main", "Green", "Ball", "Opponent"
		};
		const std::vector<std::string> gaussianColors{
				"Black", "Yellow", "Green", "Orange", "Blue", "Pink", "White", "Red",
				"Purple", "Brown", "Silver", "Cyan", "Dark Green", "Baby Pink", "Dark Grey"
		};
		bool samplesEventFlag;
		bool gaussiansFrame_flag, finalFrame_flag;
		bool thresholdFrame_flag;

		void __event_bt_CIELAB_calib_pressed();
		void __event_bt_right_CIELAB_clicked();
		void __event_bt_left_CIELAB_clicked();
		void __event_bt_switchMainAdv_clicked();

		void __event_bt_GMM_save_clicked();
		void __event_bt_GMM_load_clicked();
		void __event_bt_collectSamples_pressed();
		void __event_bt_popSample_clicked();
		void __event_bt_clearSamples_clicked();
		void __event_bt_trainGMM_clicked();
		void HScale_clusters_value_changed();
		void __event_bt_GMM_match_clicked();
		void __event_bt_GMM_done_clicked();
		void __event_rb_GMM_frame_clicked();
		void __event_bt_GMM_left_clicked();
		void __event_bt_GMM_right_clicked();
		void HScale_GMM_blur_value_changed();
		void HScale_GMM_erode_value_changed();
		void HScale_GMM_dilate_value_changed();

		void __event_rb_mode_clicked();

		void __event_rb_split_mode_clicked();

		void bt_save_picture_clicked();
		void bt_record_video_pressed();

		void __create_frm_calib_mode();
		void __create_frm_cielab();
		void __create_frm_capture();
		void __create_frm_gmm();
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
