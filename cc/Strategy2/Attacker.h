#ifndef VSSS_ATTACKER_H
#define VSSS_ATTACKER_H

#include "Robot2.h"
#include "Geometry/Geometry.h"

class Attacker : Robot2 {
	private:
		enum Behavior {
			UVF_To_Goal
		};

		void execute_behavior(Behavior behavior);
		void uvf_to_goal(Geometry::Point ball);
		void Attacker::spin_shot(bool clockwise, double speed);
		void Attacker::hang_in_the_corner(Geometry::Point ball);

};

#endif //VSSS_ATTACKER_H
