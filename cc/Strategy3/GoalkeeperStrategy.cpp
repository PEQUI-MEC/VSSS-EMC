#include "GoalkeeperStrategy.hpp"
#include "Field.h"

using namespace field;
using namespace Geometry;

void GoalkeeperStrategy::run_strategy(const Ball &ball, const std::vector<Adversary> &adversaries) {
	if (penalty)
		defend_penalty(ball, adversaries);
// 	else if (distance(robot->get_position(), ball.position) < robot->BALL_OFFSET && !at_location(robot->get_position(), Location::AnyGoal))
//         spin_shot(ball.position);
	else if (at_location(robot->get_position(), Location::AnyGoal))
		exit_goal();
	//else if (at_location(ball.position, Location::GoalkeeperCorner))
//		handle_corner(ball);
	else
		protect_goal(ball, adversaries);
}

Geometry::Point get_attacker_on_penalty(const Ball &ball, const std::vector<Adversary> &adversaries) {
	Geometry::Point atk = adversaries[0].position;
	for (auto& adv : adversaries) {
		if (distance(ball.position, adv.position) < distance(ball.position, atk)) {
			atk = adv.position;
		}
	}
	return atk;
}

std::optional<Geometry::Point> get_attacker(const Ball &ball, const std::vector<Adversary> &adversaries) {
	Point goal = our::goal::back::center;
	auto upper_goal = our::goal::front::upper_limit;
	auto lower_goal = our::goal::front::lower_limit;
	Vector ball_to_upper = upper_goal - ball.position;
	Vector ball_to_lower = lower_goal - ball.position;
	Vector ball_to_goal = goal - ball.position;
	std::optional<Geometry::Point> atk = std::nullopt;
	for (auto& adv : adversaries) {
		Vector adv_to_ball = ball.position - adv.position;
		auto can_run_to_goal = adv_to_ball.angle_to(ball_to_upper) < degree_to_rad(15) &&
							adv_to_ball.angle_to(ball_to_lower) > degree_to_rad(-15);
		if (can_run_to_goal && std::abs(adv.velocity.angle_to(adv_to_ball)) < degree_to_rad(45) && adv.velocity.size > 0.1) {
			//std::cout << "has atk"  << std::endl;
			if (atk == std::nullopt || (distance(*atk, ball.position) > distance(adv.position, ball.position))) {
				atk = adv.position;
			}
		}

	}
	return atk;
}

void GoalkeeperStrategy::defend_penalty(const Ball& ball, const std::vector<Adversary> &adversaries) {
	auto atk = get_attacker_on_penalty(ball, adversaries);
	auto kick_dir = ball.position - atk;
	auto dist_x_ball_goal = std::abs(ball.position.x - gk_line_x);
	auto distance_ball_projection = dist_x_ball_goal / std::cos(M_PI - kick_dir.theta);
	auto ball_goal_projection = ball.position + kick_dir.with_size(distance_ball_projection);
	double MAX_GOAL_Y = our::goal::front::center.y + 0.15;
	double MIN_GOAL_Y = our::goal::front::center.y - 0.15;
	if (ball_goal_projection.y > MAX_GOAL_Y) {
		ball_goal_projection.y = MAX_GOAL_Y;
	} else if (ball_goal_projection.y < MIN_GOAL_Y) {
		ball_goal_projection.y = MIN_GOAL_Y;
	}
	Point target{penalty_gk_line_x, ball_goal_projection.y};
	if (has_arrived_penalty || distance(robot->get_position(), target) < SPIN_THRESHOLD) {
		has_arrived_penalty = true;
		robot->spin_kick_to_target(ball.position, their::goal::front::center);
	} else {
		robot->go_to_and_stop_orientation(target, M_PI/2);
	}
}

Geometry::Point get_reach_line_intersection(const Ball& ball) {
	auto dist_x_ball_reach_line = distance_x(ball.position, goalkeeper::reach_line);
	auto distance_ball_projection = dist_x_ball_reach_line / std::cos(M_PI - ball.velocity.theta);
	auto ball_reach_line_projection = ball.position + ball.velocity.with_size(distance_ball_projection);
	return ball_reach_line_projection;
}

void GoalkeeperStrategy::protect_goal(const Ball& ball, const std::vector<Adversary> &adversaries) {
	auto dist_x_ball_goal = distance_x(ball.position, goalkeeper::center);
	auto distance_ball_projection = dist_x_ball_goal / std::cos(M_PI - ball.velocity.theta);
	auto ball_goal_projection = ball.position + ball.velocity.with_size(distance_ball_projection);

	Geometry::Point target = Geometry::Point{gk_line_x, ball.position.y};

	const double FAST_BALL_LIMIT = 0.2;
	bool fast_ball_to_goal = std::abs(ball.velocity.theta) > M_PI/2 && ball.velocity.size > FAST_BALL_LIMIT;
	bool slow_ball_to_goal = std::abs(ball.velocity.theta) > M_PI/2 && ball.velocity.size > 0.005
		&& ball.velocity.size < FAST_BALL_LIMIT;

	if (slow_ball_to_goal) {
		target = ball_goal_projection;
	}

	// if (auto atk = get_attacker(ball, adversaries)) {
	// 	Vector atk_to_ball = ball.position - *atk;
	// 	auto distance_atk_ball_projection = dist_x_ball_goal / std::cos(M_PI - atk_to_ball.theta);
	// 	auto atk_ball_projection = ball.position + atk_to_ball.with_size(distance_atk_ball_projection);
	// 	target = atk_ball_projection;
	// }

	// If ball is fast, go to ball projection instead of atk projection
	if (fast_ball_to_goal) {
		target = ball_goal_projection;
	}

	if (at_location(ball.position, Location::GoalkeeperCorner)) {
		if (ball.position.y > goalkeeper::center.y)
			target = goalkeeper::upper_limit;
		else
			target = goalkeeper::lower_limit;
	} else if (target.y > goalkeeper::goal_upper_limit.y) {
		target = goalkeeper::goal_upper_limit;
	} else if (target.y < goalkeeper::goal_lower_limit.y) {
		target = goalkeeper::goal_lower_limit;
	}

	auto tangent = ball.velocity.tangent_to_circle(ball.position, robot->get_position());
	//std::cout << ball.velocity.theta << std::endl;
	//std::cout << tangent << std::endl;
	//ball.position_in_seconds(1).x < target.x)
	// std::abs(ball.velocity.theta) > M_PI/2
	auto ball_to_robot = robot->get_position() - ball.position;
	bool should_spin = (distance(robot->get_position(), tangent) < reach_ball_distance
							&& distance(robot->get_position(), target) < 0.03
							&& std::abs(ball_to_robot.angle_to(ball.velocity)) < M_PI/2
							&& (distance(ball.position, ball.position_in_seconds(1.0)) > distance(ball.position, robot->get_position())))
						|| distance(robot->get_position(), ball.position) < reach_ball_distance;
	if (should_spin) {
		robot->spin_kick_to_target(ball.position, their::goal::front::center);
	} else {
		robot->go_to_and_stop_orientation(target, M_PI/2);
	}

	/**
	Geometry::Point reach_line_intersection = get_reach_line_intersection(ball);

	bool should_spin = (distance(reach_line_intersection, robot->get_position()) < 0.1 && ball.position_in_seconds(0.6).x < target.x) ||
		distance(robot->get_position(), ball.position) < reach_ball_distance ||
		(distance(robot->get_position(), target) < 0.02 && ball.position_in_seconds(0.6).x < target.x &&			distance(ball_goal_projection, target) < 0.08);

	if (should_spin) {
		robot->spin_kick_to_target(ball.position, their::goal::front::center);
	} else {
		robot->go_to_and_stop_orientation(target, M_PI/2);
	}
	**/
}
/**
void GoalkeeperStrategy::protect_goal(const Ball& ball) {
	if (at_location(ball.position, Location::OurUpperCorner)) {
		robot->go_to_and_stop_orientation(goalkeeper::upper_limit, M_PI/2);
	} else if (at_location(ball.position, Location::OurLowerCorner)) {
		robot->go_to_and_stop_orientation(goalkeeper::lower_limit, M_PI/2);
	} else if (std::abs(ball.velocity.theta) > M_PI/2
                && ball.velocity.size > 0.0) {
		// a bola está vindo para o nosso campo

		// Se goal_est estiver na pequena área, fica na projeção da estimativa da bola
		// se não fica no limite da pequena área
        auto dist_x_ball_goal = distance_x(ball.position, our::area::lower::center);
        auto distance_ball_projection = dist_x_ball_goal / std::cos(M_PI - ball.velocity.theta);
        auto ball_goal_projection = ball.position + ball.velocity.with_size(distance_ball_projection);
		if (match_y(ball_goal_projection, Location::OurBox)) {
            Point target{goalkeeper::upper_limit.x, ball_goal_projection.y};
            if (distance(robot->get_position(), target) < 0.02
                && (distance(ball.position, target) < 0.25 || ball.position_in_seconds(0.6).x < target.x)) {
                robot->spin_kick_to_target(ball.position, their::goal::front::center);
            } else {
                robot->go_to_and_stop_orientation(target, M_PI/2);
            }
        }
		else if (at_location(ball_goal_projection, Location::UpperField))
			robot->go_to_and_stop_orientation(goalkeeper::upper_limit, M_PI/2);
		else
			robot->go_to_and_stop_orientation(goalkeeper::lower_limit, M_PI/2);
	} else {
		// fica na projeção da bola, sempre em frente ao gol
		if (match_y(ball.position, Location::OurBox))
			robot->go_to_and_stop_orientation({our::area::box::upper_limit.x, ball.position.y}, M_PI/2);
		else if (at_location(ball.position, Location::UpperField))
			robot->go_to_and_stop_orientation(goalkeeper::upper_limit, M_PI/2);
		else
			robot->go_to_and_stop_orientation(goalkeeper::lower_limit, M_PI/2);
	}
}
**/

void GoalkeeperStrategy::handle_corner(const Ball &ball) {
	bool should_spin = distance(robot->get_position(), ball.position) < reach_ball_distance;
	if (should_spin) {
		robot->spin_kick_to_target(ball.position, their::goal::front::center);
	} else if (ball.position.y > goalkeeper::center.y) {
		robot->go_to_and_stop_orientation(goalkeeper::upper_limit, M_PI/2);
	} else {
		robot->go_to_and_stop_orientation(goalkeeper::lower_limit, M_PI/2);
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
