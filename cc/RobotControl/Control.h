#ifndef VSSS_SIMU_ROBOT_H
#define VSSS_SIMU_ROBOT_H

#include <ostream>
#include <utility>
#include <Strategy3/Types.hpp>
#include <Simulator/Wheel.hpp>

class Control {
	public:
	Pose pose;
	Target target;

	Wheel left{};
	Wheel right{};

	double uvf_ref_distance = 0.1;
	double uvf_n = 1.8;
	double robot_size;

	Control(const Pose &pose, const Target &target, double robot_size) :
			pose(pose), target(target), robot_size(robot_size) {}

	WheelVelocity control_step(const Pose &pose, const Target &target, double time);

	Velocity run_control();

	Velocity position_control();
	Velocity uvf_control();
	Velocity orientation_control();
	Velocity vector_control(double target_theta, double velocity, bool enable_backwards);
	bool backwards_select(double theta_error);
};

#endif //VSSS_SIMU_ROBOT_H
