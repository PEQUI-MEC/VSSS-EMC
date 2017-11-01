#include "visionGUI.hpp"

void VisionGUI::__create_frm_calib_mode() {
  Gtk::VBox * vbox;
  Gtk::Grid * grid;
  Gtk::Frame * frame;
  Gtk::Label *label;

  vbox = new Gtk::VBox();
  grid = new Gtk::Grid();
  frame = new Gtk::Frame();

  pack_start(*frame, false, false, 5);

  frame->add(*vbox);
  vbox->pack_start(*grid, false, true, 5);
  vbox->set_halign(Gtk::ALIGN_CENTER);
  vbox->set_valign(Gtk::ALIGN_CENTER);

  frame->set_label("Choose Your Destiny");

  grid->set_border_width(10);
  grid->set_column_spacing(10);
  grid->set_row_spacing(5);

  label = new Gtk::Label("Calibration Mode: ");
  grid->attach(*label, 0, 0, 1, 1);
  rb_mode_HSV.set_label("HSV");
  grid->attach(rb_mode_HSV, 1, 0, 1, 1);
  rb_mode_GMM.set_label("GMM");
  rb_mode_GMM.join_group(rb_mode_HSV);
  grid->attach(rb_mode_GMM, 2, 0, 1, 1);

  rb_mode_HSV.set_state(Gtk::STATE_INSENSITIVE);
  rb_mode_GMM.set_state(Gtk::STATE_INSENSITIVE);

  rb_mode_HSV.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_rb_mode_clicked));
  rb_mode_GMM.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_rb_mode_clicked));

}

void VisionGUI::__event_rb_mode_clicked() {
  if (rb_mode_GMM.get_active()) {
    isHSV = false;
    if (gmm.getIsTrained()) bt_GMM_save.set_state(Gtk::STATE_NORMAL);
    bt_GMM_load.set_state(Gtk::STATE_NORMAL);
    bt_collectSamples.set_state(Gtk::STATE_NORMAL);
    bt_popSample.set_state(Gtk::STATE_NORMAL);
    bt_clearSamples.set_state(Gtk::STATE_NORMAL);
    bt_trainGMM.set_state(Gtk::STATE_NORMAL);
    bt_GMM_match.set_state(Gtk::STATE_NORMAL);
    bt_GMM_done.set_state(Gtk::STATE_NORMAL);
    cb_gaussianColor.set_state(Gtk::STATE_NORMAL);
    cb_realColor.set_state(Gtk::STATE_NORMAL);
    HScale_clusters.set_state(Gtk::STATE_NORMAL);
    HScale_closing.set_state(Gtk::STATE_NORMAL);
    HScale_opening.set_state(Gtk::STATE_NORMAL);
    rb_GMM_original.set_state(Gtk::STATE_NORMAL);
    rb_GMM_gaussians.set_state(Gtk::STATE_NORMAL);
    rb_GMM_final.set_state(Gtk::STATE_NORMAL);
    if (gmm.getDoneFlag()) rb_GMM_threshold.set_state(Gtk::STATE_NORMAL);
    bt_GMM_left.set_state(Gtk::STATE_NORMAL);
    bt_GMM_right.set_state(Gtk::STATE_NORMAL);
    bt_HSV_calib.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Hmin.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Smin.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Vmin.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Hmax.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Smax.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Vmax.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Dilate.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Erode.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Blur.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Amin.set_state(Gtk::STATE_INSENSITIVE);
    bt_HSV_left.set_state(Gtk::STATE_INSENSITIVE);
    bt_HSV_right.set_state(Gtk::STATE_INSENSITIVE);
  }
  else {
    isHSV = true;
    bt_GMM_save.set_state(Gtk::STATE_INSENSITIVE);
    bt_GMM_load.set_state(Gtk::STATE_INSENSITIVE);
    bt_collectSamples.set_state(Gtk::STATE_INSENSITIVE);
    bt_popSample.set_state(Gtk::STATE_INSENSITIVE);
    bt_clearSamples.set_state(Gtk::STATE_INSENSITIVE);
    bt_trainGMM.set_state(Gtk::STATE_INSENSITIVE);
    bt_GMM_match.set_state(Gtk::STATE_INSENSITIVE);
    bt_GMM_done.set_state(Gtk::STATE_INSENSITIVE);
    cb_gaussianColor.set_state(Gtk::STATE_INSENSITIVE);
    cb_realColor.set_state(Gtk::STATE_INSENSITIVE);
    HScale_clusters.set_state(Gtk::STATE_INSENSITIVE);
    HScale_closing.set_state(Gtk::STATE_INSENSITIVE);
    HScale_opening.set_state(Gtk::STATE_INSENSITIVE);
    rb_GMM_original.set_state(Gtk::STATE_INSENSITIVE);
    rb_GMM_gaussians.set_state(Gtk::STATE_INSENSITIVE);
    rb_GMM_final.set_state(Gtk::STATE_INSENSITIVE);
    rb_GMM_threshold.set_state(Gtk::STATE_INSENSITIVE);
    bt_GMM_left.set_state(Gtk::STATE_INSENSITIVE);
    bt_GMM_right.set_state(Gtk::STATE_INSENSITIVE);
    bt_HSV_calib.set_state(Gtk::STATE_NORMAL);
    if (bt_HSV_calib.get_active()) {
      HScale_Hmin.set_state(Gtk::STATE_NORMAL);
      HScale_Smin.set_state(Gtk::STATE_NORMAL);
      HScale_Vmin.set_state(Gtk::STATE_NORMAL);
      HScale_Hmax.set_state(Gtk::STATE_NORMAL);
      HScale_Smax.set_state(Gtk::STATE_NORMAL);
      HScale_Vmax.set_state(Gtk::STATE_NORMAL);
      HScale_Dilate.set_state(Gtk::STATE_NORMAL);
      HScale_Erode.set_state(Gtk::STATE_NORMAL);
      HScale_Blur.set_state(Gtk::STATE_NORMAL);
      HScale_Amin.set_state(Gtk::STATE_NORMAL);
      bt_HSV_left.set_state(Gtk::STATE_NORMAL);
      bt_HSV_right.set_state(Gtk::STATE_NORMAL);
    }
  }
}

void VisionGUI::__create_frm_gmm() {
  Gtk::VBox * vbox;
  Gtk::Grid * grid;
  Gtk::Frame * frame;
  Gtk::Label *label;
  Gtk::HBox * hbox;

  vbox = new Gtk::VBox();
  grid = new Gtk::Grid();
  frame = new Gtk::Frame();

  pack_start(*frame, false, false, 5);

  frame->add(*vbox);
  vbox->set_halign(Gtk::ALIGN_CENTER);
  vbox->set_valign(Gtk::ALIGN_CENTER);

  frame->set_label("GMM Calibration");

  hbox = new Gtk::HBox();
  vbox->pack_start(*hbox, false, true, 5);
  hbox->set_halign(Gtk::ALIGN_CENTER);
  hbox->set_valign(Gtk::ALIGN_CENTER);

  frame = new Gtk::Frame();
  grid = new Gtk::Grid();
  grid->set_margin_left(10);
  grid->set_margin_right(10);
  grid->set_margin_top(5);
  grid->set_margin_bottom(5);
  grid->set_column_spacing(5);
  grid->set_column_homogeneous(true);
  frame->set_label("Samples");
  frame->add(*grid);
  hbox->pack_start(*frame, false, true, 5);

  bt_collectSamples.set_label("Collect");
  grid->attach(bt_collectSamples, 0, 0, 1, 1);
  bt_popSample.set_label("Pop (0)");
  grid->attach(bt_popSample, 1, 0, 1, 1);
  bt_clearSamples.set_label("Reset");
  grid->attach(bt_clearSamples, 2, 0, 1, 1);

  frame = new Gtk::Frame();
  grid = new Gtk::Grid();
  grid->set_margin_left(10);
  grid->set_margin_right(10);
  grid->set_margin_top(5);
  grid->set_margin_bottom(5);
  grid->set_column_spacing(5);
  grid->set_column_homogeneous(true);
  frame->set_label("Save/Load");
  frame->add(*grid);
  hbox->pack_start(*frame, false, true, 5);

  bt_GMM_save.set_label("Save");
  grid->attach(bt_GMM_save, 0, 0, 1, 1);
  bt_GMM_load.set_label("Load");
  grid->attach(bt_GMM_load, 3, 0, 1, 1);

  frame = new Gtk::Frame();
  grid = new Gtk::Grid();
  grid->set_border_width(10);
  grid->set_column_spacing(10);
  grid->set_row_spacing(5);
  grid->set_halign(Gtk::ALIGN_CENTER);
  grid->set_valign(Gtk::ALIGN_CENTER);
  frame->set_label("Training");
  frame->add(*grid);
  vbox->pack_start(*frame, false, true, 5);

  label = new Gtk::Label("Clusters: ");
  grid->attach(*label, 0, 0, 1, 1);
  HScale_clusters.set_digits(0);
  HScale_clusters.set_size_request(200);
  HScale_clusters.set_increments(1,3);
  HScale_clusters.set_range(1,15);
  HScale_clusters.set_value_pos(Gtk::POS_RIGHT);
  HScale_clusters.set_draw_value();
  grid->attach(HScale_clusters, 1, 0, 2, 1);
  bt_trainGMM.set_label("Train GMM");
  grid->attach(bt_trainGMM, 3, 0, 1, 1);

  frame = new Gtk::Frame();
  Gtk::VBox * innerVbox = new Gtk::VBox();
  frame->set_label("Frame");
  frame->add(*innerVbox);
  vbox->pack_start(*frame, false, true, 5);

  hbox = new Gtk::HBox();
  hbox->set_halign(Gtk::ALIGN_CENTER);
  hbox->set_valign(Gtk::ALIGN_CENTER);
  rb_GMM_original.set_label("Original");
  hbox->pack_start(rb_GMM_original, false, true, 5);
  rb_GMM_gaussians.set_label("Gaussians");
  rb_GMM_gaussians.join_group(rb_GMM_original);
  hbox->pack_start(rb_GMM_gaussians, false, true, 5);
  rb_GMM_final.set_label("Final");
  rb_GMM_final.join_group(rb_GMM_original);
  hbox->pack_start(rb_GMM_final, false, true, 5);
  rb_GMM_threshold.set_label("Threshold");
  rb_GMM_threshold.join_group(rb_GMM_original);
  hbox->pack_start(rb_GMM_threshold, false, true, 5);
  innerVbox->pack_start(*hbox, false, true, 5);

  hbox = new Gtk::HBox();
  hbox->set_halign(Gtk::ALIGN_CENTER);
  hbox->set_valign(Gtk::ALIGN_CENTER);
  label = new Gtk::Label("Threshold: ");
  hbox->pack_start(*label, false, true, 5);
  bt_GMM_left.set_label("<");
  hbox->pack_start(bt_GMM_left, false, true, 5);
  lb_threshold.set_text(realColors.at(0));
  hbox->pack_start(lb_threshold, false, true, 5);
  bt_GMM_right.set_label(">");
  hbox->pack_start(bt_GMM_right, false, true, 5);
  innerVbox->pack_start(*hbox, false, true, 5);

  hbox = new Gtk::HBox();
  hbox->set_halign(Gtk::ALIGN_CENTER);
  hbox->set_valign(Gtk::ALIGN_CENTER);
  cb_gaussianColor.append("Select Gaus.:");
  for (int i = 0; i < gmm.getClusters(); i++) {
    cb_gaussianColor.append(gaussianColors.at(i));
  }
  cb_gaussianColor.set_active(0);
  hbox->pack_start(cb_gaussianColor, false, true, 5);
  cb_realColor.append("Select Color:");
  cb_realColor.append("Main");
  cb_realColor.append("Green");
  cb_realColor.append("Pink");
  cb_realColor.append("Ball");
  cb_realColor.append("Opponent");
  cb_realColor.append("Background");
  cb_realColor.set_active(0);
  hbox->pack_start(cb_realColor, false, true, 5);
  bt_GMM_match.set_label("Match!");
  hbox->pack_start(bt_GMM_match, false, true, 5);
  bt_GMM_done.set_label("Done");
  hbox->pack_start(bt_GMM_done, false, true, 5);
  innerVbox->pack_start(*hbox, false, true, 5);

  frame = new Gtk::Frame();
  grid = new Gtk::Grid();
  grid->set_border_width(10);
  grid->set_column_spacing(10);
  grid->set_row_spacing(5);
  grid->set_halign(Gtk::ALIGN_CENTER);
  grid->set_valign(Gtk::ALIGN_CENTER);
  frame->set_label("Pos-Processing");
  frame->add(*grid);
  vbox->pack_start(*frame, false, true, 5);  vbox->set_halign(Gtk::ALIGN_CENTER);

  label = new Gtk::Label("Closing: ");
  grid->attach(*label, 0, 0, 1, 1);
  HScale_closing.set_digits(0);
  HScale_closing.set_size_request(100);
  HScale_closing.set_increments(1,2);
  HScale_closing.set_range(0,5);
  HScale_closing.set_value_pos(Gtk::POS_RIGHT);
  HScale_closing.set_draw_value();
  grid->attach(HScale_closing, 1, 0, 1, 1);
  label = new Gtk::Label("Opening: ");
  grid->attach(*label, 2, 0, 1, 1);
  HScale_opening.set_digits(0);
  HScale_opening.set_size_request(100);
  HScale_opening.set_increments(1,2);
  HScale_opening.set_range(0,5);
  HScale_opening.set_value_pos(Gtk::POS_RIGHT);
  HScale_opening.set_draw_value();
  grid->attach(HScale_opening, 3, 0, 1, 1);

  bt_GMM_save.set_state(Gtk::STATE_INSENSITIVE);
  bt_GMM_load.set_state(Gtk::STATE_INSENSITIVE);
  bt_collectSamples.set_state(Gtk::STATE_INSENSITIVE);
  bt_popSample.set_state(Gtk::STATE_INSENSITIVE);
  bt_clearSamples.set_state(Gtk::STATE_INSENSITIVE);
  bt_trainGMM.set_state(Gtk::STATE_INSENSITIVE);
  bt_GMM_match.set_state(Gtk::STATE_INSENSITIVE);
  bt_GMM_done.set_state(Gtk::STATE_INSENSITIVE);
  cb_gaussianColor.set_state(Gtk::STATE_INSENSITIVE);
  cb_realColor.set_state(Gtk::STATE_INSENSITIVE);
  HScale_clusters.set_state(Gtk::STATE_INSENSITIVE);
  HScale_closing.set_state(Gtk::STATE_INSENSITIVE);
  HScale_opening.set_state(Gtk::STATE_INSENSITIVE);
  rb_GMM_original.set_state(Gtk::STATE_INSENSITIVE);
  rb_GMM_gaussians.set_state(Gtk::STATE_INSENSITIVE);
  rb_GMM_final.set_state(Gtk::STATE_INSENSITIVE);
  rb_GMM_threshold.set_state(Gtk::STATE_INSENSITIVE);
  bt_GMM_left.set_state(Gtk::STATE_INSENSITIVE);
  bt_GMM_right.set_state(Gtk::STATE_INSENSITIVE);

  bt_GMM_save.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_GMM_save_clicked));
  bt_GMM_load.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_GMM_load_clicked));
  bt_collectSamples.signal_pressed().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_collectSamples_pressed));
  bt_popSample.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_popSample_clicked));
  bt_clearSamples.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_clearSamples_clicked));
  bt_trainGMM.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_trainGMM_clicked));
  HScale_clusters.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_clusters_value_changed));
  bt_GMM_match.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_GMM_match_clicked));
  bt_GMM_done.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_GMM_done_clicked));
  rb_GMM_original.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_rb_GMM_frame_clicked));
  rb_GMM_gaussians.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_rb_GMM_frame_clicked));
  rb_GMM_final.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_rb_GMM_frame_clicked));
  rb_GMM_threshold.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_rb_GMM_frame_clicked));
  bt_GMM_left.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_GMM_left_clicked));
  bt_GMM_right.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_GMM_right_clicked));
  HScale_closing.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_closing_value_changed));
  HScale_opening.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_opening_value_changed));
}

bool VisionGUI::getDrawSamples() {
  return samplesEventFlag;
}

bool VisionGUI::getOriginalFrameFlag() {
  return originalFrame_flag;
}

bool VisionGUI::getGaussiansFrameFlag() {
  return gaussiansFrame_flag;
}

bool VisionGUI::getFinalFrameFlag() {
  return finalFrame_flag;
}

bool VisionGUI::getThresholdFrameFlag() {
  return thresholdFrame_flag;
}

void VisionGUI::HScale_closing_value_changed() {
  gmm.setClosingSize(HScale_closing.get_value());
}

void VisionGUI::HScale_opening_value_changed() {
  gmm.setOpeningSize(HScale_opening.get_value());
}

void VisionGUI::__event_bt_GMM_save_clicked() {
  FileChooser saveWindow;

  if (saveWindow.result == Gtk::RESPONSE_OK) gmm.write(saveWindow.fileName);

}

void VisionGUI::__event_bt_GMM_load_clicked() {
  FileChooser loadWindow;

  if (loadWindow.result == Gtk::RESPONSE_OK) {
    if (gmm.read(loadWindow.fileName)) {
      rb_GMM_gaussians.set_active(true);
      rb_GMM_gaussians.clicked();
    }
  }
}

void VisionGUI::quickLoadGMM() {
  gmm.read("autoGMM.json");
}

void VisionGUI::__event_bt_GMM_left_clicked() {
  if (colorIndex-1 < 0) colorIndex = realColors.size()-1;
  else colorIndex--;

  lb_threshold.set_text(realColors.at(colorIndex));
}

void VisionGUI::__event_bt_GMM_right_clicked() {
  if (colorIndex+1 >= realColors.size()) colorIndex = 0;
  else colorIndex++;

  lb_threshold.set_text(realColors.at(colorIndex));
}

void VisionGUI::__event_rb_GMM_frame_clicked() {
  if (rb_GMM_original.get_active()) {
    originalFrame_flag = true;
    gaussiansFrame_flag = false;
    finalFrame_flag = false;
    thresholdFrame_flag = false;
  } else if (rb_GMM_gaussians.get_active()) {
    originalFrame_flag = false;
    gaussiansFrame_flag = true;
    finalFrame_flag = false;
    thresholdFrame_flag = false;
  } else if (rb_GMM_final.get_active()){
    originalFrame_flag = false;
    gaussiansFrame_flag = false;
    finalFrame_flag = true;
    thresholdFrame_flag = false;
  } else {
    originalFrame_flag = false;
    gaussiansFrame_flag = false;
    finalFrame_flag = false;
    thresholdFrame_flag = true;
  }
}

void VisionGUI::__event_bt_GMM_match_clicked() {
  int gaussian = cb_gaussianColor.get_active_row_number();
  int color = cb_realColor.get_active_row_number();
  if (gaussian >= 0 && color >= 0) gmm.setMatchColor(gaussian-1, color-1);
}

void VisionGUI::__event_bt_GMM_done_clicked() {
  gmm.setDone();
  rb_GMM_threshold.set_state(Gtk::STATE_NORMAL);
}

void VisionGUI::HScale_clusters_value_changed() {
  gmm.setClusters(HScale_clusters.get_value());

  cb_gaussianColor.remove_all();
  cb_gaussianColor.append("Select Gaussian:");
  cb_gaussianColor.set_active(0);
  for (int i = 0; i < gmm.getClusters(); i++) {
    cb_gaussianColor.append(gaussianColors.at(i));
  }
}

void VisionGUI::__event_bt_trainGMM_clicked() {
  int res = gmm.train();
  if (res == 0) {
    rb_GMM_gaussians.set_active(true);
    rb_GMM_gaussians.clicked();
    bt_collectSamples.set_active(false);
    bt_clearSamples.set_state(Gtk::STATE_INSENSITIVE);
    bt_popSample.set_state(Gtk::STATE_INSENSITIVE);
    bt_GMM_save.set_state(Gtk::STATE_NORMAL);
    samplesEventFlag = false;
  }
  else HScale_clusters.set_state(Gtk::STATE_NORMAL);
}

void VisionGUI::__event_bt_collectSamples_pressed() {
  if (samplesEventFlag) {
    bt_popSample.set_state(Gtk::STATE_INSENSITIVE);
    bt_clearSamples.set_state(Gtk::STATE_INSENSITIVE);
  } else {
    bt_popSample.set_state(Gtk::STATE_NORMAL);
    bt_clearSamples.set_state(Gtk::STATE_NORMAL);
  }

  samplesEventFlag = !samplesEventFlag;
}

void VisionGUI::__event_bt_popSample_clicked() {
  gmm.popSample();
  decrementSamples();
  std::cout << "GMM sample popped. Total left: " << gmm.getSamplesSize() << std::endl;
}

void VisionGUI::__event_bt_clearSamples_clicked() {
  gmm.clearSamples();
  totalSamples = 0;
  bt_popSample.set_label("Pop (0)");
  std::cout << "GMM samples cleared." << std::endl;
}

bool VisionGUI::getSamplesEventFlag() {
  return samplesEventFlag;
}

void VisionGUI::__create_frm_capture() {
  Gtk::VBox * vbox;
  Gtk::Grid * grid;
  Gtk::Label * label;
  Gtk::Frame * frame;

  vbox = new Gtk::VBox();
  grid = new Gtk::Grid();
  frame = new Gtk::Frame();

  pack_start(*frame, false, false, 5);

  frame->add(*vbox);
  vbox->pack_start(*grid, false, true, 5);
  vbox->set_halign(Gtk::ALIGN_CENTER);
  vbox->set_valign(Gtk::ALIGN_CENTER);

  frame->set_label("Video/Image Capture");

  grid->set_border_width(10);
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
  if (vision->isRecording()) {
    vision->finishVideo();
    bt_record_video.set_label("REC");
    en_video_name.set_text("");
    en_video_name.set_state(Gtk::STATE_NORMAL);
  } else {
    std::string name = en_video_name.get_text();

    bt_record_video.set_label("Stop");
    en_video_name.set_state(Gtk::STATE_INSENSITIVE);
    if (name.empty()) {
      vision->startNewVideo(std::to_string(vidIndex++));
      en_video_name.set_text(std::to_string(vidIndex));
    } else {
      vision->startNewVideo(name);
    }
  }
}

void VisionGUI::bt_save_picture_clicked() {
  std::string name = en_picture_name.get_text();
  if (name.empty()) {
    vision->savePicture(std::to_string(picIndex++));
  } else {
    vision->savePicture(name);
  }
  en_picture_name.set_text("");
}

void VisionGUI::__create_frm_hsv() {
  Gtk::VBox * vbox;
  Gtk::Grid * grid;
  Gtk::Label * label;
  Gtk::Frame * frame;

  vbox = new Gtk::VBox();
  grid = new Gtk::Grid();
  frame = new Gtk::Frame();

  pack_start(*frame, false, false, 5);

  frame->add(*vbox);
  vbox->pack_start(*grid, false, true, 5);
  vbox->set_halign(Gtk::ALIGN_CENTER);
  vbox->set_valign(Gtk::ALIGN_CENTER);

  frame->set_label("HSV Calibration");

  grid->set_border_width(10);
  grid->set_column_spacing(15);
  grid->set_row_spacing(5);
  grid->set_column_homogeneous(true);

  bt_HSV_calib.set_label("HSV Calib.");
  grid->attach(bt_HSV_calib, 0, 0, 2, 1);

  bt_HSV_left.set_label(" < ");
  grid->attach(bt_HSV_left, 3, 0, 1, 1);
  HSV_label.set_text("Main");
  grid->attach(HSV_label, 4, 0, 1, 1);
  bt_HSV_right.set_label(" > ");
  grid->attach(bt_HSV_right, 5, 0, 1, 1);

  label = new Gtk::Label("Hmin:");
  label->set_alignment(1.0, 1.0);
  grid->attach(*label, 0, 1, 1, 1);

  HScale_Hmin.set_digits(0);
  HScale_Hmin.set_increments(1,1);
  HScale_Hmin.set_range(0,255);
  HScale_Hmin.set_value_pos(Gtk::POS_TOP);
  HScale_Hmin.set_draw_value();
  grid->attach(HScale_Hmin, 1, 1, 2, 1);

  label = new Gtk::Label("Hmax:");
  label->set_alignment(1.0, 1.0);
  grid->attach(*label, 3, 1, 1, 1);

  HScale_Hmax.set_digits(0);
  HScale_Hmax.set_increments(1,1);
  HScale_Hmax.set_range(0,180);
  HScale_Hmax.set_value_pos(Gtk::POS_TOP);
  HScale_Hmax.set_draw_value();
  grid->attach(HScale_Hmax, 4, 1, 2, 1);

  label = new Gtk::Label("Smin:");
  label->set_alignment(1.0, 1.0);
   grid->attach(*label, 0, 2, 1, 1);

  HScale_Smin.set_digits(0);
  HScale_Smin.set_increments(1,1);
  HScale_Smin.set_range(0,255);
  HScale_Smin.set_value_pos(Gtk::POS_TOP);
  HScale_Smin.set_draw_value();
  grid->attach(HScale_Smin, 1, 2, 2, 1);

  label = new Gtk::Label("Smax:");
  label->set_alignment(1.0, 1.0);
  grid->attach(*label, 3, 2, 1, 1);

  HScale_Smax.set_digits(0);
  HScale_Smax.set_increments(1,1);
  HScale_Smax.set_range(0,255);
  HScale_Smax.set_value_pos(Gtk::POS_TOP);
  HScale_Smax.set_draw_value();

  grid->attach(HScale_Smax,4, 2, 2, 1);

  label = new Gtk::Label("Vmin:");
  label->set_alignment(1.0, 1.0);
  grid->attach(*label, 0, 3, 1, 1);

  HScale_Vmin.set_digits(0);
  HScale_Vmin.set_increments(1,1);
  HScale_Vmin.set_range(0,255);
  HScale_Vmin.set_value_pos(Gtk::POS_TOP);
  HScale_Vmin.set_draw_value();
  grid->attach(HScale_Vmin, 1, 3, 2, 1);

  label = new Gtk::Label("Vmax:");
  label->set_alignment(1.0, 1.0);
  grid->attach(*label, 3, 3, 1, 1);

  HScale_Vmax.set_digits(0);
  HScale_Vmax.set_increments(1,1);
  HScale_Vmax.set_range(0,255);
  HScale_Vmax.set_value_pos(Gtk::POS_TOP);
  HScale_Vmax.set_draw_value();

  grid->attach(HScale_Vmax, 4, 3, 2, 1);

  label = new Gtk::Label("Erode:");
  label->set_alignment(1.0, 1.0);
  grid->attach(*label, 0,4, 1, 1);

  HScale_Erode.set_digits(0);
  HScale_Erode.set_increments(1,1);
  HScale_Erode.set_range(0,50);
  HScale_Erode.set_value_pos(Gtk::POS_TOP);
  HScale_Erode.set_draw_value();
  grid->attach(HScale_Erode, 1, 4, 2, 1);

  label = new Gtk::Label("Dilate:");
  label->set_alignment(1.0, 1.0);
  grid->attach(*label, 3, 4, 1, 1);

  HScale_Dilate.set_digits(0);
  HScale_Dilate.set_increments(1,1);
  HScale_Dilate.set_range(0,50);
  HScale_Dilate.set_value_pos(Gtk::POS_TOP);
  HScale_Dilate.set_draw_value();
  grid->attach(HScale_Dilate, 4, 4, 2, 1);

  label = new Gtk::Label("Blur:");
  label->set_alignment(1.0, 1.0);
  grid->attach(*label, 0, 5, 1, 1);

  HScale_Blur.set_digits(0);
  HScale_Blur.set_increments(2,2);
  HScale_Blur.set_range(3,9);
  HScale_Blur.set_value_pos(Gtk::POS_TOP);
  HScale_Blur.set_draw_value();
  grid->attach(HScale_Blur, 1, 5, 2, 1);

  label = new Gtk::Label("Amin:");
  label->set_alignment(1.0, 1.0);
  grid->attach(*label, 3, 5, 1, 1);

  HScale_Amin.set_digits(0);
  HScale_Amin.set_increments(1,1);
  HScale_Amin.set_range(0,1000);
  HScale_Amin.set_value_pos(Gtk::POS_TOP);
  HScale_Amin.set_draw_value();

  grid->attach(HScale_Amin, 4, 5, 2, 1);

  bt_HSV_calib.set_state(Gtk::STATE_INSENSITIVE);
  HScale_Hmin.set_state(Gtk::STATE_INSENSITIVE);
  HScale_Smin.set_state(Gtk::STATE_INSENSITIVE);
  HScale_Vmin.set_state(Gtk::STATE_INSENSITIVE);
  HScale_Hmax.set_state(Gtk::STATE_INSENSITIVE);
  HScale_Smax.set_state(Gtk::STATE_INSENSITIVE);
  HScale_Vmax.set_state(Gtk::STATE_INSENSITIVE);
  HScale_Dilate.set_state(Gtk::STATE_INSENSITIVE);
  HScale_Erode.set_state(Gtk::STATE_INSENSITIVE);
  HScale_Blur.set_state(Gtk::STATE_INSENSITIVE);
  HScale_Amin.set_state(Gtk::STATE_INSENSITIVE);
  bt_HSV_left.set_state(Gtk::STATE_INSENSITIVE);
  bt_HSV_right.set_state(Gtk::STATE_INSENSITIVE);

  bt_HSV_calib.signal_pressed().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_HSV_calib_pressed));
  bt_HSV_right.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_right_HSV_calib_clicked));
  bt_HSV_left.signal_clicked().connect(sigc::mem_fun(*this, &VisionGUI::__event_bt_left_HSV_calib_clicked));

  HScale_Hmin.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Hmin_value_changed));
  HScale_Hmax.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Hmax_value_changed));
  HScale_Smin.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Smin_value_changed));
  HScale_Smax.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Smax_value_changed));
  HScale_Vmin.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Vmin_value_changed));
  HScale_Vmax.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Vmax_value_changed));
  HScale_Erode.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Erode_value_changed));
  HScale_Dilate.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Dilate_value_changed));
  HScale_Blur.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Blur_value_changed));
  HScale_Amin.signal_value_changed().connect(sigc::mem_fun(*this, &VisionGUI::HScale_Amin_value_changed));
}

void VisionGUI::HScale_Hmin_value_changed() {
  vision->setHue(Img_id, 0, HScale_Hmin.get_value());
}

void VisionGUI::HScale_Smin_value_changed() {
  vision->setSaturation(Img_id, 0, HScale_Smin.get_value());
}

void VisionGUI::HScale_Vmin_value_changed() {
  vision->setValue(Img_id, 0, HScale_Vmin.get_value());
}

void VisionGUI::HScale_Hmax_value_changed() {
  vision->setHue(Img_id, 1, HScale_Hmax.get_value());
}

void VisionGUI::HScale_Smax_value_changed() {
  vision->setSaturation(Img_id, 1, HScale_Smax.get_value());
}

void VisionGUI::HScale_Vmax_value_changed() {
  vision->setValue(Img_id, 1, HScale_Vmax.get_value());
}

void VisionGUI::HScale_Amin_value_changed() {
  vision->setAmin(Img_id, HScale_Amin.get_value());
}

void VisionGUI::HScale_Dilate_value_changed() {

 if(HScale_Dilate.get_value()<0){
   vision->setDilate(Img_id, 0);
 }else{
   vision->setDilate(Img_id, HScale_Dilate.get_value());
 }
  //std::cout<<"=================================================="<<D[Img_id]<<std::endl;

}

void VisionGUI::HScale_Erode_value_changed() {


 if(HScale_Erode.get_value()<0){
   vision->setErode(Img_id, 0);
 }else{
   vision->setErode(Img_id, HScale_Erode.get_value());
 }
  //std::cout<<"=================================================="<<E[Img_id]<<std::endl;

}

void VisionGUI::HScale_Blur_value_changed() {

 if(HScale_Blur.get_value()<3){
   vision->setBlur(Img_id, 3);
 }
 else if((int) HScale_Blur.get_value() % 2 == 0){
   vision->setBlur(Img_id, (int) HScale_Blur.get_value()+1);
 }
 else{
   vision->setBlur(Img_id, (int)HScale_Blur.get_value());
 }
  //std::cout<<"====Blur: "<<B[Img_id]<<" id color: "<<Img_id<<std::endl;

}

void VisionGUI::__event_bt_HSV_calib_pressed() {

  if (HSV_calib_event_flag) {
    HSV_calib_event_flag=false;
    // VisionGUI::__event_auto_save();
    HScale_Hmin.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Smin.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Vmin.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Hmax.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Smax.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Vmax.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Dilate.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Erode.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Blur.set_state(Gtk::STATE_INSENSITIVE);
    HScale_Amin.set_state(Gtk::STATE_INSENSITIVE);
    bt_HSV_right.set_state(Gtk::STATE_INSENSITIVE);
    bt_HSV_left.set_state(Gtk::STATE_INSENSITIVE);
  } else {
    HSV_calib_event_flag=true;
    HScale_Hmin.set_state(Gtk::STATE_ACTIVE);
    HScale_Smin.set_state(Gtk::STATE_ACTIVE);
    HScale_Vmin.set_state(Gtk::STATE_ACTIVE);
    HScale_Hmax.set_state(Gtk::STATE_ACTIVE);
    HScale_Smax.set_state(Gtk::STATE_ACTIVE);
    HScale_Vmax.set_state(Gtk::STATE_ACTIVE);
    HScale_Dilate.set_state(Gtk::STATE_ACTIVE);
    HScale_Erode.set_state(Gtk::STATE_ACTIVE);
    HScale_Blur.set_state(Gtk::STATE_ACTIVE);
    HScale_Amin.set_state(Gtk::STATE_ACTIVE);
    bt_HSV_right.set_state(Gtk::STATE_ACTIVE);
    bt_HSV_left.set_state(Gtk::STATE_ACTIVE);
  }
}


void VisionGUI::__event_bt_right_HSV_calib_clicked() {

  Img_id=Img_id+1;

  if(Img_id>4) Img_id = 0;
  HScale_Hmin.set_value(vision->getHue(Img_id, 0));
  HScale_Hmax.set_value(vision->getHue(Img_id, 1));

  HScale_Smin.set_value(vision->getSaturation(Img_id, 0));
  HScale_Smax.set_value(vision->getSaturation(Img_id, 1));

  HScale_Vmin.set_value(vision->getValue(Img_id, 0));
  HScale_Vmax.set_value(vision->getValue(Img_id, 1));

  HScale_Dilate.set_value(vision->getDilate(Img_id));
  HScale_Erode.set_value(vision->getErode(Img_id));

  HScale_Blur.set_value(vision->getBlur(Img_id));
  HScale_Amin.set_value(vision->getAmin(Img_id));
  switch(Img_id) {
  case 0:
      HSV_label.set_text("Main");
      break;
  case 1:
      HSV_label.set_text("Green");
      break;
  case 2:
      HSV_label.set_text("Pink");
      break;
  case 3:
      HSV_label.set_text("Ball");
      break;
  case 4:
      HSV_label.set_text("Opp.");
      break;
  }
}

void VisionGUI::__event_bt_left_HSV_calib_clicked() {

  Img_id=Img_id-1;
  if(Img_id<0) Img_id = 4;
  HScale_Hmin.set_value(vision->getHue(Img_id, 0));
  HScale_Hmax.set_value(vision->getHue(Img_id, 1));

  HScale_Smin.set_value(vision->getSaturation(Img_id, 0));
  HScale_Smax.set_value(vision->getSaturation(Img_id, 1));

  HScale_Vmin.set_value(vision->getValue(Img_id, 0));
  HScale_Vmax.set_value(vision->getValue(Img_id, 1));

  HScale_Dilate.set_value(vision->getDilate(Img_id));
  HScale_Erode.set_value(vision->getErode(Img_id));

  HScale_Blur.set_value(vision->getBlur(Img_id));
  HScale_Amin.set_value(vision->getAmin(Img_id));
  switch(Img_id) {
  case 0:
      HSV_label.set_text("Main");
      break;
  case 1:
      HSV_label.set_text("Green");
      break;
  case 2:
      HSV_label.set_text("Pink");
      break;
  case 3:
      HSV_label.set_text("Ball");
      break;
  case 4:
      HSV_label.set_text("Opp.");
      break;
  }
}

// void VisionGUI::__event_auto_save()
// {
//   std::cout << "AUTO SAVE" << std::endl;
//
//   if(!VisionGUI::__core_save("autosave.txt"))
//   {
//     std::cout<<"Error: could not auto save."<<std::endl;
//   }
// }

void VisionGUI::incrementSamples() {
  totalSamples++;
  std::string text = "Pop (" + std::to_string(totalSamples) + ")";
  bt_popSample.set_label(text);
}

void VisionGUI::decrementSamples() {
  if (totalSamples-1 < 0) totalSamples = 0;
  else totalSamples--;
  std::string text = "Pop (" + std::to_string(totalSamples) + ")";
  bt_popSample.set_label(text);
}

void VisionGUI::init_calib_params()
{
  // Inicializar variáveis de calibração
  int H[5][2] = { {0,180}, {0,180}, {0,180}, {0,180}, {0,180} };
  int S[5][2] = { {0, 255}, {0, 255}, {0, 255}, {0, 255}, {0, 255} };
  int V[5][2] = { {0, 255}, {0, 255}, {0, 255}, {0, 255}, {0, 255} };
  int B[5] {3, 3, 3, 3, 3};
  int D[5] = {0, 0, 0, 0, 0};
  int E[5] = {0, 0, 0, 0, 0};
  int Amin[5] = {500, 500, 500, 500, 500};

  // Configurar os valores iniciais de calibração
  vision->setCalibParams(H, S, V, Amin, E, D, B);

  // Corrigir os valores mostrados na interface
  HScale_Hmax.set_value(H[0][1]);
  HScale_Smax.set_value(S[0][1]);
  HScale_Vmax.set_value(V[0][1]);
  HScale_Amin.set_value(Amin[0]);
}

void VisionGUI::setFrameSize(int inWidth, int inHeight) {
  vision->setFrameSize(inWidth, inHeight);
}

int VisionGUI::getFrameHeight() {
  return vision->getFrameHeight();
}

int VisionGUI::getFrameWidth() {
  return vision->getFrameWidth();
}

int VisionGUI::getGMMColorIndex() {
  return colorIndex;
}

bool VisionGUI::getIsHSV() {
  return isHSV;
}

VisionGUI::VisionGUI() :
  HSV_calib_event_flag(false), Img_id(0),
  vidIndex(0), picIndex(0), samplesEventFlag(false),
  originalFrame_flag(true), totalSamples(0),
  gaussiansFrame_flag(false), finalFrame_flag(false),
  thresholdFrame_flag(false), colorIndex(0), isHSV(true) {

  vision = new Vision(640, 480);

  __create_frm_calib_mode();
  __create_frm_capture();
  __create_frm_hsv();
  __create_frm_gmm();

  init_calib_params();
}

VisionGUI::~VisionGUI() {
  if (vision->isRecording()) vision->finishVideo();
}
