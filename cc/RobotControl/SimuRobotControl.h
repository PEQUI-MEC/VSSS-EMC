#ifndef VSSS_SIMU_ROBOT_H
#define VSSS_SIMU_ROBOT_H

#include <ostream>
#include <utility>
#include "include.h"

class SimuRobotControl {
public:
	Control::Point position{};
	float orientation{};

	Control::Target target{};
	Control::Command command = Control::Command::None;

	Control::Wheel left{};
	Control::Wheel right{};

	float uvf_ref_distance = 0.1;
	float uvf_n = 1.8;

	void set_target(Control::Command command, Control::Target target);
	Control::WheelVelocity control_step(Control::Point position, float orientation,
	                           Control::WheelVelocity wheel_vel, float time);
	Control::TargetVelocity run_control();

	Control::TargetVelocity position_control();
	Control::TargetVelocity uvf_control();
	Control::TargetVelocity orientation_control();
	Control::TargetVelocity vector_control(float target_theta,
	                              float velocity, bool enable_backwards);
	bool backwards_select(float theta_error);
	void stop();
};

#endif //VSSS_SIMU_ROBOT_H
