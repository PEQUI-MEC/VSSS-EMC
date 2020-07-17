#ifndef VSSS_DEFENDERSTRATEGY_HPP
#define VSSS_DEFENDERSTRATEGY_HPP

#include "Robot2.h"
#include "Geometry/Geometry.h"
#include "Field.h"
#include "Robot3.hpp"
#include "RoleStrategy.hpp"

class DefenderStrategy : public RoleStrategy {
	public:
	// Behaviors
	void protect_goal(const Geometry::Point &ball);
	void wait_at_target(Geometry::Point target, Geometry::Point &ball);
	void decide_spin_shot(const Geometry::Point &ball);
	void exit_goal();
};

#endif //VSSS_DEFENDERSTRATEGY_HPP
