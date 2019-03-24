#ifndef VISIONROI_HPP_
#define VISIONROI_HPP_

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/video/tracking.hpp>

class VisionROI : public cv::Mat {

	private:
		bool mIsLost;
		cv::Point mPosBegin;
		cv::Point mPosEnd;
		int mSize;
		int mId;

		cv::Point mPosCenter;

		int mWidth, mHeight;

		bool checkForBoundaries();

	public:

		void setPosition(int x, int y);
		void setCenter(cv::Point pos);
		void setIsLost(bool lost);

		int getSize();
		cv::Point getPosition();
		bool getIsLost();
		int getX();
		int getY();
		cv::Point getEnd();

		VisionROI(int width, int height, int size, int id);
		~VisionROI();
};

#endif /* VISIONROI_HPP_ */
