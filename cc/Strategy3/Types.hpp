#ifndef VSSS_TYPES_HPP
#define VSSS_TYPES_HPP

#include <Geometry/Geometry.h>


struct WheelAngularVelocity {
	double left = 0;
	double right = 0;
};

// Forward declaration of Velocity
struct Velocity;

struct WheelVelocity {
	double left = 0;
	double right = 0;
	WheelAngularVelocity to_angular(double wheel_radius) const;

	Velocity to_velocity(double robot_length) const;
};

struct Velocity {
	double linear = 0;
	double angular = 0;
	WheelVelocity to_wheel_velocity(double robot_length) const;
};

struct Pose {
	Geometry::Point position;
	double orientation = 0;
	Velocity velocity;
	WheelVelocity wheel_velocity;
};

enum class Command {
	Position, Vector, Velocity, WheelVelocity, Orientation, UVF, None
};

std::string command_to_str(Command command);

struct Target {
	Command command = Command::None;
	Pose pose;
	Geometry::Point reference; // for UVF
	double n; // for UVF
};

constexpr unsigned int ROLE_COUNT = 4;
enum class Role {
	None = 0, Attacker = 1, Goalkeeper = 2, Defender = 3
};

std::string role_to_str(Role role);

#endif //VSSS_TYPES_HPP
