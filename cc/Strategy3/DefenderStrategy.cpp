#include "DefenderStrategy.hpp"

using namespace Geometry;
using namespace field;

void DefenderStrategy::decide_spin_shot(const Point &ball) {
	double upper_y = robot->get_position().y + robot->SIZE/2;
	double upper_x = robot->get_position().x + robot->SIZE/2;
	double lower_x = robot->get_position().x - robot->SIZE/2;

	auto robot_to_ball = ball - robot->get_position();

	if (at_location(robot->get_position(), Location::UpperField)) {
		if (robot_to_ball.theta < -M_PI/2) {
			robot->spin(35);
		} else {
			robot->spin(-35);
		}
	} else {
		if (robot_to_ball.theta > M_PI/2) {
			robot->spin(-35);
		} else {
			robot->spin(35);
		}
	}

//	if ((at_location(robot->get_position(), Location::UpperField) && (ball.y > upper_y || ball.x > upper_x))
//		|| (at_location(robot->get_position(), Location::LowerField) && (ball.y > upper_y || ball.x < lower_x))) {
//		robot->spin(-35.0); // Robô gira no sentido horário
//	} else {
//		robot->spin(35);//Robô gira no sentido anti-horário
//	}
}

void DefenderStrategy::wait_at_target(Geometry::Point target, Geometry::Point &ball) {
	if (distance(robot->get_position(), target) > robot->TARGET_OFFSET)
		robot->go_to_and_stop(target);
	else
		robot->set_target_orientation(ball - robot->get_position());
}

void DefenderStrategy::protect_goal(const Geometry::Point &ball) {
	if (distance(robot->get_position(), ball) < 0.08) {
		decide_spin_shot(ball);
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
