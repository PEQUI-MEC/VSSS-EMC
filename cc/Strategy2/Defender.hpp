#ifndef VSSS_DEFENDER_HPP
#define VSSS_DEFENDER_HPP

#include "Robot2.h"
#include "Geometry/Geometry.h"

class Defender : public Robot2 {
	private:
		enum Behavior {
			Wait_At_Target,
		};

		// Behaviors
		void wait_at_target(Geometry::Point target, Geometry::Point ball);
		Role get_role() override { return Role::Defender; };
		std::string get_role_name() override { return "Defender"; };
};

#endif //VSSS_DEFENDER_HPP
