#ifndef VSSS_BALL_HPP
#define VSSS_BALL_HPP

#include <Geometry/Geometry.h>
#include <LS.h>

class Ball {
	public:
	Geometry::Point position;
	Geometry::Point estimative;
	Geometry::Vector velocity;

	LS ls_x, ls_y;

	Ball() {
		ls_x.init(15, 1);
		ls_y.init(15, 1);
	}

	Ball(const Geometry::Point &position, const Geometry::Point &estimative,
		const Geometry::Vector &velocity) :
			position(position), estimative(estimative), velocity(velocity) {}

	void set_position(Geometry::Point new_position) {
		position = new_position;
	}

	void update_ls() {
		ls_x.addValue(position.x);
		ls_y.addValue(position.y);
		estimative.x = ls_x.estimate(10);
		estimative.y = ls_y.estimate(10);
		velocity = (position - estimative) * (30); //30fps;
	}

	void reset_ls() {
		ls_x.reset();
		ls_y.reset();
		estimative = position;
	}

	Ball get_inverted() const {
		return Ball{position.inverted_coordinates(),
			estimative.inverted_coordinates(), velocity.inverted_coordinates()};
	}
};

#endif //VSSS_BALL_HPP
