#ifndef VSSS_DEFENDER_HPP
#define VSSS_DEFENDER_HPP

#include "Robot2.h"
#include "Geometry/Geometry.h"

class Defender : Robot2 {
	private:
		enum Behavior {
			Wait_At_Target,
		};

		// Behaviors
		void wait_at_target(Geometry::Point target, Geometry::Point ball);
};

#endif //VSSS_DEFENDER_HPP
