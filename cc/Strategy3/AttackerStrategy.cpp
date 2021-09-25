#include "AttackerStrategy.hpp"
#include "helper.hpp"

using namespace Geometry;
using namespace field;

void AttackerStrategy::run_strategy(Ball& ball) {
	if (at_location(robot->get_position(), Location::TheirCornerAny)) {
		crossing(ball.position);
	} else if (has_ball(robot, ball) && at_location(robot->get_position(), Location::TheirAreaSideAny)) {
		spin_shot(ball.position);
	} else if (at_location(robot->get_position(), Location::OurBox)) {
		// Cobrar penalti
		charged_shot(ball.position);
	} else if (is_ball_behind(our::area::front::center, ball) && !at_location(robot->get_position(), Location::AnyGoal)) {
		protect_goal(ball.position);
	} else if (at_location(ball.position, Location::AnySide)){
		side_spin_shot(ball.position);
	} else if (at_location(robot->get_position(), Location::AnyGoal)) {
		exit_goal(ball.position);
	} else {
		uvf_to_goal(ball.position, ball.estimative);
	}
}

void AttackerStrategy::decide_spin_shot(const Geometry::Point &ball) {
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

//	if ((at_location(get_position(), Location::UpperField) && (ball.y > upper_y || ball.x > upper_x))
//		|| (at_location(get_position(), Location::LowerField) && (ball.y > upper_y || ball.x < lower_x))) {
//		spin(-35.0); // Robô gira no sentido horário
//	} else {
//		spin(35);//Robô gira no sentido anti-horário
//	}
}

void AttackerStrategy::uvf_to_goal(const Geometry::Point &ball, const Geometry::Point &ball_est) {
	Point goal = their::goal::back::center;
	Vector ball_to_pred = ball_est - ball;
	Point target = ball + (ball_to_pred * 0.4);

	Vector ball_to_goal = goal - ball;
	Vector pred_to_goal = goal - target;
	Vector robot_to_ball = ball - robot->get_position();
	double orientation_error = std::abs(wrap(robot_to_ball.theta - ball_to_goal.theta));
//	double orientation_error_backwards = std::abs(wrap(robot_to_ball.theta - (ball_to_goal.theta + M_PI)));
	auto error_smaller_than = [&](double theta) {
		return orientation_error < degree_to_rad(theta);
	};
	auto error_bigger_than = [&](double theta) {
		return orientation_error > degree_to_rad(theta);
	};

	switch (uvf_state) {
		case UvfState::seek_ball:
			if (error_smaller_than(15) && robot->get_position().x < target.x && robot_to_ball.size < 0.15)
				uvf_state = UvfState::close_to_ball;
			break;
		case UvfState::close_to_ball:
			if (error_bigger_than(70))
				uvf_state = UvfState::seek_ball;
			else if (robot_to_ball.size < 0.1)
				uvf_state = UvfState::has_ball;
			break;
		case UvfState::has_ball:
			if (error_bigger_than(90) || robot_to_ball.size > 0.3)
				uvf_state = UvfState::seek_ball;
			break;
	}

	switch (uvf_state) {
		case UvfState::seek_ball:
			robot->go_to_pose(target, ball_to_goal);
			break;
		case UvfState::close_to_ball:
			robot->go_to(ball);
			break;
		case UvfState::has_ball:
			robot->go_to(goal, 1.4);
			break;
	}
}

void AttackerStrategy::spin_shot(const Geometry::Point &ball) {
	decide_spin_shot(ball);
}

void AttackerStrategy::crossing(const Geometry::Point &ball){
	if (distance(ball, robot->get_position()) >= robot->SIZE) {
		// longe da bola: vai até a bola
		robot->go_to(ball);
	} else {
		decide_spin_shot(ball);
	}
}

void AttackerStrategy::protect_goal(const Geometry::Point &ball) {
	if (distance(robot->get_position(), ball) < robot->BALL_OFFSET) {
		 // Se a bola chegar perto, gira para jogar a bola longe
		if (at_location(ball, Location::UpperField))
			robot->spin(-35); // horário
		else
			robot->spin(35); // anti-horário
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

	if(robot->get_position().x < ball.x-0.01 && (distance_to_ball > 0.065) ){
		//para evitar o robo ficar dançando quando estiver na lateral correndo atrás da bola
		robot->go_to(ball);
	}else
	if (distance_to_ball <= 0.065 && robot->get_position().x < ball.x){
		if(ball.y > their::goal::front::center.y){
			robot->spin(-35);//Robô gira no sentido anti-horárioo
		}else{
			robot->spin(+35);// Robô gira no sentido horário
		}
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
