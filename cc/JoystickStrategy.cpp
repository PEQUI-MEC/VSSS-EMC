#include "JoystickStrategy.hpp"
#include "Field.h"

constexpr short MAX_AXES_VALUE = JoystickEvent::MAX_AXES_VALUE;
constexpr int AXIS_DEADZONE = MAX_AXES_VALUE * 0.1;

short JoystickStrategy::value(uint8_t type, uint8_t number) {
	EventKey key = std::pair(type, number);
	return controller_state[key];
}

void JoystickStrategy::set_cmd_by_input(Robot3& robot, Ball ball, double x_axis, double y_axis, short spin_kick) {
	if (spin_kick > 0) {
		robot.spin_kick_to_target(ball.position, field::their::goal::front::center);
	} else {
		if (std::abs(x_axis) > AXIS_DEADZONE || std::abs(y_axis) > AXIS_DEADZONE) {
			Geometry::Vector analog_direction(Geometry::Point{x_axis/MAX_AXES_VALUE, y_axis/MAX_AXES_VALUE});
			robot.go_in_direction(analog_direction, analog_direction.size * 0.8);
		} else {
			robot.stop();
		}
	}
}

void JoystickStrategy::run_strategy(std::vector<Robot3> &team,
							  std::vector<Adversary> &adversaries,
							  Ball ball,
							  bool first_iteration, bool is_simulation,
							  bool is_inverted) {
	if (joystick.isFound()) {
		JoystickEvent event;
		while (joystick.sample(&event)) {
			EventKey key = std::pair(event.type & ~JS_EVENT_INIT, event.number);
			controller_state[key] = event.value;
		}
	} else {
		printf("Joystick not found");
	}

	int left_robot_index = 0;
	int right_robot_index = 1;
	int third_robot_index = 2;

	if (value(1, 4) > 0) {
		team[left_robot_index].stop();
		left_robot_index = third_robot_index;
	} else if (value(1, 5) > 0) {
		team[right_robot_index].stop();
		right_robot_index = third_robot_index;
	} else {
		team[third_robot_index].stop();
	}

	int axis_direction = is_inverted ? -1 : 1;
	if (is_simulation) {
		set_cmd_by_input(team[left_robot_index], ball, value(2, 0)*axis_direction, -value(2, 1)*axis_direction, value(1, 6));
		set_cmd_by_input(team[right_robot_index], ball, value(2, 3)*axis_direction, -value(2, 4)*axis_direction, value(1, 7));
	} else {
		set_cmd_by_input(team[left_robot_index], ball, -value(2, 1)*axis_direction, -value(2, 0)*axis_direction, value(1, 6));
		set_cmd_by_input(team[right_robot_index], ball, -value(2, 4)*axis_direction, -value(2, 3)*axis_direction, value(1, 7));
	}
}

void JoystickStrategy::print_buttons_state() {
	for(const auto& [key, val] : controller_state) {
		std::cout << "(" << int(key.first) << ", " << int(key.second) << "): " << val << std::endl;
	}
}
