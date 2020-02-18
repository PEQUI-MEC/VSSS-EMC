#ifndef VSSS_STRATEGY2_HPP
#define VSSS_STRATEGY2_HPP

#include "../Geometry/Geometry.h"
#include "Attacker.h"
#include "Defender.hpp"
#include "Goalkeeper.hpp"
#include "Field.h"
#include <chrono>

class Strategy2 {
	private:
		using sc = std::chrono::system_clock;
		using duration_ms = std::chrono::duration<double, std::milli>;

		Attacker &attacker;
		Defender &defender;
		Goalkeeper &goalkeeper;
		Geometry::Point &ball;
		Geometry::Point &ball_est;

		sc::time_point last_transition = sc::now();

//		Returns true if a transition happens
		bool transitions();
		bool trainstion_by_collision();
		void swap_robots(Robot2 &robot1, Robot2 &robot2);
		void swap_all_robots();
		bool has_ball(const Robot2 &robot);
		bool is_ball_behind(const Geometry::Point &point);
		bool is_ball_behind(const Robot2 &robot);
		bool is_ball_est_ahead(const Geometry::Point &point, double offset = 0);
		void execute_goalkeeper();
		void execute_attacker();
		void execute_defender();

		bool is_collision(Robot2 &robot1, Robot2 &robot2);
		bool is_collision_axis_y(Robot2 &robot1, Robot2 &robot2);
		bool is_collision_axis_x(Robot2 &robot1, Robot2 &robot2);
		bool is_opposite_direction(Robot2 &robot1, Robot2 &robot2);


	public:
		Strategy2();
		Strategy2(Attacker &attacker, Defender &defender, Goalkeeper &goalkeeper, Geometry::Point &ball,
				  Geometry::Point &ball_est);
		void run();
};

#endif //VSSS_STRATEGY2_HPP
