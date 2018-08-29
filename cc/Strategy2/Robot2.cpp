#include "Robot2.h"

void Robot2::go_to(Geometry::Point point, double velocity) {
	command = Command::Vector;
	Geometry::Vector direction = point - pose.position;
	target.orientation = direction.theta;
	target.velocity = velocity;
}

void Robot2::go_to_pose(Geometry::Point point, Geometry::Vector direction, double velocity) {
	command = Command::UVF;
	target.position = point;
	target.orientation = direction.theta;
	target.velocity = velocity;
}

void Robot2::go_to_and_stop(Geometry::Point point, double velocity) {
	command = Command::Position;
	target.position = point;
	target.velocity = velocity;
}

void Robot2::go_in_direction(Geometry::Vector vector, double velocity) {
	command = Command::Vector;
	target.orientation = vector.theta;
	target.velocity = velocity;
}

void Robot2::set_target_orientation(Geometry::Vector orientation) {
	command = Command::Orientation;
	target.orientation = orientation.theta;
}

void Robot2::spin(double angular_velocity) {
	command = Command::Angular_Vel;
	target.angular_velocity = angular_velocity;
}

//	Overloads using default target velocity
void Robot2::go_to(Geometry::Point point) {
	go_to(point, default_target_velocity);
}

void Robot2::go_to_pose(Geometry::Point point, Geometry::Vector direction) {
	go_to_pose(point, direction, default_target_velocity);
}

void Robot2::go_to_and_stop(Geometry::Point point) {
	go_to_and_stop(point, default_target_velocity);
}

void Robot2::go_in_direction(Geometry::Vector vector) {
	go_in_direction(vector, default_target_velocity);
}

Robot2::Pose Robot2::get_pose() const {
	return pose;
}