#include "AttackerStrategy.hpp"
#include "helper.hpp"

using namespace Geometry;
using namespace field;

void AttackerStrategy::run_strategy(Ball& ball, bool is_penalty) {
	if (at_location(robot->get_position(), Location::TheirCornerAny)) {
		crossing(ball.position);
	} else if (has_ball(robot, ball) && at_location(robot->get_position(), Location::TheirAreaSideAny)) {
		robot->spin_kick_to_target(ball.position, their::goal::front::center);
// 	} else if (at_location(robot->get_position(), Location::OurBox)) {
		// Cobrar penalti
// 		charged_shot(ball.position);
    } else if (at_location(ball.position, Location::AnySide)){
		side_spin_shot(ball.position);
 	} else if (is_ball_behind(our::area::front::center, ball) && !at_location(robot->get_position(), Location::AnyGoal)) {
 		protect_goal(ball.position);
// 	} else if (at_location(robot->get_position(), Location::AnyGoal)) {
// 		exit_goal(ball.position);
	} else {
		uvf_to_goal(ball, is_penalty);
	}
}

void AttackerStrategy::uvf_to_goal(Ball& ball, bool is_penalty) {
	Point goal = their::goal::back::center;
	Point target = ball.position + (ball.velocity * 0.05);

	auto upper_goal = field::their::goal::front::upper_limit;
	auto lower_goal = field::their::goal::front::lower_limit;
	Vector ball_to_upper = upper_goal - ball.position;
	Vector ball_to_lower = lower_goal - ball.position;

	Vector ball_to_goal = goal - ball.position;
	Vector pred_to_goal = goal - target;
	Vector robot_to_ball = ball.position - robot->get_position();

	auto can_run_to_goal = robot_to_ball.angle_to(ball_to_upper) > degree_to_rad(-5) &&
							robot_to_ball.angle_to(ball_to_lower) < degree_to_rad(5);

	// bool use_ball_vel = std::abs(wrap(ball.velocity.theta - M_PI/2)) < 15;
	bool use_ball_vel = std::abs(ball.velocity.size) > 0.04;

	double orientation_error = std::abs(wrap(robot_to_ball.theta - ball_to_goal.theta));
//	double orientation_error_backwards = std::abs(wrap(robot_to_ball.theta - (ball_to_goal.theta + M_PI)));
	auto error_smaller_than = [&](double theta) {
		return orientation_error < degree_to_rad(theta);
	};
	auto error_bigger_than = [&](double theta) {
		return orientation_error > degree_to_rad(theta);
	};

	bool in_dir_of_goal = std::abs(ball.velocity.theta) < degree_to_rad(90) && std::abs(ball.velocity.size) > 0.05;
	// bool in_dir_of_goal = std::abs(ball.velocity.theta) < degree_to_rad(90);
	auto oposite_ball_vel = Vector{1, wrap(ball.velocity.theta + M_PI)};
	auto target_dir = in_dir_of_goal ? ball.velocity : oposite_ball_vel;
	// auto target_dir = ball_to_goal;
	// robot->target.n = in_dir_of_goal ? 1.7 : 1.7;

	target_dir = use_ball_vel ? target_dir : ball_to_goal;

	switch (uvf_state) {
		case UvfState::seek_ball:
			if (can_run_to_goal) {
				uvf_state = UvfState::close_to_ball;
                uvf_run_direction = robot_to_ball;
			}
			break;
		case UvfState::close_to_ball:
			if (error_bigger_than(90))
				uvf_state = UvfState::seek_ball;
			else if (robot_to_ball.size < 0.1)
				uvf_state = UvfState::has_ball;
			break;
		case UvfState::has_ball:
			if (error_bigger_than(90) || robot_to_ball.size > 0.25)
				uvf_state = UvfState::seek_ball;
			break;
	}

	switch (uvf_state) {
		case UvfState::seek_ball:
			robot->go_to_pose(ball.position, target_dir);
			break;
		case UvfState::close_to_ball:
			robot->go_to(ball.position);
			break;
		case UvfState::has_ball:
            if (is_penalty) {
                robot->go_to(ball.position_in_seconds(0.2), 2);
            } else {
                robot->go_to(goal, 1.5);
            }
			break;
	}
}

void AttackerStrategy::crossing(const Geometry::Point &ball){
	if (distance(ball, robot->get_position()) >= robot->SIZE) {
		// longe da bola: vai até a bola
		robot->go_to(ball);
	} else {
		robot->spin_kick_to_target(ball, their::goal::front::center);
	}
}

void AttackerStrategy::protect_goal(const Geometry::Point &ball) {
	if (distance(robot->get_position(), ball) < robot->BALL_OFFSET) {
		 // Se a bola chegar perto, gira para jogar a bola longe
		robot->spin_kick_to_target(ball, their::goal::front::center);
	} else if (at_location(ball, Location::UpperField)) {
		// bloquear area (cima)
		robot->go_to_and_stop(our::corner::upper::attacker::point);
	} else {
		// bloquear area (baixo)
		robot->go_to_and_stop(our::corner::lower::attacker::point);
	}
}

void AttackerStrategy::charged_shot(const Geometry::Point &ball) {
	robot->go_in_direction(ball - robot->get_position(), 1.2);
}

void AttackerStrategy::side_spin_shot(Point ball){
	double distance_to_ball = distance(robot->get_position(), ball);

	if(robot->get_position().x < ball.x-0.01 && (distance_to_ball > 0.07) ){
		//para evitar o robo ficar dançando quando estiver na lateral correndo atrás da bola
		robot->go_to(ball);
	}else
	if (distance_to_ball <= 0.07 && robot->get_position().x < ball.x){
		robot->spin_kick_to_target(ball, their::goal::front::center);
	}else{
		if(ball.y > their::goal::front::center.y){
			Vector ball_to_side = {1,  degree_to_rad(45)};
			robot->go_to_pose(ball, ball_to_side);
		} else {
			Vector ball_to_side = {1, - degree_to_rad(45)}; //ball_to_goal.theta - degree_to_rad(90)
			robot->go_to_pose(ball, ball_to_side);
		}

	}
}

void AttackerStrategy::exit_goal(const Geometry::Point &ball) {
	if (!at_location(robot->get_position(), Location::OurField) && ball.x > robot->get_position().x)
		// Se o atacante estiver dentro do gol adversário, mas a bola está na frente dele
		// só continua indo pro gol, não importa
		robot->go_in_direction({1,0}, 1.4);
	else
		// Sai do gol
		robot->go_to(center::point);
}
