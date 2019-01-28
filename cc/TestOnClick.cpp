#include "TestOnClick.hpp"
#include <tag.hpp>

void TestOnClick::run() {
	if (!m_is_active || m_selected_robot == nullptr)
		return;

	switch (m_command) {
		case Robot2::Command::Position:
			if (is_target_valid())
				m_selected_robot->go_to_and_stop(m_target);
			break;
		case Robot2::Command::UVF:
			if (is_target_valid())
				m_selected_robot->go_to_pose(m_target, m_orientation);
			break;
		case Robot2::Command::Orientation:
			m_selected_robot->set_target_orientation(m_orientation);
			break;
		case Robot2::Command::Vector:
			m_selected_robot->go_in_direction(m_orientation);
			break;
		default:
			m_selected_robot->stop();
	}
}

void TestOnClick::set_orientation(double orientation) {
	if (!m_is_active || m_selected_robot == nullptr)
		return;

	m_orientation = Geometry::Vector(1, Geometry::degree_to_rad(orientation));
}

TestOnClick::TestOnClick(const std::array<Robot2 *, 3> &robots) : m_is_active(false), m_robots(robots), m_command(Robot2::Command::None),
	m_target(-1, -1), m_orientation(1, 0), m_selected_robot(nullptr){
}

void TestOnClick::select_robot(double x, double y) {
	for (auto &robot : m_robots) {
		if (Geometry::distance(robot->get_position(), Geometry::Point(x, y)) < ROBOT_RADIUS ) {
			m_selected_robot = robot;
			return;
		}
	}

	m_selected_robot = nullptr;
}

void TestOnClick::select_target(double x, double y) {
	m_target = Geometry::Point(x, y);
}

void TestOnClick::set_active(bool active) {
	if (!active) {
		m_target = Geometry::Point(-1, -1);
		m_selected_robot = nullptr;
	}

	m_is_active = active;
};
