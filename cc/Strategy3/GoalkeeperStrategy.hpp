#ifndef VSSS_GOALKEEPERSTRATEGY_HPP
#define VSSS_GOALKEEPERSTRATEGY_HPP

#include "Robot2.h"
#include "Geometry/Geometry.h"
#include "Robot3.hpp"
#include "RoleStrategy.hpp"

class GoalkeeperStrategy : public RoleStrategy {
	public:
	// Beahviors
	void protect_goal(const Geometry::Point& ball, const Geometry::Point& ball_est);
	void spin_shot(const Geometry::Point& ball);
	void exit_goal();

	Role get_role() {
		return Role::Goalkeeper;
	}
};

#endif //VSSS_GOALKEEPER_HPP
