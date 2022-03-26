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

Velocity Control::vector_control_old(double target_theta, double velocity, bool enable_backwards, double orientation_weight) {
    double avoidance_field_theta = 0;
    for (auto& obs : obstacles) {
        avoidance_field_theta += avoidance_field(obs, target_theta);
    }
	auto error = Geometry::wrap(target_theta + avoidance_field_theta - pose.orientation);
	if (enable_backwards && backwards_select(error)) {
		auto backwards_error = Geometry::wrap(target_theta + avoidance_field_theta - (pose.orientation + M_PI));
		return {-velocity * std::cos(backwards_error) * 0.8, orientation_weight * backwards_error};
	} else {
		return {velocity * std::cos(error) * 0.8, orientation_weight * error};
	}
}

Velocity Control::position_control() {
	double target_theta = std::atan2(target.pose.position.y - pose.position.y,
									 target.pose.position.x - pose.position.x);
	double error = std::sqrt(std::pow(target.pose.position.x - pose.position.x, 2.0f)
							 + std::pow(target.pose.position.y - pose.position.y, 2.0f));
	if (error < 0.005) return {0, 0};
	else return vector_control_old(target_theta, target.pose.velocity.linear * std::tanh(30 * error), true, 25);
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
	return vector_control_old(target.pose.orientation, 0, true, 10);
}

double Control::avoidance_field(Geometry::Point point, double target_theta) {
    auto robot_to_point = point - pose.position;
//     double max_shift = 0.06;
//     if (robot_to_point > max_shift) {
//         point = point + robot_to_point.with_size(-max_shift);
//     } else {
//
//     }
    auto theta_diff = Geometry::wrap(target_theta - robot_to_point.theta);
    return std::copysign(0.015, theta_diff) / robot_to_point.size;
}

double err(double theta) {
	return (-2 * std::abs(theta))/M_PI + 1;
	return 1 - std::pow(std::abs(theta)/(M_PI/2), 1/1.3);
}

float saturate(float value, float limit) {
	if(value > limit) value = limit;
	if(value < -limit) value = -limit;
	return value;
}

Velocity Control::nonlinear_controller(float theta_error, float velocity, bool backwards) {
	float m = 1;
	if(backwards) m = -1;

	float right_wheel_velocity = m + std::sin(theta_error) + m*kgz*std::tan(m*theta_error/2);
	right_wheel_velocity = saturate(right_wheel_velocity, 1);

	float left_wheel_velocity = m - std::sin(theta_error) + m*kgz*std::tan(-m*theta_error/2);
	left_wheel_velocity = saturate(left_wheel_velocity, 1);

	return WheelVelocity{left_wheel_velocity * velocity, right_wheel_velocity * velocity}
				.to_velocity(robot_size);
}


Velocity Control::vector_control(double target_theta, double velocity, bool enable_backwards, double orientation_weight) {
    double avoidance_field_theta = 0;
    for (auto& obs : obstacles) {
        avoidance_field_theta += avoidance_field(obs, target_theta);
    }
	auto error = Geometry::wrap(target_theta + avoidance_field_theta - pose.orientation);
	if (enable_backwards && backwards_select(error)) {
		auto backwards_error = Geometry::wrap(target_theta + avoidance_field_theta - (pose.orientation + M_PI));
		return nonlinear_controller(backwards_error, velocity, true);
	} else {
		return nonlinear_controller(error, velocity, false);
	}
}

bool Control::backwards_select(double theta_error) {
	return std::abs(theta_error) > M_PI / 2;
}
