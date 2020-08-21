#ifndef VSSS_TAGS_HPP
#define VSSS_TAGS_HPP

#include <Strategy3/Team.hpp>

class Tags {
	public:
	std::vector<Tag> yellow = {};
	std::vector<Tag> blue = {};
	Tag ball = {cv::Point(0, 0), 0};

	bool yellow_has_orientation = true;
	bool blue_has_orientation = true;
	bool found_ball = false;

	std::vector<Tag>& get_tags(RobotColor color) {
		if (color == RobotColor::Yellow) return yellow;
		else return blue;
	}
};

#endif //VSSS_TAGS_HPP
