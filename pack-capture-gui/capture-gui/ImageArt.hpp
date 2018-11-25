//
// Created by daniel on 25/11/18.
//

#ifndef VSSS_IMAGEART_HPP
#define VSSS_IMAGEART_HPP

#include "opencv2/opencv.hpp"
#include "vision.hpp"
#include "Robot2.h"

namespace art {
	void draw(cv::Mat& frame,
			  const std::vector<cv::Point> &gmm_points,
			  const cv::Point& ball,
			  const std::array<vision::Vision::RecognizedTag, 3>& our_tags,
			  const std::array<cv::Point, vision::MAX_ADV>& adv_tags,
			  const std::array<Robot2 *, 3>& our_robots);

};



#endif //VSSS_IMAGEART_HPP
