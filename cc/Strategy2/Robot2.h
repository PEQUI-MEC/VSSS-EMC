#ifndef VSSS_ROBOT_H
#define VSSS_ROBOT_H

#include "Geometry/Geometry.h"

class Robot2 {
	private:
		struct Pose {
			Geometry::Point position;
			Geometry::Vector orientation;
			double velocity;
			double angular_velocity;
		};

		enum class Command {
			Position, Vector, UVF, Orientation, Angular_Vel, None
		};

		Pose pose;
		Pose target;
		Command command = Command::None;

	public:
		void go_to(Geometry::Point point);
		void go_to(Geometry::Point point, Geometry::Vector direction);
		void go_to_and_stop(Geometry::Point point);
		void go_in_direction(Geometry::Vector vector);
		void set_orientation(double orientation);
		void spin(double angular_velocity);
};

#endif //VSSS_ROBOT_H
