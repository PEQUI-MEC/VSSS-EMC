#include <utility>

#include "Tag.hpp"

Tag::Tag(cv::Point_<double> pos, double myarea) : position(std::move(pos)), front_point(0), rear_point(0), area(myarea){
}

void Tag::setLine(cv::Vec4f myLine) {
	// Linha rotacionada em 90 graus
	float vx = myLine[1];
	float vy = -myLine[0];

	float x = myLine[2];
	float y = myLine[3];
	Tag::front_point = cv::Point((int) (x + vx * ROBOT_RADIUS), (int) (y + vy * ROBOT_RADIUS));
	Tag::rear_point = cv::Point((int) (x - vx * ROBOT_RADIUS), (int) (y - vy * ROBOT_RADIUS));
}

void Tag::switchPoints() {
	cv::Point temp;
	temp = Tag::rear_point;
	Tag::rear_point = Tag::front_point;
	Tag::front_point = temp;
}
