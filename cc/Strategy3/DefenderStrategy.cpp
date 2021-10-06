#include "DefenderStrategy.hpp"
#include "helper.hpp"

using namespace Geometry;
using namespace field;

void DefenderStrategy::run_strategy(const Ball &ball) {
	if (is_ball_est_ahead(their::area::front::center, ball, -0.08) && !at_location(robot->get_position(), Location::AnyGoal)) {
		// Bola na na área adversária
		if (at_location(ball.position, Location::UpperField))
			wait_at_target(defender::front::lower::wait_point, ball.position);
		else
			wait_at_target(defender::front::upper::wait_point, ball.position);

	} else if (at_location(robot->get_position(), Location::AnyGoal)) {
		exit_goal();
	} else {
		protect_goal(ball.position);
	}
}

void DefenderStrategy::wait_at_target(const Point target, const Point &ball) {
	if (distance(robot->get_position(), target) > robot->TARGET_OFFSET)
		robot->go_to_and_stop(target);
	else
		robot->set_target_orientation(ball - robot->get_position());
}

void DefenderStrategy::protect_goal(const Geometry::Point &ball) {
	if (distance(robot->get_position(), ball) < 0.08) {
		robot->spin_kick_to_target(ball, their::goal::front::center);
	} else if (ball.x >= robot->get_position().x) {
		robot->go_to_and_stop({defender::back::upper_limit.x, ball.y});
	} else if (at_location(ball, Location::UpperField)){
		robot->go_to_and_stop(defender::back::upper_limit);
	} else {
		robot->go_to_and_stop(defender::back::lower_limit);
	}
}

void DefenderStrategy::exit_goal() {
	robot->go_in_direction(Vector{0,0.8});
}
