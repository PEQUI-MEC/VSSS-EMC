#include "Control.h"

// WheelVelocity prev{0, 0};
double max_error = 0.14;

double limit_vel(double target, double current) {
	double error = target - current;
	double sign = error > 0 ? 1 : -1;
	if (std::abs(error) > max_error) {
		return current + sign * max_error;
	} else {
		return target;
	}
}

WheelVelocity Control::control_step(const Pose &pose, const Target &target, double time) {
//	Sets current pose and runs robot control
	this->pose = pose;
	this->target = target;
	auto target_vel = run_control();
	auto target_wheel_vel = target_vel.to_wheel_velocity(robot_size);

//	Sets target wheel velocity and runs wheel control
	left.target_vel = target_wheel_vel.left;
	right.target_vel = target_wheel_vel.right;

	// WheelVelocity t_vel{left.target_vel, right.target_vel};

	double left_target = limit_vel(left.pid_step(pose.wheel_velocity.left, time), pose.wheel_velocity.left);
	double right_target = limit_vel(right.pid_step(pose.wheel_velocity.right, time), pose.wheel_velocity.right);

	WheelVelocity vel{left_target, right_target};

	// WheelVelocity vel{limit_vel(left.target_vel, pose.wheel_velocity.left),
					// limit_vel(right.target_vel, pose.wheel_velocity.right)};
	return vel;


	// prev = t_vel;
	

	// double max_vel_left = prev.left + 10 * time;
	// double max_vel_right = prev.right + 10 * time;
	// prev = vel;
	// if (vel.left > max_vel_left) {
	// 	vel.left = max_vel_left;
	// }
	// if (vel.right > max_vel_right) {
	// 	vel.right = max_vel_right;
	// }

	// Usando velocidade diretamente em vez de PID
	// return {left.target_vel, right.target_vel};
	
//	if (target.command == Command::WheelVelocity) {
//		left.pid_step(pose.wheel_velocity.left, time);
//		right.pid_step(pose.wheel_velocity.right, time);
//		return target.pose.wheel_velocity;
//	} else {
	// WheelVelocity vel{left.target_vel, right.target_vel};
	// WheelVelocity vel{left.pid_step(pose.wheel_velocity.left, time),
	// 			right.pid_step(pose.wheel_velocity.right, time)};
	// double max_vel_left = prev.left + 10 * time;
	// double max_vel_right = prev.right + 10 * time;
	// prev = vel;
	// if (vel.left > max_vel_left) {
	// 	vel.left = max_vel_left;
	// }
	// if (vel.right > max_vel_right) {
	// 	vel.right = max_vel_right;
	// }
	// return vel;
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
	else return vector_control(target_theta, target.pose.velocity.linear * std::tanh(20 * error), true, 20);
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

Velocity Control::vector_control(double target_theta, double velocity, bool enable_backwards, double orientation_weight) {
	auto error = Geometry::wrap(target_theta - pose.orientation);
	if (enable_backwards && backwards_select(error)) {
		auto backwards_error = Geometry::wrap(target_theta - (pose.orientation + M_PI));
		return {-velocity * std::cos(backwards_error), orientation_weight * backwards_error};
	} else {
		return {velocity * std::cos(error), orientation_weight * error};
	}
}

bool Control::backwards_select(double theta_error) {
	return std::abs(theta_error) > M_PI / 2;
}
