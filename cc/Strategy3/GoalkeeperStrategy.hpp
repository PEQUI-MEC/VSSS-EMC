#ifndef VSSS_GOALKEEPERSTRATEGY_HPP
#define VSSS_GOALKEEPERSTRATEGY_HPP

#include "Robot2.h"
#include "Geometry/Geometry.h"
#include "Robot3.hpp"
#include "RoleStrategy.hpp"
#include "Ball.hpp"

class GoalkeeperStrategy : public RoleStrategy {
	public:
	bool penalty = false;
	bool has_arrived_penalty = false;

	// Beahviors
	void run_strategy(const Ball& balll, const std::vector<Geometry::Point> &adversaries);
	void defend_penalty(const Ball& ball, const std::vector<Geometry::Point> &adversaries);
	void protect_goal(const Ball& ball);
	void spin_shot(const Geometry::Point& ball);
	void exit_goal();

	Role get_role() {
		return Role::Goalkeeper;
	}
};

#endif //VSSS_GOALKEEPER_HPP
