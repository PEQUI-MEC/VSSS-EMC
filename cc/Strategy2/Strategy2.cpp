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
//	transitions();

	execute_goalkeeper();
	execute_defender();
	execute_attacker();
}

void Strategy2::transitions() {

	// Cruzamento
	if (at_location(ball, Location::TheirBox) && distance(attacker.get_position(), ball) > 0.12)
		swap_robots(attacker, defender);

	// Se a bola está bem atrás do atacante mas está na frente do defensor
	if (distance_x(attacker.get_position(), ball) > 0.36 && is_ball_behind(attacker) &&
		!at_location(attacker, Location::TheirCornerAny))
		swap_robots(attacker, defender);

	// Caso a bola esteja atrás do defensor e do atacante
	if (is_ball_behind(attacker) && is_ball_behind(defender) && at_location(ball, Location::WideDangerZone))
		swap_all_robots();

	// Caso o angulo do defensor para bola em direção ao gol do oponente for bom
	Vector def_to_goal = their::goal::front::center - defender.get_position();
	Vector ball_est_to_goal = their::goal::front::center - ball_est;
	double theta = std::abs(def_to_goal.theta - ball_est_to_goal.theta);
	if (theta <= degree_to_rad(20))
		swap_robots(attacker, defender);

}

void Strategy2::execute_goalkeeper() {
	if (distance(goalkeeper.get_position(), ball) < 0.10)
		goalkeeper.spin_shot(ball);
	else
		goalkeeper.protect_goal(ball, ball_est);
}

void Strategy2::execute_defender() {
	if (is_ball_est_ahead(their::area::front::center, -0.08)) {
		// Bola na na área adversária
		if (at_location(ball, Location::UpperField))
			defender.wait_at_target(defender::front::upper::wait_point, ball);
		else
			defender.wait_at_target(defender::front::lower::wait_point, ball);
	} else {
		defender.protect_goal(ball);
	}
}

void Strategy2::execute_attacker() {
	if (is_ball_behind(our::area::front::center)) {
		attacker.protect_goal(ball);
	} else if (at_location(attacker, Location::TheirCornerAny)) {
		attacker.crossing(ball);
	} else if (has_ball(attacker) && at_location(attacker, Location::TheirAreaSideAny)) {
		attacker.spin_shot(ball);
	} else if (at_location(attacker, Location::OurBox)) {
		// Cobrar penalti
		attacker.charged_shot(ball);
	} else if (at_location(ball, Location::AnySide)){
		attacker.side_spin_shot(ball);
	} else {
		attacker.uvf_to_goal(ball);
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
	swap_robots(defender, attacker);
	swap_robots(attacker, goalkeeper);
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
	return distance(robot.get_position(), ball) < 0.08;
}
