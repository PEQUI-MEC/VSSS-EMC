#ifndef VSSS_STRATEGY2_HPP
#define VSSS_STRATEGY2_HPP

#include "Geometry/Geometry.h"
#include "Strategy2/Attacker.h"
#include "Strategy2/Defender.hpp"
#include "Strategy2/Goalkeeper.hpp"
#include "Strategy2/Field.h"
#include "LS.h"


class Strategy2 {
	private:
		Attacker& attacker;
		Defender& defender;
		Goalkeeper& goalkeeper;
		Geometry::Point& ball;
		Geometry::Point ball_est;
		LS ls_x, ls_y;

		void calculate_ball_est();
		void check_for_transitions();
		void swap_robots(Robot2& robot1, Robot2& robot2);
		void swap_all_robots();
		bool is_ball_behind(const Geometry::Point& point);
		bool is_ball_behind(const Robot2& robot);
		bool is_ball_est_ahead(const Geometry::Point& point, double offset = 0);
		void execute_goalkeeper();
		void execute_attacker();
		void execute_defender();

	public:
		Strategy2(Attacker &attacker, Defender &defender, Goalkeeper& goalkeeper, Geometry::Point &ball);
		void run();
};

#endif //VSSS_STRATEGY2_HPP
