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
    } else if (attacker && !at_location(robot->get_position(), Location::AnyGoal) && at_location(ball.position, Location::TheirField)) {
			Point target = ( distance(attacker->get_position() , ball.position) <= 0.3 )? attacker.get_position() : ball.position;

			int grid_width = 3;
			int grid_height = 4;

			double rectangle_width = their::area::front::center.x - center::point.x;
			double rectangle_height = field_height - 0;
			double conversion_x = rectangle_width/(grid_width + 1);
			double conversion_y = rectangle_height/(grid_height + 1);
			double conversion_offset_x = conversion_x + center::point.x;
			double conversion_offset_y = conversion_y + 0;

			int grid_position_x = std::round((target.x - conversion_offset_x)/conversion_x);
			int grid_position_y = std::round((target.y - conversion_offset_y)/conversion_y);

			int defender_grid_position_x = grid_position_x - 1;
			int defender_grid_position_y = (target.y > center::point.x)? grid_position_y - 2 : grid_position_y + 2;

			defender_grid_position_x_clamped = std::clamp(defender_grid_position_x, -1, grid_width-1);
			defender_grid_position_y_clamped = std::clamp(defender_grid_position_y, 0, grid_height-1);

			double defender_field_position_x = defender_grid_position_x/conversion_x + conversion_offset_x;
			double defender_field_position_y = defender_grid_position_y/conversion_y + conversion_offset_y;

			point defender_field_position = {defender_field_position_x , defender_field_position_y};
			robot->go_to_and_stop(defender_field_position);

//		// Bola na na área adversária
//		//std::vector<float> DefenderPoints = {Point{}, Point{}, Point{}};
//		//robot->go_to_and_stop({defender::back::upper_limit.x, ball.y});
//		//atacante + bola, defensor atras espelhado
//		//atk sem bola, defensor ponto + prox atras da bola 
//
//		double max_x = their::area::front::center.x;
//		double min_x = center::point.x;
//		double sizex = (max_x - min_x);
//
//		double max_y = field_height;
//		double min_y = 0;
//		double sizey = (max_y - min_y);
//
//		int grid_x = 3;
//		int grid_y = 4;
//
//		double find_rounded_x = round(attacker->get_position().x * ((grid_x - 1)/ sizex));
//		double closest_x = find_rounded_x * (sizex / (grid_x - 1));
//
//		double find_rounded_y = round(attacker->get_position().y * ((grid_y - 1)/ sizey));
//		double closest_y = find_rounded_y * (sizey / (grid_y - 1));
//
//		Point closest = {closest_x , closest_y};
//
//		double distance_attacker_ball = distance(attacker->get_position() , ball.position);
//		
//
//		if (distance_attacker_ball <= 0.3){
//
//			if(at_location(attacker->get_position(), field::Location::LowerField)){
//				closest_x = (find_rounded_x - 1) * (sizex / (grid_x - 1));
//				closest_y = (find_rounded_y + 2 ) * (sizey / (grid_y - 1));
//				closest = {closest_x , closest_y};
//				robot->go_to_and_stop(closest);
//			} else{
//				closest_x = (find_rounded_x - 1) * (sizex / (grid_x - 1));
//				closest_y = (find_rounded_y - 2 ) * (sizey / (grid_y - 1));
//				closest = {closest_x , closest_y};
//				robot->go_to_and_stop(closest);
//			}
//
		} else {
			find_rounded_x = round(ball.position.x * ((grid_x - 1)/ sizex));
			find_rounded_y = round(ball.position.y * ((grid_y - 1)/ sizey));

			closest_x = (find_rounded_x - 1) * (sizex / (grid_x - 1));
			closest_y = (find_rounded_y - 2 ) * (sizey / (grid_y - 1));
			closest = {closest_x , closest_y};
			robot->go_to_and_stop(closest);
		}
		//double pos_y = std::clamp(ball.position_in_seconds(1).y, defender::front::lower::wait_point.y, defender::front::upper::wait_point.y);
		//wait_at_target({attacker->get_position().x - robot->SIZE*5, pos_y}, ball.position);
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
