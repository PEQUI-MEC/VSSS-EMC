#ifndef VSSS_ROBOTS_HPP
#define VSSS_ROBOTS_HPP

#include <vector>
#include "Robot3.hpp"

class Robots {
	public:
	std::vector<Robot3> robots;

	Robot3& find_robot_by_id(char ID) {
		auto robot_it = std::find_if(robots.begin(), robots.end(), [&ID](const Robot3& robot){
			return robot.ID == ID;
		});
		return *robot_it;
	}

	Robot3& find_robot_by_tag(int TAG) {
		auto robot_it = std::find_if(robots.begin(), robots.end(), [&TAG](const Robot3& robot){
			return robot.ID == TAG;
		});
		return *robot_it;
	}

	Robot3& find_robot_by_role(Role role) {
		auto robot_it = std::find_if(robots.begin(), robots.end(), [&role](const Robot3& robot){
			return robot.role == role;
		});
		return *robot_it;
	}
};

#endif //VSSS_ROBOTS_HPP
