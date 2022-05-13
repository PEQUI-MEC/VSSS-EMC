#include "Team.hpp"

std::vector<Adversary> Team::get_inverted_robot_adversary() {
	std::vector<Adversary> adversaries(robots.size());
	for (ulong i = 0; i < robots.size(); i++) {
		Geometry::Vector velocity(robots[i].pose.velocity.linear, robots[i].pose.orientation);
		adversaries[i] = Adversary{robots[i].pose.position.inverted_coordinates(), velocity.inverted_coordinates()};
	}
	return adversaries;
}

std::string to_string(RobotColor color) {
	if (color == RobotColor::Yellow) {
		return "Yellow";
	} else {
		return "Blue";
	}
}
