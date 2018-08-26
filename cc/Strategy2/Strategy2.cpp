#include "Strategy2.hpp"

using namespace field;
using namespace Geometry;

Strategy2::Strategy2(Attacker &attacker, Defender &defender, Goalkeeper& goalkeeper, Geometry::Point &ball) :
		attacker(attacker), defender(defender), goalkeeper(goalkeeper), ball(ball) {
}

void Strategy2::run() {

	checkForTransitions();
}

void Strategy2::checkForTransitions() {

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
	double theta = abs(def_to_goal.theta - ball_est_to_goal.theta);
	if (theta <= degree_to_rad(20))
		swap_robots(attacker, defender);

}

void Strategy2::swap_robots(Robot2& robot1, Robot2& robot2)
{
	const Robot2::Pose pose = robot1.get_pose();
	const char ID = robot1.get_ID();

	robot1.set_pose(robot2.get_pose());
	robot1.set_ID(robot2.get_ID());

	robot2.set_pose(pose);
	robot2.set_ID(ID);
}

void Strategy2::swap_all_robots() {
	// antigo goalkeeper -> atacker / antigo defender -> goalkeeper / antigo attacker -> defender
	swap_robots(goalkeeper, attacker);
	swap_robots(defender, goalkeeper);
}

bool Strategy2::is_ball_behind(const Point& point) {
	return ball.x < point.x;
}

bool Strategy2::is_ball_behind(const Robot2& robot) {
	return is_ball_behind(robot.get_position());
}
