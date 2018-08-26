//
// Created by daniel on 26/08/18.
//

#include "Strategy2.hpp"

using namespace field;
using namespace Geometry;

void Strategy2::run() {

	checkForTransitions();
}

void Strategy2::checkForTransitions() {

	// Cruzamento
	if (at_location(ball, Location::TheirBox) && distance(attacker.get_position(), ball) > 0.12)
		swap_atk_def();

	// Se a bola está bem atrás do atacante mas está na frente do defensor
	if (distance_x(attacker.get_position(), ball) > 0.36 && is_ball_behind(attacker) &&
		!at_location(attacker, Location::TheirCornerAny))
		swap_atk_def();

	if ()
}

void Strategy2::swap_atk_def()
{
	Pose old_def_pose = defender.get_pose();
}

bool Strategy2::is_ball_behind(const Point& point) {
	return ball.x < point.x;
}

bool Strategy2::is_ball_behind(const Robot2& robot) {
	return is_ball_behind(robot.get_position());
}
