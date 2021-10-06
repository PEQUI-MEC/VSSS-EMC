#include "helper.hpp"

bool has_ball(const Robot3 *robot, const Ball &ball) {
	return distance(robot->get_position(), ball.position) < robot->BALL_OFFSET;
}

bool is_ball_behind(const Geometry::Point &point, const Ball &ball) {
	return ball.position.x < point.x;
}

bool is_ball_behind(const Robot3 *robot, const Ball &ball) {
	return is_ball_behind(robot->get_position(), ball);
}

bool is_ball_est_ahead(const Geometry::Point &point, const Ball &ball, double offset) {
	return (ball.position + ball.velocity * 0.35).x > point.x + offset;
}
