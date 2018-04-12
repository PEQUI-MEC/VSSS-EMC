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

class Vision
{

private:

  // Constants
  static const int MAIN = 0;
  static const int GREEN = 1;
  static const int BALL = 2;
  static const int ADV = 3;
  static const int MAX_ADV = 3;
  static const int TOTAL_COLORS = 4;
  static const int MIN = 0;
  static const int MAX = 1;


  // Frames
  cv::Mat in_frame, hsv_frame;
  std::vector<cv::Mat>threshold_frame;
  cv::Mat splitFrame;

  // Robots
  std::vector<Robot> robot_list;
  cv::Point advRobots[MAX_ADV];

  // Ball
  cv::Point ball;

  // TAGS
  std::vector<std::vector<Tag>> tags;

  // HSV/CIELAB Calibration Parameters
  int hue[4][2];
  int saturation[4][2];
  int value[4][2];
		int cieL[4][2];
		int cieA[4][2];
		int cieB[4][2];
  int dilate[2][4];
  int erode[2][4];
  int blur[2][4];
  int areaMin[2][4];
		int convertType;

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
  void pick_a_tag(std::vector<VisionROI> *windowsList);
  void pick_a_tag();
  int inSphere(Robot * robot, std::vector<Tag> * tempTags, cv::Point secondary);

public:
		// Public constants
		static const int HSV = 0;
		static const int CIELAB = 1;

  Vision(int w, int h);
  ~Vision();

  void run(cv::Mat raw_frame);
  void runGMM(std::vector<cv::Mat> thresholds, std::vector<VisionROI> *windowsList);
  void recordVideo(cv::Mat frame);
  void setCalibParams(int type, int H[4][2], int S[4][2], int V[4][2], int Amin[4], int E[4], int D[4], int B[4]);
  double calcDistance(cv::Point p1, cv::Point p2);
  void saveCameraCalibPicture(std::string  in_name, std::string directory);
  void startNewVideo(std::string videoName);
  bool recordToVideo();
  bool finishVideo();
  bool isRecording();
  void savePicture(std::string in_name);

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
    void cameraCalibration();
    std::vector<cv::Point3f> createKnownBoardPosition(cv::Size boardSize, float squareEdgeLenght);
	bool foundChessBoardCorners();
  	void switchMainWithAdv();

  cv::Point getBall();
  Robot getRobot(int index);
  cv::Point getRobotPos(int index);
  cv::Point getAdvRobot(int index);
  cv::Point* getAllAdvRobots();
		cv::Mat getSplitFrame();

  int getRobotListSize();
  int getAdvListSize();
  cv::Mat getThreshold(int index);

  int getHue(int index0, int index1);
  int getSaturation(int index0, int index1);
  int getValue(int index0, int index1);
		int getCIE_L(int index0, int index1);
		int getCIE_A(int index0, int index1);
		int getCIE_B(int index0, int index1);
		int getConvertType();
  int getErode(int convertType, int index);
  int getDilate(int convertType, int index);
  int getBlur(int convertType, int index);
  int getAmin(int convertType, int index);

  void setFrameSize(int inWidth, int inHeight);
  int getFrameHeight();
  int getFrameWidth();

  void setHue(int index0, int index1, int inValue);
  void setSaturation(int index0, int index1, int inValue);
  void setValue(int index0, int index1, int inValue);
		void setCIE_L(int index0, int index1, int inValue);
		void setCIE_A(int index0, int index1, int inValue);
		void setCIE_B(int index0, int index1, int inValue);
		void setConvertType(int type);
  void setErode(int convertType, int index, int inValue);
  void setDilate(int convertType, int index, int inValue);
  void setBlur(int convertType, int index, int inValue);
  void setAmin(int convertType, int index, int inValue);
};

#endif /* VISION_HPP_ */
