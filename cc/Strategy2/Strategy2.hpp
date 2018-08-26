#ifndef VSSS_STRATEGY2_HPP
#define VSSS_STRATEGY2_HPP

#include "Geometry/Geometry.h"
#include "Strategy2/Field.h"
#include "Strategy2/Attacker.h"
#include "Strategy2/Defender.hpp"
#include "Strategy2/Goalkeeper.hpp"


class Strategy2 {
	private:
		Attacker& attacker;
		Defender& defender;
		Goalkeeper& goalkeeper;
		Geometry::Point& ball;

		void checkForTransitions();
		void swap_atk_def();
		bool is_ball_behind(const Geometry::Point& point);
		bool is_ball_behind(const Robot2& robot);

	public:
		void run();
};

#endif //VSSS_STRATEGY2_HPP
