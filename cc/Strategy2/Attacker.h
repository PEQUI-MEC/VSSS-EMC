#ifndef VSSS_ATTACKER_H
#define VSSS_ATTACKER_H

#include "boost/variant.hpp"
#include "Robot2.h"
#include "Geometry/Geometry.h"

class Attacker : public Robot2 {
	public:
		enum Behavior {
			UVF_To_Goal
		};

		void execute_behavior(Behavior behavior);
		void uvf_to_goal(Geometry::Point ball);
		Role get_role() override { return Role::Attacker; };
		std::string get_role_name() override { return "Attacker"; };
};

#endif //VSSS_ATTACKER_H
