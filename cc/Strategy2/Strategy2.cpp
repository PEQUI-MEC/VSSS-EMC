#include "Strategy2.hpp"

using namespace field;
using namespace Geometry;

Strategy2::Strategy2(Attacker &attacker, Defender &defender, Goalkeeper& goalkeeper, Geometry::Point &ball,
					 Geometry::Point &ball_est) :
		attacker(attacker),
		defender(defender),
		goalkeeper(goalkeeper),
		ball(ball),
		ball_est(ball_est) {
}

void Strategy2::run() {
	duration_ms since_last_transition = sc::now() - last_transition;
	if(since_last_transition.count() > 2000) {
		bool transitioned = transitions();
		if(transitioned)
			last_transition = sc::now();
	}

	execute_goalkeeper();
	execute_defender();
	execute_attacker();

	if (at_location(goalkeeper, Location::TheirField)) {
		attacker.go_to(ball);
	} else {
		auto new_limit = defender::back::upper_limit.x + 0.09;

		if (at_location(attacker.get_position(), Location::OurBox)) {
			attacker.go_in_direction({0, 0.8});
		} else if (at_location(attacker.get_target().position, Location::OurBox)) {
			attacker.stop();
		} else if (attacker.get_target().position.x < new_limit && attacker.get_position().x > new_limit) {
			attacker.stop();
		}

		if (at_location(defender.get_position(), Location::OurBox)) {
			defender.go_in_direction({0, 0.8});
		} if (at_location(defender.get_target().position, Location::OurBox)) {
			defender.stop();
		}
	}
}

bool Strategy2::transitions() {
	bool collided = trainstion_by_collision();

	Point goal = their::goal::back::center;

	Vector ball_to_goal = goal - ball;
	Vector robot_to_ball = ball - defender.get_position();
	double orientation_error_def = std::abs(wrap(robot_to_ball.theta - ball_to_goal.theta));
	double orientation_error_atk = std::abs(wrap(robot_to_ball.theta - ball_to_goal.theta));

	auto diferent_sides_up = at_location(ball, Location::UpperField) && at_location(defender, Location::UpperField)
			&& at_location(attacker, Location::LowerField);
	auto diferent_sides_down = at_location(ball, Location::LowerField) && at_location(defender, Location::LowerField)
							 && at_location(attacker, Location::UpperField);
	auto distancey = distance_y(attacker.get_position(), defender.get_position());
	auto distancex = distance_x(attacker.get_position(), defender.get_position());
	if (
			((distance(attacker.get_position(), ball) > distance(defender.get_position(), ball)) && distancey > 0.6)
//			|| (diferent_sides_up || diferent_sides_down)
			&& (orientation_error_def + degree_to_rad(10) < orientation_error_atk)
//			&& defender.get_position().x > 0.3*field_height
			)
		swap_robots(attacker, defender);

		// Cruzamento
	else if (at_location(ball, Location::TheirBox) &&
		distance(attacker.get_position(), ball) > distance(defender.get_position(), ball))
		swap_robots(attacker, defender);


	//Se a bola está bem atrás do atacante mas está na frente do defensor
	else if (distance_x(attacker.get_position(), ball) > 0.30 && is_ball_behind(attacker) &&
		!at_location(attacker, Location::TheirCornerAny))
		swap_robots(attacker, defender);

	//Se o defensor está mais perto da bola do que o atacante
	else if (distance(attacker.get_position(), ball) > distance(defender.get_position(), ball) &&
		!at_location(attacker, Location::TheirCornerAny))
		swap_robots(attacker, defender);


	//Caso a bola esteja atrás do defensor e do atacante
//	else if (is_ball_behind(attacker) && is_ball_behind(defender) && at_location(ball, Location::WideDangerZone))
//		swap_all_robots();
	else
		return false | collided;

	return true;
}



bool Strategy2::trainstion_by_collision() {
	if (is_collision(goalkeeper, defender)) {
		if (is_opposite_direction(goalkeeper, defender)) {
			swap_robots(goalkeeper, defender);
			std::cout << "troca de goleiro e defensor" << std::endl;
		}
	} else if (is_collision(defender, attacker)) {
		if (is_opposite_direction(defender, attacker)) {
			swap_robots(defender, attacker);
			std::cout << "troca de defensor e atacante" << std::endl;
		}
	} else if (is_collision(goalkeeper, attacker)) {
		if (is_opposite_direction(goalkeeper, attacker)) {
			swap_robots(goalkeeper, attacker);
			std::cout << "troca de goleiro e atacante" << std::endl;
		}
	} else {
		return false;
	}

	return true;
}

void Strategy2::execute_goalkeeper() {
	if (distance(goalkeeper.get_position(), ball) < goalkeeper.BALL_OFFSET && !at_location(goalkeeper, Location::AnyGoal))
		goalkeeper.spin_shot(ball);
	else if (at_location(goalkeeper, Location::AnyGoal))
		goalkeeper.exit_goal();
	else
		goalkeeper.protect_goal(ball, ball_est);
}

void Strategy2::execute_defender() {
	if(attacker.get_position().x < 0.3 * field_width) {
		defender.stop();
		} else if (is_ball_est_ahead(their::area::front::center, -0.08) && !at_location(defender, Location::AnyGoal)) {
			// Bola na na área adversária
			if (at_location(ball, Location::UpperField))
				defender.wait_at_target(defender::front::lower::wait_point, ball);
			else
				defender.wait_at_target(defender::front::upper::wait_point, ball);

	} else if (at_location(defender, Location::AnyGoal)) {
		defender.exit_goal();
	} else {
		defender.protect_goal(ball);
	}
}

void Strategy2::execute_attacker() {
	if ((ball.x < 0.3 * field_width) && attacker.get_position().x > 0.3 * field_width) {
		if (at_location(defender, Location::UpperField)) {
			auto pos = defender.get_position().y - 0.5;
			auto ynew = std::min(0.1, pos);
			attacker.go_to_and_stop({0.31 * field_width, ynew});
		} else {
			auto pos = defender.get_position().y + 0.15;
			auto ynew = std::min(field_height - 0.5, pos);
			attacker.go_to_and_stop({0.31 * field_width, ynew});
		}
	} else
		if (at_location(attacker, Location::TheirCornerAny)) {
		attacker.crossing(ball);
	} else if (has_ball(attacker) && at_location(attacker, Location::TheirAreaSideAny)) {
		attacker.spin_shot(ball);
	} else if (at_location(attacker, Location::OurBox)) {
		// Cobrar penalti
		attacker.charged_shot(ball);
	} else if (is_ball_behind(our::area::front::center) && !at_location(attacker, Location::AnyGoal)) {
		attacker.protect_goal(ball);
	} else if (at_location(ball, Location::AnySide)){
		attacker.side_spin_shot(ball);
	} else if (at_location(attacker, Location::AnyGoal)) {
		attacker.exit_goal(ball);
	} else {
		attacker.uvf_to_goal(ball, ball_est);
	}

}

void Strategy2::swap_robots(Robot2& robot1, Robot2& robot2)
{
	const auto pose = robot1.get_pose();
	const auto ID = robot1.get_ID();
	const auto tag = robot1.tag;

	robot1.set_pose(robot2.get_pose());
	robot1.set_ID(robot2.get_ID());
	robot1.tag = robot2.tag;

	robot2.set_pose(pose);
	robot2.set_ID(ID);
	robot2.tag = tag;
}

void Strategy2::swap_all_robots() {
	// antigo goalkeeper -> atacker / antigo defender -> goalkeeper / antigo attacker -> defender
//	swap_robots(defender, attacker);
//	swap_robots(attacker, goalkeeper);
}

bool Strategy2::is_ball_behind(const Point& point) {
	return ball.x < point.x;
}

bool Strategy2::is_ball_behind(const Robot2& robot) {
	return is_ball_behind(robot.get_position());
}

bool Strategy2::is_ball_est_ahead(const Point& point, double offset) {
	return ball_est.x > point.x + offset;
}

bool Strategy2::has_ball(const Robot2& robot) {
	return distance(robot.get_position(), ball) < robot.BALL_OFFSET;
}

bool in_range(Robot2 &this_robot, Robot2 &other_robot) {
//	auto max_theta = degree_to_rad(70);
	auto robot1_to_robot2 = this_robot.get_position() - other_robot.get_position();
	auto theta_error = std::abs(wrap(this_robot.get_pose().orientation - robot1_to_robot2.theta));
	return distance(this_robot.get_position(), other_robot.get_position()) < 0.12 && theta_error < degree_to_rad(60);
}

bool Strategy2::is_collision(Robot2 &robot1, Robot2 &robot2) {
	return distance(robot1.get_position(), robot2.get_position()) < 0.12f;
	return is_collision_axis_y(robot1, robot2) && is_collision_axis_x(robot1, robot2);
}

bool Strategy2::is_collision_axis_x(Robot2 &robot1, Robot2 &robot2) {
	double robot1_y = robot1.get_position().y;
	double robot2_y = robot2.get_position().y;

	double robot1_y_start = robot1_y - robot1.SIZE / 2;
	double robot1_y_end   = robot1_y + robot1.SIZE / 2;

	double robot2_y_start = robot2_y - robot2.SIZE / 2;
	double robot2_y_end   = robot2_y + robot2.SIZE / 2;

	return (robot1_y_start >= robot2_y_start && robot1_y_start <= robot2_y_end) ||
		(robot1_y_end >= robot2_y_start && robot1_y_start <= robot2_y_end);
}

bool Strategy2::is_collision_axis_y(Robot2 &robot1, Robot2 &robot2) {
	double robot1_x = robot1.get_position().x;
	double robot2_x = robot2.get_position().x;

	double robot1_x_start = robot1_x - robot1.SIZE / 2;
	double robot1_x_end   = robot1_x + robot1.SIZE / 2;

	double robot2_x_start = robot2_x - robot2.SIZE / 2;
	double robot2_x_end   = robot2_x + robot2.SIZE / 2;

	return (robot1_x_start >= robot2_x_start && robot1_x_start <= robot2_x_end) ||
		   (robot1_x_end >= robot2_x_start && robot1_x_start <= robot2_x_end);
}

bool Strategy2::is_opposite_direction(Robot2 &robot1, Robot2 &robot2) {
	auto robot1_dir = robot1.get_direction();
	if(robot1_dir.size > 0) {
		auto robot1_to_robot2 = robot2.get_position() - robot1.get_position();
		return std::abs(wrap(robot1_to_robot2.theta - robot1_dir.theta)) < degree_to_rad(90);
	}

	auto robot2_dir = robot2.get_direction();
	if (robot2_dir.size > 0) {
		auto robot2_to_robot1 = robot1.get_position() - robot2.get_position();
		return std::abs(wrap(robot2_to_robot1.theta - robot2_dir.theta)) < degree_to_rad(90);
	}

	return false;
}