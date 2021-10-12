#ifndef VSSS_SIMU_ROBOT_H
#define VSSS_SIMU_ROBOT_H

#include <ostream>
#include <utility>
#include <Strategy3/Types.hpp>
#include <Simulator/Wheel.hpp>

struct Obstacle {
	Geometry::Point position;
	double weight;
	double shift_closer_distance;
};

class Control {
	public:
	Pose pose;
	Target target;

	Wheel left{};
	Wheel right{};

	double uvf_ref_distance = 0.1;
	double uvf_n = 1.8;
	double robot_size;

	using sc = std::chrono::system_clock;
	using duration_ms = std::chrono::duration<double, std::milli>;
	sc::time_point last_change_orientation = sc::now();
	bool is_backwards = false;

    std::vector<Obstacle> obstacles;

	Control(const Pose &pose, const Target &target, double robot_size) :
			pose(pose), target(target), robot_size(robot_size) {}

	WheelVelocity control_step(const Pose &pose, const Target &target, double time);

	Velocity run_control();

	Velocity position_control();
	Velocity uvf_control();
	Velocity orientation_control();
	Velocity vector_control(double target_theta, double velocity, bool enable_backwards, double orientation_weight = 12);
	bool backwards_select(double theta_error);
    double avoidance_field(Obstacle obs, double target_theta);
};

#endif //VSSS_SIMU_ROBOT_H
