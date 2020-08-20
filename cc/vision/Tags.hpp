#ifndef VSSS_TAGS_HPP
#define VSSS_TAGS_HPP

#include <Strategy3/Team.hpp>
#include "RecognizedTag.hpp"

class Tags {
	public:
	std::vector<RecognizedTag> yellow = {};
	std::vector<RecognizedTag> blue = {};
	Tag ball = {cv::Point(0, 0), 0};

	std::vector<RecognizedTag>& get_tags(RobotColor color) {
		if (color == RobotColor::Yellow) return yellow;
		else return blue;
	}
};

#endif //VSSS_TAGS_HPP
