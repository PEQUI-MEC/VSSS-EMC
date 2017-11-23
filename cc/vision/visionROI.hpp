#ifndef VISIONROI_HPP_
#define VISIONROI_HPP_

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/tracking.hpp>

class VisionROI: public cv::Mat {

private:
  int mSize;
  int mId;
  cv::Point mPosBegin, mPosEnd;
  cv::Point mPosCenter;
  bool mIsLost;
  int mWidth, mHeight;

  bool checkForBoundaries();

public:

  void setSize(cv::Size size);
  void setSize(int width, int height);
  void setId(int id);
  void setPosition(cv::Point pos);
  void setPosition(int x, int y);
  void setCenter(cv::Point pos);
  void setCenter(int x, int y);
  void setIsLost(bool lost);

  int getSize();
  int getId();
  cv::Point getPosition();
  bool getIsLost();
  int getX();
  int getY();
  cv::Point getCenter();
  int getCenterX();
  int getCenterY();
  cv::Point getEnd();

  VisionROI(int width, int height, int size, int id);
  ~VisionROI();

};


#endif /* VISIONROI_HPP_ */
