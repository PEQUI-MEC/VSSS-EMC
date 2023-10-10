#include "DefenderStrategy.hpp"
#include "helper.hpp"

using namespace Geometry;
using namespace field;

void DefenderStrategy::run_strategy(const Ball &ball, const Robot3 * attacker) {
	auto df_limit = Point{1.0, field_height/2};
    if (is_ball_behind(our::area::front::center + Vector{0.05, 0}, ball) ||
        (is_ball_behind(our::area::front::center + Vector{0.3, 0}, ball) && attacker && distance(attacker->get_position(), ball.position) < 0.1)
    ) {
		// Sai do caminho e se prepara pra substituir o goleiro
		if (at_location(ball.position, Location::UpperField))
			wait_at_target(defender::back::lower_limit, ball.position);
		else
			wait_at_target(defender::back::upper_limit, ball.position);
    } else if (attacker && !at_location(robot->get_position(), Location::AnyGoal) && at_location(ball.position, Location::TheirField) && ( distance(attacker->get_position() , ball.position) > 0.3 )) {
        Point target = {ball.position.x - 0.3, ball.position.y};
        robot->go_to_and_stop(target);
    } else if (attacker && !at_location(robot->get_position(), Location::AnyGoal) && at_location(ball.position, Location::TheirField)) {
        Point target = attacker->get_position();

        int grid_width = 3;
        int grid_height = 4;

        double rectangle_width = their::area::front::center.x - center::point.x;
        double rectangle_height = field_height - 0;
        double conversion_x = rectangle_width/(grid_width + 1);
        double conversion_y = rectangle_height/(grid_height + 1);
        double conversion_offset_x = conversion_x + center::point.x;
        double conversion_offset_y = conversion_y + 0;

        double grid_position_x = (target.x - conversion_offset_x)/conversion_x;
        double grid_position_y = (target.y - conversion_offset_y)/conversion_y;
        int rounded_grid_position_x = std::round(grid_position_x);
        int rounded_grid_position_y = std::round(grid_position_y);

        int clamped_grid_position_x = std::clamp(rounded_grid_position_x, 0, grid_width - 1);
        int clamped_grid_position_y = std::clamp(rounded_grid_position_y, 0, grid_height - 1);

        int defender_grid_position_x = clamped_grid_position_x - 1;
        int defender_grid_position_y = clamped_grid_position_y + (target.y > center::point.y)? - 2 : + 2;

        int defender_grid_position_x_clamped = std::clamp(defender_grid_position_x, -1, grid_width-1);
        int defender_grid_position_y_clamped = std::clamp(defender_grid_position_y, 0, grid_height-1);

        double defender_field_position_x = defender_grid_position_x_clamped*conversion_x + conversion_offset_x;
        double defender_field_position_y = defender_grid_position_y_clamped*conversion_y + conversion_offset_y;

        Point defender_field_position = {defender_field_position_x , defender_field_position_y};
        robot->go_to_and_stop(defender_field_position);
	} else if (at_location(robot->get_position(), Location::AnyGoal)) {
		exit_goal();
	}else {
		protect_goal(ball.position);
	}
}

void DefenderStrategy::wait_at_target(const Point target, const Point &ball) {
	if (distance(robot->get_position(), target) > robot->TARGET_OFFSET)
		robot->go_to_and_stop(target);
	else
		robot->set_target_orientation(ball - robot->get_position());
}

void DefenderStrategy::protect_goal(const Geometry::Point &ball) {
	if (distance(robot->get_position(), ball) < 0.08) {
		robot->spin_kick_to_target(ball, their::goal::front::center);
	} else if (ball.x >= robot->get_position().x) {
		robot->go_to_and_stop({defender::back::upper_limit.x, ball.y});
	} else if (at_location(ball, Location::UpperField)){
		robot->go_to_and_stop(defender::back::upper_limit);
	} else {
		robot->go_to_and_stop(defender::back::lower_limit);
	}
}

void DefenderStrategy::exit_goal() {
	robot->go_to_and_stop(center::point);
}
