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
		protect_goal(ball);
}

void GoalkeeperStrategy::protect_goal(const Ball& ball) {
	if (at_location(ball.position, Location::OurUpperCorner)) {
		robot->go_to_and_stop_orientation(our::area::upper::center, M_PI/2);
	} else if (at_location(ball.position, Location::OurLowerCorner)) {
		robot->go_to_and_stop_orientation(our::area::lower::center, M_PI/2);
	} else if (std::abs(ball.velocity.theta) > M_PI/2
                && ball.velocity.size > 0.1) {
		// a bola está vindo para o nosso campo

		// Se goal_est estiver na pequena área, fica na projeção da estimativa da bola
		// se não fica no limite da pequena área
        auto dist_x_ball_goal = distance_x(ball.position, our::area::lower::center);
        auto distance_ball_projection = dist_x_ball_goal / std::cos(M_PI - ball.velocity.theta);
        auto ball_goal_projection = ball.position + ball.velocity.with_size(distance_ball_projection);
		if (match_y(ball_goal_projection, Location::OurBox))
			robot->go_to_and_stop_orientation({our::area::box::upper_limit.x, ball_goal_projection.y}, M_PI/2);
		else if (at_location(ball_goal_projection, Location::UpperField))
			robot->go_to_and_stop_orientation(our::area::box::upper_limit, M_PI/2);
		else
			robot->go_to_and_stop_orientation(our::area::box::lower_limit, M_PI/2);
	} else {
		// fica na projeção da bola, sempre em frente ao gol
		if (match_y(ball.position, Location::OurBox))
			robot->go_to_and_stop_orientation({our::area::box::upper_limit.x, ball.position.y}, M_PI/2);
		else if (at_location(ball.position, Location::UpperField))
			robot->go_to_and_stop_orientation(our::area::box::upper_limit, M_PI/2);
		else
			robot->go_to_and_stop_orientation(our::area::box::lower_limit, M_PI/2);
	}
}

void GoalkeeperStrategy::spin_shot(const Point& ball) {
    if (std::abs(distance_y(robot->get_position(), ball)) > robot->SIZE/4) { // é size/4 para que evite fazer gol contra
        robot->spin_kick_to_target(ball, their::goal::front::center);
    } else {
        // FIXME: Caso contrário o goleiro deve ficar parado para evitar fazer gol contra. Poderia fazer outra coisa?
        robot->go_to_and_stop_orientation(robot->get_position(), M_PI/2);
    }
}

void GoalkeeperStrategy::exit_goal() {
	robot->go_to_and_stop(our::area::box::center_line_point);
}
