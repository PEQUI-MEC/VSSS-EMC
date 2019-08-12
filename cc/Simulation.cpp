#include "Simulation.hpp"

Simulation::Simulation(const std::string &name1, const std::string &name2, const std::string &name3,
		bool is_right_team, capture::V4LInterface &interface_ref) :
		ros_robots({RosRobot{nh, name1}, RosRobot{nh, name2}, RosRobot{nh, name3}}),
		ball_position_sub(nh, "ball/position", 1),
		sync(*ros_robots[0].pose_sub, *ros_robots[1].pose_sub,
			 *ros_robots[2].pose_sub, ball_position_sub, 10),
			 is_right_team(is_right_team), interface(interface_ref) {

  if (is_right_team)
    team = std::make_shared<Team>(interface.teams[static_cast<int>(Teams::RightSimu)]);
  else
    team = std::make_shared<Team>(interface.teams[static_cast<int>(Teams::LeftSimu)]);
  
	sync.registerCallback(&Simulation::ros_callback, this);
	start_ros_thread();
}

double quat_to_euler(double a, double b, double c, double d) {
	return std::atan2(2 * (a * d + b * c),
					  1 - 2 * (std::pow(c, 2) + std::pow(d, 2)));
}

void Simulation::ros_callback(const PoseStampedPtr &robot1_msg, const PoseStampedPtr &robot2_msg,
							  const PoseStampedPtr &robot3_msg, const PointStampedPtr &ball_msg) {

	const std::array<const PoseStampedPtr *, 3> msgs_ptr{&robot1_msg, &robot2_msg, &robot3_msg};

	for (auto& robot : team->robots) {
		const auto& msg = msgs_ptr[robot->tag]->get();
		const auto& position = msg->pose.position;
		const auto& quat = msg->pose.orientation;
		auto theta = quat_to_euler(quat.w, quat.x, quat.y, quat.z);
		if (!is_right_team) {
			robot->set_pose_simu({position.x, position.y}, theta);
		} else {
			robot->set_pose_simu({1.7 - position.x, 1.3 - position.y}, Geometry::wrap(theta + PI));
		}
	}

	auto ball_position = ball_msg.get()->point;
	if (!is_right_team) {
		team->ball.position = {ball_position.x, ball_position.y};
	} else {
		team->ball.position = {1.7 - ball_position.x, 1.3 - ball_position.y};
	}

	if (interface.get_start_game_flag()) {
		team->ball.update_estimate();

		team->strategy.run();

		for (auto& robot : team->robots) {
			auto target = robot->get_target();

			vsss_msgs::Control control_msg;
			control_msg.command = (uint8_t) robot->get_command();
			if (!is_right_team) {
				control_msg.pose.x = target.position.x;
				control_msg.pose.y = target.position.y;
				control_msg.pose.theta = target.orientation;
			} else {
				control_msg.pose.x = 1.7 - target.position.x;
				control_msg.pose.y = 1.3 - target.position.y;
				control_msg.pose.theta = Geometry::wrap(target.orientation + PI);
			}
			control_msg.velocity.linear.x = target.velocity;
			control_msg.velocity.angular.z = target.angular_velocity;

			ros_robots[robot->tag].control_pub.publish(control_msg);
		}
	}
}

void Simulation::start_ros_thread() {
	assert(ros_thread == std::nullopt);
//	stop camera before run simulation
	if (0 != interface.bt_start.get_label().compare("start")) {
		interface.__event_bt_start_clicked();
	}
	run_ros_thread = true;
	ros_thread = std::optional<std::thread>{[&] {
		while (run_ros_thread) ros::spinOnce();
	}};
}

void Simulation::stop_ros_thread() {
	run_ros_thread = false;
	auto &thread = ros_thread.value();
	while (!thread.joinable());
	thread.join();
	ros_thread = std::nullopt;
}