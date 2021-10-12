#include "Control.h"

WheelVelocity Control::control_step(const Pose &pose, const Target &target, double time) {
//	Sets current pose and runs robot control
	this->pose = pose;
	this->target = target;
	auto target_vel = run_control();
	auto target_wheel_vel = target_vel.to_wheel_velocity(robot_size);

//	Sets target wheel velocity and runs wheel control
	left.target_vel = target_wheel_vel.left;
	right.target_vel = target_wheel_vel.right;

	// Usando velocidade diretamente em vez de PID
	return {left.target_vel, right.target_vel};
	
//	if (target.command == Command::WheelVelocity) {
//		left.pid_step(pose.wheel_velocity.left, time);
//		right.pid_step(pose.wheel_velocity.right, time);
//		return target.pose.wheel_velocity;
//	} else {
		return {left.pid_step(pose.wheel_velocity.left, time),
				right.pid_step(pose.wheel_velocity.right, time)};
//	}
}

Velocity Control::run_control() {
	switch (target.command) {
		case Command::Position:
			return position_control();
		case Command::Vector:
			return vector_control(target.pose.orientation, target.pose.velocity.linear, true);
		case Command::Velocity:
			return target.pose.velocity; // used to make robot spin
		case Command::WheelVelocity:
			return target.pose.wheel_velocity.to_velocity(robot_size);
		case Command::Orientation:
			return orientation_control();
		case Command::UVF:
			return uvf_control();
		case Command::None:
			return {0, 0};
	}
}

Velocity Control::position_control() {
	double target_theta = std::atan2(target.pose.position.y - pose.position.y,
									 target.pose.position.x - pose.position.x);
	double error = std::sqrt(std::pow(target.pose.position.x - pose.position.x, 2.0f)
							 + std::pow(target.pose.position.y - pose.position.y, 2.0f));
	if (error < 0.02) return {0, 0};
	else return vector_control(target_theta, target.pose.velocity.linear * std::tanh(20 * error), true, 25);
}

Velocity Control::uvf_control() {
	auto targ_pos = target.pose.position;
	double state_to_targ = std::atan2(targ_pos.y - pose.position.y, targ_pos.x - pose.position.x);
	double state_to_ref = std::atan2(target.reference.y - pose.position.y, target.reference.x - pose.position.x);
	double fi = Geometry::wrap(state_to_ref - state_to_targ);
	double uvf_target_theta = Geometry::wrap(state_to_targ - uvf_n * fi);
	return vector_control(uvf_target_theta, target.pose.velocity.linear, true);
}

Velocity Control::orientation_control() {
	return vector_control(target.pose.orientation, 0, true);
}

double Control::avoidance_field(Obstacle obs, double target_theta) {
	auto robot_to_obs = obs.position - pose.position;

	if (robot_to_obs.size < 0.1 && std::abs(obs.shift_closer_distance) > 0.015) return 0;

	auto obstacle_pos = obs.position + robot_to_obs.with_size(-obs.shift_closer_distance);

    auto robot_to_point = obstacle_pos - pose.position;
    auto theta_diff = Geometry::wrap(target_theta - robot_to_point.theta);
	int sign = theta_diff > 0 ? 1 : -1;
    return (sign * obs.weight) / robot_to_point.size;
}

double err(double error) {
	return std::cos(error);
	return 1 - std::pow(error/(M_PI/2), 1.0/2.0);
}

Velocity Control::vector_control(double target_theta, double velocity, bool enable_backwards, double orientation_weight) {
    double avoidance_field_theta = 0;
    for (auto& obs : obstacles) {
        avoidance_field_theta += avoidance_field(obs, target_theta);
    }
	auto error = Geometry::wrap(target_theta + avoidance_field_theta - pose.orientation);
	if (enable_backwards && backwards_select(error)) {
		auto backwards_error = Geometry::wrap(target_theta + avoidance_field_theta - (pose.orientation + M_PI));
		return {-velocity * std::cos(backwards_error), orientation_weight * backwards_error};
	} else {
		return {velocity * std::cos(error), orientation_weight * error};
	}
}

bool Control::backwards_select(double theta_error) {
	bool should_be_backwards = std::abs(theta_error) > M_PI / 2;
	// return should_be_backwards;

	duration_ms since_last_change = sc::now() - last_change_orientation;
	if (is_backwards != should_be_backwards && since_last_change.count() > 200) {
		is_backwards = should_be_backwards;
		last_change_orientation = sc::now();
		return should_be_backwards;
	} else {
		return is_backwards;
	}
}
