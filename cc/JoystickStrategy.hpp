#ifndef VSSS_JOYSTICKSTRATEGY_HPP
#define VSSS_JOYSTICKSTRATEG_HPP

#include <Geometry/Geometry.h>
#include <Strategy3/Strategy.hpp>
#include "joystick.hh"

class JoystickStrategy : public Strategy {
	public:
	JoystickStrategy(const std::string& path) : joystick(path), path(path){};
	JoystickStrategy(const JoystickStrategy& copied) : joystick(copied.path), path(copied.path) {}
	~JoystickStrategy(){};

	void run_strategy(std::vector<Robot3> &team, std::vector<Adversary> &adversaries, Ball ball,
						bool first_iteration);

	JoystickStrategy * clone() {
		return new JoystickStrategy(*this);
	}

	void set_foul(VSSRef::ref_to_team::VSSRef_Command foul, bool is_defending) {};

	private:
	Joystick joystick;
	std::string path;

	using EventKey = std::pair<uint8_t, uint8_t>;
	std::map<EventKey, short> controller_state;

	Velocity vector_control(Robot3& robot, Geometry::Vector direction);
	void set_cmd_by_input(Robot3& robot, Ball ball, double x_axis, double y_axis, short spin_kick);
	short value(uint8_t type, uint8_t number);
	void print_buttons_state();
};

#endif //VSSS_JOYSTICKSTRATEGY_HPP
