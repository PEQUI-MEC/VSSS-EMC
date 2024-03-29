#ifndef VSSS_ROLESTRATEGY_HPP
#define VSSS_ROLESTRATEGY_HPP

#include "Robot3.hpp"
#include "Adversary.hpp"

class RoleStrategy {
	public:
	Robot3 * robot = nullptr;
	Robot3* operator->() {
		return robot;
	}
	virtual Role get_role() = 0;

	void set_robot(Robot3* robot) {
		this->robot = robot;
		if (this->robot != nullptr) {
			robot->role = get_role();
		}
	}

	bool has_robot() const {
		return robot != nullptr;
	}
};

#endif //VSSS_ROLESTRATEGY_HPP
