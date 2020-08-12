#include "Team.hpp"

std::vector<Geometry::Point> Team::get_inverted_robot_positions() {
	std::vector<Geometry::Point> positions(robots.size());
	for (ulong i = 0; i < robots.size(); i++) {
		positions[i] = robots[i].pose.position.inverted_coordinates();
	}
	return positions;
}

std::string to_string(RobotColor color) {
	if (color == RobotColor::Yellow) {
		return "Yellow";
	} else {
		return "Blue";
	}
}
