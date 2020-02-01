#include "SimuRobotControl.h"

using namespace Control;

WheelVelocity SimuRobotControl::control_step(Point position, float orientation,
                                             WheelVelocity wheel_vel, float time) {
//	Sets current pose and runs robot control
	this->position = position;
	this->orientation = orientation;
	auto target_vel = run_control();
	auto target_wheel_vel = target_vel.to_wheel_vel();

//	Sets target wheel velocity and runs wheel control
	left.target_vel = target_wheel_vel.left;
	right.target_vel = target_wheel_vel.right;
	return { left.pid_step(wheel_vel.left, time),
	         right.pid_step(wheel_vel.right, time) };
}

TargetVelocity SimuRobotControl::run_control() {
	switch (command) {
		case Command::Position:
			return position_control();
		case Command::Vector:
			return vector_control(target.theta, target.velocity, true);
        case Command::UVF:
			return uvf_control();
		case Command::Orientation:
			return orientation_control();
        case Command::Velocity:
            return {target.velocity, target.angular_vel};
		case Command::None:
			return {0, 0};
	    default:
            return {0, 0};
	}
}

TargetVelocity SimuRobotControl::position_control() {
	float target_theta = std::atan2(target.position.y - position.y,
	                                target.position.x - position.x);
	float error = std::sqrt(std::pow(target.position.x - position.x, 2.0f)
	                        + std::pow(target.position.y - position.y, 2.0f));
	if (error < 0.02) return {0, 0};
	else return vector_control(target_theta, target.velocity * std::sqrt(error), true);
}

TargetVelocity SimuRobotControl::uvf_control() {
	auto targ_pos = target.position;
	auto uvf_ref = target.position + Vector {uvf_ref_distance, target.theta};
	float state_to_targ = std::atan2(targ_pos.y - position.y, targ_pos.x - position.x);
	float state_to_ref = std::atan2(uvf_ref.y - position.y, uvf_ref.x - position.x);
	float fi = wrap(state_to_ref - state_to_targ);
	float uvf_target_theta = wrap(state_to_targ - uvf_n * fi);
	return vector_control(uvf_target_theta, target.velocity, true);
}

TargetVelocity SimuRobotControl::orientation_control() {
	return vector_control(target.theta, 0, true);
}

TargetVelocity SimuRobotControl::vector_control(float target_theta,
                                                float velocity, bool enable_backwards) {
	auto error = wrap(target_theta - orientation);
	if (enable_backwards && backwards_select(error)) {
		auto backwards_error = wrap(target_theta - (orientation + PI));
		return {-velocity * std::cos(backwards_error), 10 * backwards_error};
	} else {
		return {velocity * std::cos(error), 15 * error};
	}
}

void SimuRobotControl::stop() {
    command = Command::None;
}

bool SimuRobotControl::backwards_select(float theta_error) {
	return std::abs(theta_error) > PI / 2;
}

void SimuRobotControl::set_target(Command command, Target target) {
    this->target = target;
    this->command = command;
}
