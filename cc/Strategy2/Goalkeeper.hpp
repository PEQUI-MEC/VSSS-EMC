//
// Created by daniel on 17/08/18.
//

#ifndef VSSS_GOALKEEPER_HPP
#define VSSS_GOALKEEPER_HPP

#include "Robot2.h"
#include "Geometry/Geometry.h"

class Goalkeeper : Robot2 {
	private:
		enum Behavior {
			Protect_Goal,
			Spin_Shot,
		};

		// Beahviors
		void protect_goal(const Geometry::Point ball, const Geometry::Point ball_est);
		void spin_shot(Geometry::Point ball);
};

#endif //VSSS_GOALKEEPER_HPP
