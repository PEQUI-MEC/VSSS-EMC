#ifndef GMM_HPP_
#define GMM_HPP_

#include "opencv2/opencv.hpp"
#include "opencv2/ml.hpp"
#include <iostream>     // std::cout
#include <math.h>

class GMM
{

private:
  cv::Mat inFrame, gaussiansFrame, finalFrame;
  std::vector<cv::Point> samplePoints;
  std::vector<cv::Mat> samples;

  int clusters;
  std::vector<cv::Mat> covs;
  cv::Mat means, weights;
  cv::Ptr<cv::ml::EM> em;
  bool isTrained;

  const std::vector<cv::Vec3b> colors = {
		{22, 255, 239}, // yellow
		{0, 255, 0}, // green
		{195, 0, 255}, // pink
		{46, 83, 247}, // orange
		{137, 56, 39}, // blue
		{0, 0, 0}, // black
		{255, 255, 255}, // white
		{0, 0, 255}, // red
		{102, 12, 75}, // purple
		{0, 0, 86}, // brown
		{179, 179, 179}, // silver
		{255, 255, 0}, // cyan
		{19, 81, 2}, // dark green
		{230, 169, 252}, // baby pink
		{89, 89, 89} // dark grey
  };

  cv::Mat classify();
  cv::Mat predict();
  cv::Mat crop(cv::Point p1, cv::Point p2);
  cv::Mat formatSamplesForEM();
  cv::Mat formatFrameForEM();

public:
  GMM();
  ~GMM();

  void setFrame(cv::Mat frame);

  void pushSample(int points[2][2]);
  void popSample();
  void clearSamples();
  int getSamplesSize();
  std::vector<cv::Point> getSamplePoints();

  void setClusters(int k);
  int getClusters();
  cv::Mat getGaussiansFrame();
  cv::Mat getFinalFrame();
  bool getIsTrained();

  int train();
  void run(cv::Mat frame);






};

#endif /* VISION_HPP_ */
