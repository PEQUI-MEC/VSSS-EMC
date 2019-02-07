#include "TestOnClick.hpp"
#include <tag.hpp>
#include <Strategy2/Robot2.h>

using namespace onClick;

void TestOnClick::run() {
	if (!m_is_active || has_no_robot())
		return;

	if (!is_target_valid())
		m_selected_robot->stop();
	else {
		switch (m_command) {
			case Robot2::Command::Position:
					m_selected_robot->go_to_and_stop(m_target);
				break;
			case Robot2::Command::UVF:
				if (Geometry::distance(m_selected_robot->get_position(), m_target) > 0.08)
					m_selected_robot->go_to_pose(m_target, m_orientation);
				else
					m_selected_robot->set_target_orientation(m_orientation);
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
}

void TestOnClick::set_orientation(const double orientation) {
	if (!m_is_active || has_no_robot())
		return;

	m_orientation = Geometry::Vector(1, Geometry::degree_to_rad(orientation));
}

TestOnClick::TestOnClick(const std::array<Robot2 *, 3> &robots) : m_is_active(false), m_robots(robots), m_command(Robot2::Command::None),
	m_target(-1, -1), m_orientation(1, 0), m_selected_robot(nullptr){
}

void TestOnClick::select_robot(const double x,const double y) {
	for (auto &robot : m_robots) {
		if (Geometry::distance(robot->get_position(), Geometry::Point(x,y)) < robot->SIZE ) {
			m_selected_robot = robot;
			return;
		}
	}

	m_selected_robot = nullptr;
}

void TestOnClick::select_target(const double x,const double y) {
	if (has_no_robot())
		return;

	m_target = Geometry::Point(x,y);

	if (m_command == Robot2::Command::Vector || m_command == Robot2::Command::Orientation)
		m_orientation = Geometry::Vector(m_target - m_selected_robot->get_position());
}

void TestOnClick::set_active(const bool is_active) {
	if (!is_active) {
		m_target = Geometry::Point(-1, -1);
		if (m_selected_robot != nullptr) {
			m_selected_robot->stop();
			m_selected_robot = nullptr;
		}
	}

	m_is_active = is_active;
}

void TestOnClick::set_command(const Robot2::Command command) {
	m_command = command;
	m_target = Geometry::Point(-1, -1);
}
