#include "GoalkeeperStrategy.hpp"
#include "Field.h"

using namespace field;
using namespace Geometry;

void GoalkeeperStrategy::run_strategy(const Ball &ball) {
	if (distance(robot->get_position(), ball.position) < robot->BALL_OFFSET && !at_location(robot->get_position(), Location::AnyGoal))
		spin_shot(ball.position);
	else if (at_location(robot->get_position(), Location::AnyGoal))
		exit_goal();
	else
		protect_goal(ball.position, ball.estimative);
}

void GoalkeeperStrategy::protect_goal(const Point& ball, const Point& ball_est) {
	if (at_location(ball, Location::OurUpperCorner)) {
		robot->go_to_and_stop(our::area::upper::center);
	} else if (at_location(ball, Location::OurLowerCorner)) {
		robot->go_to_and_stop(our::area::lower::center);
	} else if (ball_est.x < ball.x && distance(ball, ball_est) > 0.03) {
		// a bola está vindo para o nosso campo

		// Se goal_est estiver na pequena área, fica na projeção da estimativa da bola
		// se não fica no limite da pequena área
		if (match_y(ball_est, Location::OurBox))
			robot->go_to_and_stop({our::area::box::upper_limit.x, ball_est.y});
		else if (at_location(ball_est, Location::UpperField))
			robot->go_to_and_stop(our::area::box::upper_limit);
		else
			robot->go_to_and_stop(our::area::box::lower_limit);
	} else {
		// fica na projeção da bola, sempre em frente ao gol
		if (match_y(ball, Location::OurBox))
			robot->go_to_and_stop({our::area::box::upper_limit.x, ball.y});
		else if (at_location(ball, Location::UpperField))
			robot->go_to_and_stop(our::area::box::upper_limit);
		else
			robot->go_to_and_stop(our::area::box::lower_limit);
	}
}

void GoalkeeperStrategy::spin_shot(const Point& ball) {
	double lower_robot_x = robot->get_position().x - robot->SIZE/4; // é size/4 para que evite fazer gol contra
	double upper_robot_x = robot->get_position().x + robot->SIZE/2;

	if (ball.x > upper_robot_x) { // bola na frente do goleiro
		if (at_location(robot->get_position(), Location::UpperField))
			robot->spin(35.0);
		else
			robot->spin(-35.0);
	} else if (ball.x > lower_robot_x && ball.x <= upper_robot_x) { // bola na lateral do goleiro
		if (ball.y > robot->get_position().y) // bola acima do goleiro
			robot->spin(-35.0); // horário
		else
			robot->spin(35.0); // anti-horário
	} else {
		// FIXME: Caso contrário o goleiro deve ficar parado para evitar fazer gol contra. Poderia fazer outra coisa?
		robot->go_to_and_stop(robot->get_position());
	}
}

void GoalkeeperStrategy::exit_goal() {
	robot->go_to_and_stop(our::area::box::center_line_point);
}
