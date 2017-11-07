#ifndef VISION_HPP_
#define VISION_HPP_

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/tracking.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "../../pack-capture-gui/capture-gui/Robot.hpp"
#include <iostream>     // std::cout
#include "tag.hpp"

class Vision
{

private:

  // Constants
  static const int MAIN = 0;
  static const int GREEN = 1;
  // static const int PINK = 2;
  static const int BALL = 2;
  static const int ADV = 3;
  static const int MAX_ADV = 3;
  static const int TOTAL_COLORS = 4;
  static const int MIN = 0;
  static const int MAX = 1;

  // Frames
  cv::Mat in_frame, hsv_frame;
  std::vector<cv::Mat>threshold_frame;

  // Robots
  std::vector<Robot> robot_list;
  cv::Point advRobots[MAX_ADV];

  // Ball
  cv::Point ball;

  // TAGS
  std::vector<std::vector<Tag>> tags;

  // HSV Calibration Parameters
  int hue[5][2];
  int saturation[5][2];
  int value[5][2];
  int dilate[5];
  int erode[5];
  int blur[5];
  int areaMin[5];

  // image size
  int width;
  int height;

  // record video flag
  bool bOnAir;

  // video
  cv::VideoWriter video;

  // threads
  boost::thread_group threshold_threads;

  void preProcessing();
  void posProcessing(int color);
  void segmentAndSearch(int color);
  void searchTags(int color);
  void searchGMMTags(std::vector<cv::Mat> thresholds);
  void findTags();
  // void findElements();
  void pick_a_tag();
  int inSphere(Robot * robot, std::vector<Tag> * tempTags, cv::Point secondary);

public:
  Vision(int w, int h);
  ~Vision();

  void run(cv::Mat raw_frame);
  void runGMM(std::vector<cv::Mat> thresholds);
  void setCalibParams(int H[5][2], int S[5][2], int V[5][2], int Amin[5], int E[5], int D[5], int B[5]);
  double calcDistance(cv::Point p1, cv::Point p2);

  void startNewVideo(std::string videoName);
  bool recordToVideo();
  bool finishVideo();
  bool isRecording();
  void savePicture(std::string in_name);

  void switchMainWithAdv();

  cv::Point getBall();
  Robot getRobot(int index);
  cv::Point getRobotPos(int index);
  cv::Point getAdvRobot(int index);
  cv::Point* getAllAdvRobots();

  int getRobotListSize();
  int getAdvListSize();
  cv::Mat getThreshold(int index);
  void setAllThresholds(cv::Mat input);

  int getHue(int index0, int index1);
  int getSaturation(int index0, int index1);
  int getValue(int index0, int index1);
  int getErode(int index);
  int getDilate(int index);
  int getBlur(int index);
  int getAmin(int index);

  void setFrameSize(int inWidth, int inHeight);
  int getFrameHeight();
  int getFrameWidth();

  void setHue(int index0, int index1, int inValue);
  void setSaturation(int index0, int index1, int inValue);
  void setValue(int index0, int index1, int inValue);
  void setErode(int index, int inValue);
  void setDilate(int index, int inValue);
  void setBlur(int index, int inValue);
  void setAmin(int index, int inValue);


};

#endif /* VISION_HPP_ */
