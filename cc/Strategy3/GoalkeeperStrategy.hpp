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

	const double SPIN_THRESHOLD = 0.04;

	// Beahviors
	void run_strategy(const Ball& balll, const std::vector<Adversary> &adversaries);
	void defend_penalty(const Ball& ball, const std::vector<Adversary> &adversaries);
	void protect_goal(const Ball& ball, const std::vector<Adversary> &adversaries);
	void spin_shot(const Geometry::Point& ball);
	void exit_goal();
	void handle_corner(const Ball &ball);

	Role get_role() {
		return Role::Goalkeeper;
	}
};

#endif //VSSS_GOALKEEPER_HPP
