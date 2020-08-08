#ifndef VSSS_BALL_HPP
#define VSSS_BALL_HPP

#include <Geometry/Geometry.h>
#include <LS.h>

class Ball {
	public:
	Geometry::Point position;
	Geometry::Point estimative;

	LS ls_x, ls_y;

	Ball() {
		ls_x.init(15, 1);
		ls_y.init(15, 1);
	}

	void update_ls() {
		ls_x.addValue(position.x);
		ls_y.addValue(position.y);
		estimative.x = ls_x.estimate(10);
		estimative.y = ls_y.estimate(10);
	}
};

#endif //VSSS_BALL_HPP
