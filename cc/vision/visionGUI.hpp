#ifndef VISIONGUI_HPP_
#define VISIONGUI_HPP_

#include <gtkmm.h>
#include "vision.hpp"
#include "gmm.hpp"
#include "../filechooser.hpp"

class VisionGUI: public Gtk::VBox
{
public:
  Vision *vision;
  GMM *gmm;
  Gtk::ToggleButton bt_HSV_calib;

  int Img_id;

  bool HSV_calib_event_flag;

  Gtk::RadioButton rb_original_view, rb_split_view;

  Gtk::Scale HScale_Hmin;
  Gtk::Scale HScale_Smin;
  Gtk::Scale HScale_Vmin;
  Gtk::Scale HScale_Hmax;
  Gtk::Scale HScale_Smax;
  Gtk::Scale HScale_Vmax;

  Gtk::Scale HScale_Dilate;
  Gtk::Scale HScale_Erode;

  Gtk::Scale HScale_Blur;

  Gtk::Scale HScale_Amin;

		Gtk::Label lb_Hmin, lb_Hmax, lb_Smin, lb_Smax, lb_Vmin, lb_Vmax;

  Gtk::ToggleButton bt_record_video;
  Gtk::Button bt_save_picture;
  Gtk::Entry en_video_name, en_picture_name;

  // Frame Calibration Mode
  Gtk::RadioButton rb_mode_GMM, rb_mode_HSV;

  VisionGUI();
  ~VisionGUI();

  void selectFrame(int sector);
  void hideGMM();

  void incrementSamples();
  void decrementSamples();

  void quickLoadGMM();

  void setFrameSize(int inWidth, int inHeight);
  int getFrameHeight();
  int getFrameWidth();
  bool getSamplesEventFlag();
  bool getDrawSamples();
  bool getOriginalFrameFlag();
  bool getGaussiansFrameFlag();
  bool getFinalFrameFlag();
  bool getThresholdFrameFlag();
  int getGMMColorIndex();
  bool getIsHSV();
  bool getIsSplitView();
  bool getIsDrawing();

private:

  // Frame Calibration Mode
  bool isHSV;

  // Frame Drawing Options
  Gtk::CheckButton draw_info_checkbox;
  bool draw_info_flag;

  // Frame Split View
  Gtk::Frame fr_splitView;
  bool isSplitView, disableSplitView;

  // Frame Capture
  int picIndex, vidIndex;

  // Frame HSV Calibration
  Gtk::Frame fr_HSV;
  Gtk::Label HSV_label;
  Gtk::Button bt_HSV_left;
  Gtk::Button bt_HSV_right;
  Gtk::Button bt_switchMainAdv;
        Gtk::ComboBoxText cb_convertType;

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
  int colorIndex;
  const std::vector<std::string> realColors {
    "Main", "Green", "Ball", "Opponent"
  };
  const std::vector<std::string> gaussianColors {
    "Black", "Yellow", "Green", "Orange", "Blue", "Pink", "White", "Red",
    "Purple", "Brown", "Silver", "Cyan", "Dark Green","Baby Pink", "Dark Grey"
  };
  bool samplesEventFlag;
  bool originalFrame_flag, gaussiansFrame_flag, finalFrame_flag;
  bool thresholdFrame_flag;

  void __event_bt_HSV_calib_pressed();
  void __event_bt_right_HSV_calib_clicked();
  void __event_bt_left_HSV_calib_clicked();
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
  void __create_frm_hsv();
  void __create_frm_capture();
  void __create_frm_gmm();
  void __create_frm_split_view();
  void __create_frm_drawing_options();

  void HScale_Hmin_value_changed();
  void HScale_Smin_value_changed();
  void HScale_Vmin_value_changed();

  void event_draw_info_checkbox_signal_clicked();

  void HScale_Hmax_value_changed();
  void HScale_Smax_value_changed();
  void HScale_Vmax_value_changed();
  void HScale_Dilate_value_changed();
  void HScale_Erode_value_changed();
  void HScale_Blur_value_changed();
  void HScale_Amin_value_changed();

  void init_calib_params();

  // void __event_auto_save();

    public:
		void __event_cb_convertType_changed();
};

#endif /* VISIONGUI_HPP_ */
