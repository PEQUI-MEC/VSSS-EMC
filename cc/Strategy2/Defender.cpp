#include "Defender.hpp"

using namespace Geometry;
using namespace field;

void Defender::decide_spin_shot(const Geometry::Point &ball) {
	double upper_y = get_position().y + SIZE/2;
	double upper_x = get_position().x + SIZE/2;
	double lower_x = get_position().x - SIZE/2;

	auto robot_to_ball = ball - get_position();

	if (at_location(get_position(), Location::UpperField)) {
		if (robot_to_ball.theta < -M_PI/2) {
			spin(35);
		} else {
			spin(-35);
		}
	} else {
		if (robot_to_ball.theta > M_PI/2) {
			spin(-35);
		} else {
			spin(35);
		}
	}

//	if ((at_location(get_position(), Location::UpperField) && (ball.y > upper_y || ball.x > upper_x))
//		|| (at_location(get_position(), Location::LowerField) && (ball.y > upper_y || ball.x < lower_x))) {
//		spin(-35.0); // Robô gira no sentido horário
//	} else {
//		spin(35);//Robô gira no sentido anti-horário
//	}
}

void Defender::wait_at_target(Geometry::Point target, Geometry::Point &ball) {
	if (distance(get_position(), target) > TARGET_OFFSET)
		go_to_and_stop(target);
	else
		set_target_orientation(ball - get_position());
}

void Defender::protect_goal(const Geometry::Point &ball) {
	if (distance(get_position(), ball) < 0.08 && ball.x > get_position().x) {
		decide_spin_shot(ball);
	} else if (ball.x >= get_position().x) {
		go_to_and_stop({defender::back::upper_limit.x, ball.y});
	} else if (at_location(ball, Location::UpperField)){
		go_to_and_stop(defender::back::upper_limit);
	} else {
		go_to_and_stop(defender::back::lower_limit);
	}
}

void Defender::exit_goal() {
	go_in_direction(Vector{0,0.8});
}
