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
  // Frames
  cv::Mat inFrame, gaussiansFrame, finalFrame, preThreshold;
  std::vector<cv::Mat>threshold_frame;
  std::vector<cv::Point> samplePoints;
  std::vector<cv::Mat> samples;

  // Expectation Maximization
  int clusters;
  std::vector<cv::Mat> covs;
  cv::Mat means, weights;
  cv::Ptr<cv::ml::EM> em;
  bool isTrained;
  bool isDone;

  // Pos-Processing
  int closingSize, openingSize;

  // Multi-thread
  const static int TOTAL_COLORS = 5;
  const static int TOTAL_THREADS = 8;
  cv::Mat partialFrames[TOTAL_THREADS];
  cv::Mat partialPredicts[TOTAL_THREADS];
  boost::thread_group threads;

  // Convert Type
  const static int HSV_TYPE = 0;
  const static int CIELAB_TYPE = 1;
  int convertType;

  // Colors
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
  std::vector<int> matchColor = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  void classify(int index);
  void paint();
  cv::Mat predict(int index);
  cv::Mat crop(cv::Point p1, cv::Point p2);
  cv::Mat formatSamplesForEM();
  cv::Mat formatFrameForEM(int index);
  void setAllThresholds();
  void posProcessing();

public:
  GMM();
  ~GMM();

  // Expectation Maximization
  void pushSample(int points[2][2]);
  void popSample();
  void clearSamples();
  bool read(std::string fileName);
  bool write(std::string fileName);
  int train();
  void run(cv::Mat frame);

  // GET
  int getSamplesSize();
  std::vector<cv::Point> getSamplePoints();
  int getClusters();
  cv::Mat getGaussiansFrame();
  cv::Mat getFinalFrame();
  cv::Mat getPreThresholdFrame();
  bool getIsTrained();
  bool getDoneFlag();
  cv::Mat getThresholdFrame(int color);
  std::vector<cv::Mat> getAllThresholds();
  int getConvertType();
  int getClosingSize();
  int getOpeningSize();

  // SET
  void setFrame(cv::Mat frame);
  void setClusters(int k);
  void setDone();
  void setMatchColor(int gaussian, int color);
  void setOpeningSize(int value);
  void setClosingSize(int value);
  void setConvertType(int value);

};

#endif /* VISION_HPP_ */
