#include "Defender.hpp"

using namespace Geometry;
using namespace field;

void Defender::wait_at_target(Geometry::Point target, Geometry::Point &ball) {
	if (distance(get_position(), target) > 0.03)
		go_to_and_stop(target);
	else
		set_target_orientation(ball - get_position());
}

void Defender::protect_goal(const Geometry::Point &ball, const Geometry::Point &ball_est) {
	const Line ball_est_line(ball, ball_est);
	const Point goal_intersection = intersection(our::goal::front::line, ball_est_line);

	if (at_location(goal_intersection, Location::OurBox)) {
		go_to_and_stop(intersection(defender::back::line, ball_est_line));
	} else if (at_location(goal_intersection, Location::UpperField)){
		go_to_and_stop(defender::back::upper_limit);
	} else {
		go_to_and_stop(defender::back::lower_limit);
	}
}
