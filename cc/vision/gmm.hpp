#ifndef GMM_HPP_
#define GMM_HPP_

#include "opencv2/opencv.hpp"
#include "opencv2/ml.hpp"
#include <iostream>     // std::cout
#include <math.h>

class GMM
{

private:
  cv::Mat inFrame;
  std::vector<cv::Point> samplePoints;
  std::vector<cv::Mat> samples;



public:
  GMM();
  ~GMM();

  void setFrame(cv::Mat frame);
  cv::Mat crop(cv::Point p1, cv::Point p2);

  void pushSample(int points[2][2]);
  void popSample();
  void clearSamples();
  int getSamplesSize();
  std::vector<cv::Point> getSamplePoints();




};

#endif /* VISION_HPP_ */
