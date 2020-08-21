#include "Types.hpp"

std::string role_to_str(Role role) {
	switch (role) {
		case Role::Attacker:
			return "Attacker";
		case Role::Goalkeeper:
			return "Goalkeeper";
		case Role::Defender:
			return "Defender";
		default:
			return "None";
	}
}

std::string command_to_str(Command command) {
	switch (command) {
		case Command::Position:
			return "Position";
		case Command::Vector:
			return "Vector";
		case Command::Velocity:
			return "Velocity";
		case Command::WheelVelocity:
			return "Wheel Velocity";
		case Command::Orientation:
			return  "Orientation";
		case Command::UVF:
			return "UVF";
		case Command::None:
			return "None";
	}
}

Velocity WheelVelocity::to_velocity(double robot_length) const {
	return Velocity{(left + right)/2, (right - left) / robot_length};
}

WheelAngularVelocity WheelVelocity::to_angular(double wheel_radius) const {
	return WheelAngularVelocity{left / wheel_radius, right / wheel_radius};
}

WheelVelocity Velocity::to_wheel_velocity(double robot_length) const {
	return WheelVelocity{linear - (angular * robot_length)/2, linear + (angular * robot_length)/2};
}

Pose Pose::inverted(bool invert) const {
	if (!invert) return *this;
	else return Pose {
		position.inverted_coordinates(),
		Geometry::wrap(orientation + M_PI),
		velocity,
		wheel_velocity
	};
}

Target Target::inverted(bool invert) const {
	if (!invert) return *this;
	else return Target {
		command,
		pose.inverted(),
		reference.inverted_coordinates(),
		n
	};
}
