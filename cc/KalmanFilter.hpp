#ifndef KALMANFILTER_HPP_
#define KALMANFILTER_HPP_

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>

class KalmanFilter {
		cv::KalmanFilter KF;
		cv::Mat KF_state;// [x,y,v_x,v_y,w,h]
		cv::Mat KF_meas; // [z_x,z_y,z_w,z_h]
		bool kf_found;
		double ticks;

	public:
		KalmanFilter() = default;
		void KF_init(cv::Point pos);
		cv::Point KF_Prediction(cv::Point pos);
};

#endif /* KALMANFILTER_HPP_ */
