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
};

#endif //VSSS_ATTACKER_H
