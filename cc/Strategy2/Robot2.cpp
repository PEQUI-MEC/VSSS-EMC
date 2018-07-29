#include "Robot2.h"

void Robot2::go_to(Geometry::Point point) {
	command = Command::Vector;
	Geometry::Vector direction = point - pose.position;
	target.orientation.theta = direction.theta;
}

void Robot2::go_to(Geometry::Point point, Geometry::Vector direction) {
	command = Command::UVF;
	target.position = point;
	target.orientation = direction;
}

void Robot2::go_to_and_stop(Geometry::Point point) {
	command = Command::Position;
	target.position = point;
}

void Robot2::go_in_direction(Geometry::Vector vector) {
	command = Command::Vector;
	target.orientation = vector;
}

void Robot2::set_orientation(double orientation) {
	command = Command::Orientation;
	target.orientation.theta = orientation;
}

void Robot2::spin(double angular_velocity) {
	command = Command::Angular_Vel;
	target.angular_velocity = angular_velocity;
}

