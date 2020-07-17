#ifndef VSSS_ROLESTRATEGY_HPP
#define VSSS_ROLESTRATEGY_HPP

#include "Robot3.hpp"

class RoleStrategy {
	public:
	Robot3 * robot = nullptr;
	Robot3* operator->() {
		return robot;
	}
	void set_robot(Robot3& robot) {
		this->robot = &robot;
		robot.role = Role::Goalkeeper;
	}
};

#endif //VSSS_ROLESTRATEGY_HPP
