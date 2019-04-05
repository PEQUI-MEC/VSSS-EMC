#ifndef VSSS_BALL_HPP
#define VSSS_BALL_HPP

#include "LS.h"
#include "Geometry.h"

class Ball {
	private:
		LS ls_x, ls_y;

	public:
		Geometry::Point position;
		Geometry::Point estimate;

		Ball() {
			ls_x.init(15, 1);
			ls_y.init(15, 1);
		}

		void update_estimate() {
			ls_x.addValue(position.x);
			ls_y.addValue(position.y);

			estimate.x = ls_x.estimate(10);
			estimate.y = ls_y.estimate(10);
		}
};

#endif //VSSS_BALL_HPP
