#ifndef VSSS_STRATEGY3_HPP
#define VSSS_STRATEGY3_HPP

#include "Strategy.hpp"
#include "AttackerStrategy.hpp"
#include "DefenderStrategy.hpp"
#include "GoalkeeperStrategy.hpp"
#include "Ball.hpp"

class Strategy3 : public Strategy {
	AttackerStrategy attacker;
	DefenderStrategy defender;
	GoalkeeperStrategy goalkeeper;
	Ball ball;

	void run_strategy(Robots team,
					  std::vector<Geometry::Point> &adversaries,
					  Ball ball);

	Strategy3 * clone() {
		return new Strategy3(*this);
	}

	using sc = std::chrono::system_clock;
	using duration_ms = std::chrono::duration<double, std::milli>;
	sc::time_point last_transition = sc::now();

	bool transitions();
	bool trainstion_by_collision();
	void swap_robots(RoleStrategy &role1, RoleStrategy &role2);
	void swap_all_robots();

	bool is_collision(Robot3 *robot1, Robot3 *robot2);
	bool is_collision_axis_y(Robot3 *robot1, Robot3 *robot2);
	bool is_collision_axis_x(Robot3 *robot1, Robot3 *robot2);
	bool is_opposite_direction(Robot3 *robot1, Robot3 *robot2);
};

#endif //VSSS_STRATEGY3_HPP
