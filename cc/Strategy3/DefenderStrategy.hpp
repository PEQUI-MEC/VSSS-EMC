#ifndef VSSS_DEFENDERSTRATEGY_HPP
#define VSSS_DEFENDERSTRATEGY_HPP

#include "Robot2.h"
#include "Geometry/Geometry.h"
#include "Field.h"
#include "Robot3.hpp"
#include "RoleStrategy.hpp"
#include "Ball.hpp"

class DefenderStrategy : public RoleStrategy {
	public:
	// Behaviors
	void run_strategy(const Ball& ball, const Robot3 * attacker);
	void protect_goal(const Geometry::Point &ball);
	void wait_at_target(const Geometry::Point target, const Geometry::Point &ball);
	void exit_goal();

	Role get_role() {
		return Role::Defender;
	}
};

#endif //VSSS_DEFENDERSTRATEGY_HPP
