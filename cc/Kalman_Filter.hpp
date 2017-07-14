#ifndef KALMANFILTER_HPP_
#define KALMANFILTER_HPP_
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>




class KalmanFilter
{
  cv::KalmanFilter KF;
  cv::Mat KF_state;// [x,y,v_x,v_y,w,h]
  cv::Mat KF_meas; // [z_x,z_y,z_w,z_h]
  bool kf_found ;
  double ticks;

public:
  void KF_init(cv::Point pos) {
    ticks = 0;
    int stateSize = 6;
    int measSize = 4;
    int contrSize = 0;
    unsigned int type = CV_32F;
    cv::Mat stateAux(stateSize, 1, type);
    cv::Mat measAux(measSize, 1, type);
    KF_state = stateAux;  // [x,y,v_x,v_y,w,h]
    KF_meas = measAux;    // [z_x,z_y,z_w,z_h]
    KF.init(stateSize, measSize, contrSize, type);
    /*//cv::Mat procNoise(stateSize, 1, type)
    // [E_x,E_y,E_v_x,E_v_y,E_w,E_h]
    // Transition State Matrix A
    // Note: set dT at each processing step!
    // [ 1 0 dT 0  0 0 ]
    // [ 0 1 0  dT 0 0 ]
    // [ 0 0 1  0  0 0 ]
    // [ 0 0 0  1  0 0 ]
    // [ 0 0 0  0  1 0 ]
    // [ 0 0 0  0  0 1 ]*/
    cv::setIdentity(KF.transitionMatrix);
    /* // Measure Matrix H
    // [ 1 0 0 0 0 0 ]
    // [ 0 1 0 0 0 0 ]
    // [ 0 0 0 0 1 0 ]
    // [ 0 0 0 0 0 1 ]*/
    KF.measurementMatrix = cv::Mat::zeros(measSize, stateSize, type);
    KF.measurementMatrix.at<float>(0) = 1.0f;
    KF.measurementMatrix.at<float>(7) = 1.0f;
    KF.measurementMatrix.at<float>(16) = 1.0f;
    KF.measurementMatrix.at<float>(23) = 1.0f;
    /* // Process Noise Covariance Matrix Q
    // [ Ex 0  0    0 0    0 ]
    // [ 0  Ey 0    0 0    0 ]
    // [ 0  0  Ev_x 0 0    0 ]
    // [ 0  0  0    1 Ev_y 0 ]
    // [ 0  0  0    0 1    Ew ]
    // [ 0  0  0    0 0    Eh ]
    //cv::setIdentity(kfBall.processNoiseCov, cv::Scalar(1e-2));
    * */
    KF.processNoiseCov.at<float>(0) = 1e-2;
    KF.processNoiseCov.at<float>(7) = 1e-2;
    KF.processNoiseCov.at<float>(14) = 2.0f;
    KF.processNoiseCov.at<float>(21) = 1.0f;
    KF.processNoiseCov.at<float>(28) = 1e-2;
    KF.processNoiseCov.at<float>(35) = 1e-2;
    // Measures Noise Covariance Matrix R
    cv::setIdentity(KF.measurementNoiseCov, cv::Scalar(1e-1));
    //KF_meas.at<float>(0) = pos.x;
    //KF_meas.at<float>(1) = pos.y;
    //KF_meas.at<float>(2) = (float) 5;
    //KF_meas.at<float>(3) = (float) 5;
  }



  cv::Point KF_Prediction(cv::Point pos) {
    //std::cout<<"KF1"<<std::endl;
    KF.statePost = KF_state;
    //std::cout<<"KF1.1"<<std::endl;
    KF_meas.at<float>(0) = pos.x;
    //std::cout<<"KF1.2"<<std::endl;
    KF_meas.at<float>(1) = pos.y;
    //std::cout<<"KF1.3"<<std::endl;
    KF_meas.at<float>(2) = (float) 5;
    KF_meas.at<float>(3) = (float) 5;
    //std::cout<<"KF2"<<std::endl;
    if (!kf_found) // First detection!
    {
      //std::cout<<"KF3.1"<<std::endl;
      // >>>> Initialization
      KF.errorCovPre.at<float>(0) = 1; // px
      KF.errorCovPre.at<float>(7) = 1; // px
      KF.errorCovPre.at<float>(14) = 1;
      KF.errorCovPre.at<float>(21) = 1;
      KF.errorCovPre.at<float>(28) = 1; // px
      KF.errorCovPre.at<float>(35) = 1; // px
      KF_state.at<float>(0) = KF_meas.at<float>(0);
      KF_state.at<float>(1) = KF_meas.at<float>(1);
      KF_state.at<float>(2) = 0;
      KF_state.at<float>(3) = 0;
      KF_state.at<float>(4) = KF_meas.at<float>(2);
      KF_state.at<float>(5) = KF_meas.at<float>(3);
      // <<<< Initialization
      kf_found = true;
    }
    else {
      //std::cout<<"KF3.2"<<std::endl;
      KF.correct(KF_meas); // Kalman Correction
    }
    //std::cout<<"KF4"<<std::endl;
    double precTick = ticks;
    ticks = (double) cv::getTickCount();
    double dT = (ticks - precTick) / cv::getTickFrequency(); //seconds
    KF.transitionMatrix.at<float>(2) = dT;
    KF.transitionMatrix.at<float>(9) = dT;
    //std::cout<<"KF5"<<std::endl;
    KF_state = KF.predict();
    cv::Point center;
    //std::cout<<"KF6"<<std::endl;
    center.x = KF_state.at<float>(0);
    center.y = KF_state.at<float>(1);
    return center;
  }


  KalmanFilter()
  {

  }

  ~KalmanFilter()
    {}

};



    #endif /* KALMANFILTER_HPP_ */
