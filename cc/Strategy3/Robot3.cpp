#include "Robot3.hpp"

void Robot3::go_to(Geometry::Point point, std::optional<double> velocity) {
	target.command = Command::Vector;
	Geometry::Vector direction = point - pose.position;
	target.pose.position = point;
	target.pose.orientation = direction.theta;
	target.pose.velocity.linear = velocity.value_or(default_target_velocity);
}

void Robot3::go_to_pose(Geometry::Point point, Geometry::Vector direction, std::optional<double> velocity) {
	target.command = Command::UVF;
	target.pose.position = point;
	target.reference= point + direction.with_size(0.12);
	target.pose.orientation = direction.theta;
	target.pose.velocity.linear = velocity.value_or(default_target_velocity);
}

void Robot3::go_to_and_stop(Geometry::Point point, std::optional<double> velocity) {
	if (distance(point, pose.position) >= TARGET_OFFSET) {
		target.command = Command::Position;
		target.pose.position = point;
		target.pose.velocity.linear = velocity.value_or(default_target_velocity);
	} else {
		target.command = Command::None;
		spin(0);
	}
}

void Robot3::go_in_direction(Geometry::Vector vector, std::optional<double> velocity) {
	target.command = Command::Vector;
	target.pose.orientation = vector.theta;
	target.pose.velocity.linear = velocity.value_or(default_target_velocity);
}

void Robot3::set_target_orientation(Geometry::Vector orientation) {
	target.command = Command::Orientation;
	target.pose.orientation = orientation.theta;
	target.pose.velocity.linear = default_target_velocity; // mudar p/ angular em vez de linear em Robot3 e messenger
}

void Robot3::spin(double angular_velocity) {
//	std::cout << (angular_velocity > 0 ? "positivo" : "negativo") << std::endl;
	target.command = Command::Velocity;
	target.pose.velocity.linear = 0;
	target.pose.velocity.angular = angular_velocity;
}

void Robot3::set_target_wheel_velocity(double left, double right) {
	target.command = Command::WheelVelocity;
	target.pose.wheel_velocity = {left, right};
}

Geometry::Vector Robot3::get_direction() {
	switch (target.command) {
		case Command::Position:
			return Geometry::Vector{target.pose.position - pose.position}
					.with_size(target.pose.velocity.linear);
		case Command::Orientation:
		case Command::UVF:
		case Command::Vector:
			return Geometry::Vector(target.pose.velocity.linear, target.pose.orientation)
					.with_size(target.pose.velocity.linear);
		default:
			return Geometry::Vector{0, 0};
	}
}
