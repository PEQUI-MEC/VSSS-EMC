#ifndef VSSS_TESTONCLICK_HPP
#define VSSS_TESTONCLICK_HPP

#include "Robot2.h"
#include <memory>

namespace onClick {
	class TestOnClick {
		private:
			bool m_is_active;

			const std::array<Robot2 *, 3>& m_robots;
			const Geometry::Point& m_ball;
			Robot2* m_selected_robot;

			const double m_ball_radius = 0.02135; // metros

			Robot2::Command m_command;

			Geometry::Point m_target;
			Geometry::Vector m_orientation;

			bool m_is_target_ball;
			bool m_is_ready;

		public:

			explicit TestOnClick(const std::array<Robot2 *, 3> &robots, const Geometry::Point &ball);

			void run();

			void set_ready(bool is_rdy = true);

			void set_active(bool is_active = true);
			void set_command(Robot2::Command command);
			void set_orientation(double orientation);
			void set_orientation(double x, double y);

			Robot2::Command get_command() const { return m_command; };
			Robot2* get_selected_robot() const { return m_selected_robot; };
			double get_orientation_value() const { return m_orientation.theta; };
			Geometry::Point get_target() const { return m_is_target_ball? m_ball : m_target; };

			bool is_active() const { return m_is_active; };
			bool has_robot() const { return m_selected_robot != nullptr; };
			bool is_target_valid() const { return m_target.x >= 0 && m_target.y >= 0; }

			void select_robot(double x, double y);
			void select_target(double x, double y);


	};
}



#endif //VSSS_TESTONCLICK_HPP
