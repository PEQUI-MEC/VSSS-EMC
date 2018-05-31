#ifndef VISION_HPP_
#define VISION_HPP_
//dimensões dos quadrados do tabuleiro
#define CALIBRATION_SQUARE_DIMENSION 0.02629f
// número de intersecções do tabuleiro
#define CHESSBOARD_DIMENSION cv::Size(6,9)

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
#include <iostream>
#include "tag.hpp"
#include "visionROI.hpp"

class Vision {

	private:

		// Constants
		static const int MAIN = 0;
		static const int GREEN = 1;
		static const int BALL = 2;
		static const int ADV = 3;
		static const int MIN = 0;
		static const int MAX = 1;
		static const int MAX_ADV = 3;

		// Frames
		cv::Mat in_frame, lab_frame;
		std::vector<cv::Mat> threshold_frame;
		cv::Mat splitFrame;

		// Robots
		std::vector<Robot> robot_list;
		cv::Point advRobots[MAX_ADV];

		// Ball
		cv::Point ball;

		// TAGS
		std::vector<std::vector<Tag>> tags;

		// HSV/CIELAB Calibration Parameters
		int cieL[4][2];
		int cieA[4][2];
		int cieB[4][2];
		int dilate[4];
		int erode[4];
		int blur[4];
		int areaMin[4];

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
		void posProcessing(unsigned long color);
		void segmentAndSearch(unsigned long color);
		void searchTags(unsigned long color);
		void searchGMMTags(std::vector<cv::Mat> thresholds);
		void findTags();
		void pick_a_tag(std::vector<VisionROI> *windowsList);
		void pick_a_tag();
		int inSphere(Robot *robot, std::vector<Tag> *tempTags, cv::Point secondary);

	public:

		// Public Constants
		static const int TOTAL_COLORS = 4;

		Vision(int w, int h);
		~Vision();

		void run(cv::Mat raw_frame);
		void runGMM(std::vector<cv::Mat> thresholds, std::vector<VisionROI> *windowsList);
		void recordVideo(cv::Mat frame);
		void setCalibParams(const int H[4][2], const int S[4][2], const int V[4][2], const int Amin[4], const int E[4],
							const int D[4], const int B[4]);
		double calcDistance(cv::Point p1, cv::Point p2);
		void saveCameraCalibPicture(std::string in_name, std::string directory);
		void startNewVideo(std::string videoName);
		bool recordToVideo();
		bool finishVideo();
		bool isRecording();
		void savePicture(std::string in_name);

		//video
		bool video_rec_enable;

		//Cam calib
		std::vector<cv::Mat> savedCamCalibFrames;
		cv::Mat cameraMatrix;
		cv::Mat distanceCoeficents;
		bool flag_cam_calibrated = false;
		std::vector<std::vector<cv::Point2f>> getChessBoardCorners(std::vector<cv::Mat> images);
		std::vector<cv::Mat> getCamCalibFrames();
		cv::Mat getcameraMatrix();
		cv::Mat getdistanceCoeficents();
		void setFlagCamCalibrated(bool value);
		void saveCamCalibFrame();
		void popCamCalibFrames();
		void collectImagesForCalibration();
		void cameraCalibration();
		std::vector<cv::Point3f> createKnownBoardPosition(cv::Size boardSize, float squareEdgeLenght);
		bool foundChessBoardCorners();
		void switchMainWithAdv();

		cv::Point getBall();
		Robot getRobot(unsigned long index);
		cv::Point getRobotPos(unsigned long index);
		cv::Point getAdvRobot(int index);
		cv::Point *getAllAdvRobots();
		cv::Mat getSplitFrame();

		int getRobotListSize();
		int getAdvListSize();
		cv::Mat getThreshold(unsigned long index);

		int getCIE_L(unsigned long index0, int index1);
		int getCIE_A(unsigned long index0, int index1);
		int getCIE_B(unsigned long index0, int index1);
		int getErode(unsigned long index);
		int getDilate(unsigned long index);
		int getBlur(unsigned long index);
		int getAmin(unsigned long index);

		void setFrameSize(int inWidth, int inHeight);

		void setCIE_L(unsigned long index0, int index1, int inValue);
		void setCIE_A(unsigned long index0, int index1, int inValue);
		void setCIE_B(unsigned long index0, int index1, int inValue);
		void setErode(unsigned long index, int inValue);
		void setDilate(unsigned long index, int inValue);
		void setBlur(unsigned long index, int inValue);
		void setAmin(unsigned long index, int inValue);
};

#endif /* VISION_HPP_ */
