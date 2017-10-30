#ifndef GMM_HPP_
#define GMM_HPP_

#include "opencv2/opencv.hpp"
#include "opencv2/ml.hpp"
#include <iostream>     // std::cout
#include <math.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

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

  const static int TOTAL_THREADS = 8;
  cv::Mat partialFrames[TOTAL_THREADS];
  boost::thread_group threads;

  const std::vector<cv::Vec3b> colors = {
		{239, 255, 22}, // yellow
		{0, 255, 0}, // green
		{255, 0, 195}, // pink
		{247, 83, 46}, // orange
		{39, 56, 137}, // blue
		{0, 0, 0}, // black
		{255, 255, 255}, // white
		{255, 0, 0}, // red
		{75, 12, 102}, // purple
		{86, 0, 0}, // brown
		{179, 179, 179}, // silver
		{0, 255, 255}, // cyan
		{2, 81, 19}, // dark green
		{252, 169, 230}, // baby pink
		{89, 89, 89} // dark grey
  };

  void classify(int index);
  cv::Mat predict(int index);
  cv::Mat crop(cv::Point p1, cv::Point p2);
  cv::Mat formatSamplesForEM();
  cv::Mat formatFrameForEM(int index);

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
