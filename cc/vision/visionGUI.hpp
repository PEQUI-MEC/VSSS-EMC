#ifndef VISIONGUI_HPP_
#define VISIONGUI_HPP_

#include <gtkmm.h>
#include "vision.hpp"

class VisionGUI: public Gtk::VBox
{
public:
  Vision *vision;
  Gtk::ToggleButton bt_HSV_calib;

  int Img_id;

  bool HSV_calib_event_flag;

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

  Gtk::ToggleButton bt_record_video;
  Gtk::Button bt_save_picture;
  Gtk::Entry en_video_name, en_picture_name;

  VisionGUI();
  ~VisionGUI();

  void setFrameSize(int inWidth, int inHeight);
  int getFrameHeight();
  int getFrameWidth();

private:

  // Frame Capture
  Gtk::Frame frm_capture;
  int picIndex, vidIndex;

  // Frame HSV Calibration
  Gtk::Label HSV_label;
  Gtk::Frame frm_calibration;
  Gtk::Button bt_HSV_left;
  Gtk::Button bt_HSV_right;

  void __event_bt_HSV_calib_pressed();
  void __event_bt_right_HSV_calib_clicked();
  void __event_bt_left_HSV_calib_clicked();

  void bt_save_picture_clicked();
  void bt_record_video_pressed();

  void __create_frm_calibration();
  void __create_frm_capture();

  void HScale_Hmin_value_changed();
  void HScale_Smin_value_changed();
  void HScale_Vmin_value_changed();

  void HScale_Hmax_value_changed();
  void HScale_Smax_value_changed();
  void HScale_Vmax_value_changed();
  void HScale_Dilate_value_changed();
  void HScale_Erode_value_changed();
  void HScale_Blur_value_changed();
  void HScale_Amin_value_changed();

  void init_calib_params();

  // void __event_auto_save();

};

#endif /* VISIONGUI_HPP_ */
