#include "Strategy3.hpp"
#include "Field.h"
#include "helper.hpp"
#include <algorithm>
#include <array>

using namespace field;
using namespace Geometry;

Robot3 * find_robot_by_role(std::vector<Robot3> &robots, Role role) {
	auto robot_it = std::find_if(robots.begin(), robots.end(), [&role](const Robot3& robot){
		return robot.role == role;
	});
	if (robot_it == robots.end()) {
		return nullptr;
	} else {
		return &*robot_it;
	}
}

std::array<std::array<int, 3>, 6> all_possible_permutations() {
    std::array<int, 3> robot_ids = {0, 1, 2};
    std::array<std::array<int, 3>, 6> permutations;
    int i = 0;
    do {
        permutations[i] = robot_ids;
        i += 1;
    } while (std::next_permutation(std::begin(robot_ids), std::end(robot_ids)));
    return permutations;
}

double score_atacker(const Robot3& robot, const Ball& ball) {
    Point goal = their::goal::back::center;
    Vector ball_to_goal = goal - ball.position;
	Vector robot_to_ball = ball.position - robot.get_position();
    return (robot.pose.position - ball.position).size
        + 0.2 * std::abs(wrap(robot_to_ball.theta - ball_to_goal.theta));
}

double score_goalkeeper(const Robot3& robot, const Ball& ball) {
    auto upper_goal = field::our::goal::front::upper_limit;
	auto lower_goal = field::our::goal::front::lower_limit;
    auto goalkeeper_y = ball.position.y;
    goalkeeper_y = std::max(goalkeeper_y, lower_goal.y);
    goalkeeper_y = std::min(goalkeeper_y, upper_goal.y);
    auto target = Point(field::our::goal::back::center.x, goalkeeper_y);
    return (robot.pose.position - field::our::goal::front::center).size;
}

double score_formation(std::array<int, 3> formation, std::vector<Robot3> &team, Ball& ball) {
    return score_atacker(team[formation[0]], ball)
        - 1.2 * score_goalkeeper(team[formation[0]], ball)
        + 1.2 * score_goalkeeper(team[formation[1]], ball);
//         + 0.5 * (score_goalkeeper(team[formation[1]], ball) * (team[formation[0]].pose.position - team[formation[1]].pose.position).size);
}

void Strategy3::set_default_formation(std::vector<Robot3> &team, Ball ball) {
	for (auto& robot : team) {
		robot.role = Role::Defender;
	}
	auto& closest_to_goal = *std::min_element(team.begin(), team.end(), [&](Robot3& r1, Robot3& r2) {
		return (r1.pose.position - field::our::goal::front::center).size < (r2.pose.position - field::our::goal::front::center).size;
	});
	closest_to_goal.role = Role::Goalkeeper;
	auto& closest_to_ball = *std::min_element(team.begin(), team.end(), [&](Robot3& r1, Robot3& r2) {
		if (r1.role == Role::Goalkeeper) return false;
		else if (r2.role == Role::Goalkeeper) return true; 
		else  return score_atacker(r1, ball) < score_atacker(r2, ball);
	});
	closest_to_ball.role = Role::Attacker;
}

void Strategy3::set_foul(VSSRef::Foul foul) {
	new_foul = true;
}

void Strategy3::run_strategy(std::vector<Robot3> &team, std::vector<Geometry::Point> &adversaries, Ball ball,
							 bool first_iteration) {

    auto vec = all_possible_permutations();
//     for (auto v : vec) {
//         std::cout << v[0] << ", " << v[1] <<  ", " << v[2] << std::endl;
//     }
//     std::cout << "end" << std::endl;

    auto best_formation = *std::min_element(vec.begin(), vec.end(), [&](std::array<int, 3> f1, std::array<int, 3> f2) {
        return score_formation(f1, team, ball) < score_formation(f2, team, ball);
	});
    attacker.set_robot(&team[best_formation[0]]);
    goalkeeper.set_robot(&team[best_formation[1]]);
    defender.set_robot(&team[best_formation[2]]);


	// if (new_foul) {
//     if (attacker.has_robot() && defender.has_robot() && goalkeeper.has_robot())
// 		set_default_formation(team, ball);
		// new_foul = false;
	// }

	Robot3* attacker_robot = find_robot_by_role(team, Role::Attacker);
	attacker.set_robot(attacker_robot);
	Robot3* defender_robot = find_robot_by_role(team, Role::Defender);
	defender.set_robot(defender_robot);
	Robot3* goalkeeper_robot = find_robot_by_role(team, Role::Goalkeeper);
	goalkeeper.set_robot(goalkeeper_robot);
	this->ball = ball;

// 	duration_ms since_last_transition = sc::now() - last_transition;
// 	if(since_last_transition.count() > 2000) {
// 		bool transitioned = transitions();
// 		if(transitioned)
// 			last_transition = sc::now();
// 	}
    if ((attacker->pose.position - ball.position).size > 0.3) {
        attacker->control.obstacles = adversaries;
    } else {
        attacker->control.obstacles.clear();
    }
    attacker->control.obstacles.push_back(Point(attacker->pose.position.x, 0));
    attacker->control.obstacles.push_back(Point(attacker->pose.position.x, field_height));
    if (attacker->target.command == Command::UVF) {
        attacker->control.obstacles.push_back(attacker->target.reference);
    }

//     auto atk_obs = adversaries;
    attacker->control.obstacles.push_back(defender->pose.position);
    attacker->control.obstacles.push_back(goalkeeper->pose.position);
//     attacker->control.obstacles = atk_obs;

    defender->control.obstacles = adversaries;
//     defender->control.obstacles.clear();
//     auto df_obs = adversaries;
    defender->control.obstacles.push_back(attacker->pose.position);
    defender->control.obstacles.push_back(goalkeeper->pose.position);
//     defender->control.obstacles = df_obs;

    goalkeeper->control.obstacles.clear();

 	if (attacker.has_robot()) {
// 		if (defender.has_robot() &&
// 				(ball.position.x < 0.3 * field_width) &&
// 				attacker->get_position().x > 0.3 * field_width) {
//
// 			if (at_location(defender->get_position(), Location::UpperField)) {
// 				auto pos = defender->get_position().y - 0.5;
// 				auto ynew = std::min(0.1, pos);
// 				attacker->go_to_and_stop({0.31 * field_width, ynew});
// 			} else {
// 				auto pos = defender->get_position().y + 0.15;
// 				auto ynew = std::min(field_height - 0.5, pos);
// 				attacker->go_to_and_stop({0.31 * field_width, ynew});
// 			}
// 		} else {
			attacker.run_strategy(ball);
// 		}

// 		if (goalkeeper.has_robot() && at_location(*goalkeeper.robot, Location::TheirField)) {
//			cobrança de penalti
// 			attacker->go_to(ball.position);
// 		} else {
// 			auto new_limit = defender::back::upper_limit.x + 0.09;

// 			if (at_location(attacker->get_position(), Location::OurBox)) {
//                 std::cout << "here" << std::endl;
// 				attacker->go_in_direction({0, 0.8});
// 			} else
            auto cmd = attacker->target.command;
            if (!at_location(attacker->pose.position, Location::OurBox)) {
                if (cmd == Command::Position || cmd == Command::Vector) {
                    if (at_location(attacker->target.pose.position, Location::OurBox)) {
                        attacker->stop();
                    }
                } else if (cmd == Command::UVF) {
                    auto target_to_reference = attacker->target.reference - attacker->target.pose.position;
                    auto target = attacker->target.pose.position + target_to_reference.with_size(-0.1);
                    if (at_location(attacker->target.pose.position, Location::OurBox) || at_location(target, Location::OurBox)) {
                        attacker->stop();
                    }
                }
            }

// 			} else if (attacker->target.pose.position.x < new_limit && attacker->get_position().x > new_limit) {
// 				attacker->stop();
// 			}
// 		}
	}

	if (defender.has_robot()) {
// 		if (attacker.has_robot() && attacker->get_position().x < 0.3 * field_width) {
// 			defender->stop();
// 		} else {
			defender.run_strategy(ball);
// 		}

// 		if (at_location(defender->get_position(), Location::OurBox)) {
// 			defender->go_in_direction({0, 0.8});
// 		}
		if (at_location(defender->target.pose.position, Location::OurBox)) {
			defender->stop();
		}
	}

	if (goalkeeper.has_robot()) goalkeeper.run_strategy(ball);
}

bool Strategy3::transitions() {
	bool collided = false;


//	Todas as transições a seguir são entre atacantes e defensores
	if (!attacker.has_robot() || !defender.has_robot()) return false;

	Point goal = their::goal::back::center;

	Vector ball_to_goal = goal - ball.position;
	Vector robot_to_ball = ball.position - defender->get_position();
	double orientation_error_def = std::abs(wrap(robot_to_ball.theta - ball_to_goal.theta));
	double orientation_error_atk = std::abs(wrap(robot_to_ball.theta - ball_to_goal.theta));

	auto diferent_sides_up = at_location(ball.position, Location::UpperField) && at_location(defender->get_position(), Location::UpperField)
							 && at_location(attacker->get_position(), Location::LowerField);
	auto diferent_sides_down = at_location(ball.position, Location::LowerField) && at_location(defender->get_position(), Location::LowerField)
							   && at_location(attacker->get_position(), Location::UpperField);
	auto distancey = distance_y(attacker->get_position(), defender->get_position());
	auto distancex = distance_x(attacker->get_position(), defender->get_position());
	if (
			((distance(attacker->get_position(), ball.position) > distance(defender->get_position(), ball.position)) && distancey > 0.6)
			//			|| (diferent_sides_up || diferent_sides_down)
			&& (orientation_error_def + degree_to_rad(10) < orientation_error_atk)
//			&& defender.get_position().x > 0.3*field_height
			)
		swap_robots(attacker, defender);

		// Cruzamento
	else if (at_location(ball.position, Location::TheirBox) &&
			 distance(attacker->get_position(), ball.position) > distance(defender->get_position(), ball.position))
		swap_robots(attacker, defender);


		//Se a bola está bem atrás do atacante mas está na frente do defensor
	else if (distance_x(attacker->get_position(), ball.position) > 0.30 && is_ball_behind(attacker->get_position(), ball) &&
			 !at_location(attacker->get_position(), Location::TheirCornerAny))
		swap_robots(attacker, defender);

		//Se o defensor está mais perto da bola do que o atacante
	else if (distance(attacker->get_position(), ball.position) > distance(defender->get_position(), ball.position) &&
			 !at_location(attacker->get_position(), Location::TheirCornerAny))
		swap_robots(attacker, defender);


		//Caso a bola esteja atrás do defensor e do atacante
//	else if (is_ball_behind(attacker) && is_ball_behind(defender) && at_location(ball, Location::WideDangerZone))
//		swap_all_robots();
	else
		return false | collided;

	return true;
}



bool Strategy3::trainstion_by_collision() {
	if (is_collision(goalkeeper.robot, defender.robot)) {
		if (is_opposite_direction(goalkeeper.robot, defender.robot)) {
			swap_robots(goalkeeper, defender);
			std::cout << "troca de goleiro e defensor" << std::endl;
		}
	} else if (is_collision(defender.robot, attacker.robot)) {
		if (is_opposite_direction(defender.robot, attacker.robot)) {
			swap_robots(defender, attacker);
			std::cout << "troca de defensor e atacante" << std::endl;
		}
	} else if (is_collision(goalkeeper.robot, attacker.robot)) {
		if (is_opposite_direction(goalkeeper.robot, attacker.robot)) {
			swap_robots(goalkeeper, attacker);
			std::cout << "troca de goleiro e atacante" << std::endl;
		}
	} else {
		return false;
	}

	return true;
}

void Strategy3::swap_robots(RoleStrategy& role1, RoleStrategy& role2) {
	if (!role1.has_robot() || !role2.has_robot()) return;

	Robot3 * aux_robot1 = role1.robot;
	Role aux_role1 = role1->role;

	role1.robot = role2.robot;
	role1.robot->role = role2.robot->role;

	role2.robot = aux_robot1;
	role2->role = aux_role1;
}

void Strategy3::swap_all_robots() {
	// antigo goalkeeper -> atacker / antigo defender -> goalkeeper / antigo attacker -> defender
//	swap_robots(defender, attacker);
//	swap_robots(attacker, goalkeeper);
}

bool in_range(Robot3 &this_robot, Robot3 &other_robot) {
//	auto max_theta = degree_to_rad(70);
	auto robot1_to_robot2 = this_robot.get_position() - other_robot.get_position();
	auto theta_error = std::abs(wrap(this_robot.pose.orientation - robot1_to_robot2.theta));
	return distance(this_robot.get_position(), other_robot.get_position()) < 0.12 && theta_error < degree_to_rad(60);
}

bool Strategy3::is_collision(Robot3 *robot1, Robot3* robot2) {
	if (robot1 == nullptr || robot2 == nullptr) return false;
	return distance(robot1->get_position(), robot2->get_position()) < 0.12f;
	return is_collision_axis_y(robot1, robot2) && is_collision_axis_x(robot1, robot2);
}

bool Strategy3::is_collision_axis_x(Robot3 *robot1, Robot3 *robot2) {
	if (robot1 == nullptr || robot2 == nullptr) return false;

	double robot1_y = robot1->get_position().y;
	double robot2_y = robot2->get_position().y;

	double robot1_y_start = robot1_y - robot1->SIZE / 2;
	double robot1_y_end   = robot1_y + robot1->SIZE / 2;

	double robot2_y_start = robot2_y - robot2->SIZE / 2;
	double robot2_y_end   = robot2_y + robot2->SIZE / 2;

	return (robot1_y_start >= robot2_y_start && robot1_y_start <= robot2_y_end) ||
		   (robot1_y_end >= robot2_y_start && robot1_y_start <= robot2_y_end);
}

bool Strategy3::is_collision_axis_y(Robot3 *robot1, Robot3 *robot2) {
	if (robot1 == nullptr || robot2 == nullptr) return false;

	double robot1_x = robot1->get_position().x;
	double robot2_x = robot2->get_position().x;

	double robot1_x_start = robot1_x - robot1->SIZE / 2;
	double robot1_x_end   = robot1_x + robot1->SIZE / 2;

	double robot2_x_start = robot2_x - robot2->SIZE / 2;
	double robot2_x_end   = robot2_x + robot2->SIZE / 2;

	return (robot1_x_start >= robot2_x_start && robot1_x_start <= robot2_x_end) ||
		   (robot1_x_end >= robot2_x_start && robot1_x_start <= robot2_x_end);
}

bool Strategy3::is_opposite_direction(Robot3 *robot1, Robot3 *robot2) {
	if (robot1 == nullptr || robot2 == nullptr) return false;

	auto robot1_dir = robot1->get_direction();
	if(robot1_dir.size > 0) {
		auto robot1_to_robot2 = robot2->get_position() - robot1->get_position();
		return std::abs(wrap(robot1_to_robot2.theta - robot1_dir.theta)) < degree_to_rad(90);
	}

	auto robot2_dir = robot2->get_direction();
	if (robot2_dir.size > 0) {
		auto robot2_to_robot1 = robot1->get_position() - robot2->get_position();
		return std::abs(wrap(robot2_to_robot1.theta - robot2_dir.theta)) < degree_to_rad(90);
	}

	return false;
}
