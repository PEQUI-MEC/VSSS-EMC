//
// Created by daniel on 17/08/18.
//

#ifndef VSSS_GOALKEEPER_HPP
#define VSSS_GOALKEEPER_HPP

#include "Robot2.h"
#include "Geometry/Geometry.h"

class Goalkeeper : public Robot2 {
	private:
		enum Behavior {
			Protect_Goal,
			Spin_Shot,
		};

		// Beahviors
		void protect_goal(Geometry::Point ball, Geometry::Point ball_est);
		void spin_shot(Geometry::Point ball);
		Role get_role() override { return Role::Goalkeeper; };
		std::string get_role_name() override { return "Goalkeeper"; };
};

#endif //VSSS_GOALKEEPER_HPP
