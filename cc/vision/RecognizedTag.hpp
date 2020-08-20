#ifndef VSSS_RECOGNIZEDTAG_HPP
#define VSSS_RECOGNIZEDTAG_HPP

#include <opencv2/core/types.hpp>
#include "tag.hpp"

struct RecognizedTag {
	cv::Point position = {ROBOT_RADIUS/2, 0};
	double orientation = 0;
	cv::Point front_point = {ROBOT_RADIUS, 0};
	cv::Point rear_point = {0, 0};
};

#endif //VSSS_RECOGNIZEDTAG_HPP
