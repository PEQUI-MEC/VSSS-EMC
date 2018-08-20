#include "Defender.hpp"

using namespace Geometry;

void Defender::wait_at_target(const Point target, const Point ball) {
	if (distance(get_pose().position, target) > 0.03)
		go_to_and_stop(target);
	else
		set_target_orientation(ball - get_pose().position);
}
