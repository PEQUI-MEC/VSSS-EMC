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

class Vision
{

public:

  // Constants
  static const int MAIN = 0;
  static const int GREEN = 1;
  static const int PINK = 2;
  static const int BALL = 3;
  static const int ADV = 4;
  static const int MAX_ADV = 3;
  static const int TOTAL_COLORS = 5;
  static const int MIN = 0;
  static const int MAX = 1;

  // Frames
  cv::Mat in_frame;
  std::vector<cv::Mat>threshold_frame;

  // Robots
  std::vector<Robot> robot_list;
  cv::Point advRobots[MAX_ADV];

  // Ball
  cv::Point ball;

  // TAGS
  std::vector<std::vector<cv::Point>> tags;
  std::vector<std::vector<double>> tagsArea;

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

  // video
  cv::VideoWriter video;
  int frameCounter;

  // threads
  boost::thread_group threshold_threads;

  void preProcessing();
  void posProcessing(int color);
  void segmentAndSearch(int color);
  void searchTags(int color);
  void findTags();
  void findElements();

public:
  Vision(int w, int h);
  ~Vision();

  void run(cv::Mat raw_frame);
  void setCalibParams(int H[5][2], int S[5][2], int V[5][2], int Amin[5], int E[5], int D[5], int B[5]);
  double calcDistance(cv::Point p1, cv::Point p2);

  void startNewVideo(std::string videoName);
  bool recordToVideo(cv::Mat frame);
  bool finishVideo();
  void savePicture(std::string in_name, cv::Mat in_frame);

  cv::Point getBall();
  Robot getRobot(int index);
  cv::Point getRobotPos(int index);
  cv::Point getAdvRobot(int index);
  int getRobotListSize();
  int getAdvListSize();
  cv::Mat getThreshold(int index);

};

#endif /* VISION_HPP_ */
