#include "SimulatorClient.hpp"

SimulatorClient::SimulatorClient() {
	client.Connect("127.0.0.1", 20011);
	vision_server.SetMessageHandler([&]([[maybe_unused]] evpp::EventLoop* loop, evpp::udp::MessagePtr& msg) {
		std::lock_guard<std::mutex> guard(data_mutex);
		packet.ParseFromArray(msg->data(), (int) msg->size());
		if (packet.has_frame()) {
			new_data = true;
			auto now = std::chrono::high_resolution_clock::now();
			elapsed_time = now - last_msg_time;
			last_msg_time = now;
		}
	});
	vision_server.Init(10002);
	vision_server.Start();

	referee_server.SetMessageHandler([&]([[maybe_unused]] evpp::EventLoop* loop, evpp::udp::MessagePtr& msg) {
		std::lock_guard<std::mutex> guard(data_mutex);
		ref_command.ParseFromArray(msg->data(), (int) msg->size());
		new_ref_cmd = true;
	});
	referee_server.Init(10003);
	referee_server.Start();
}

void SimulatorClient::update_robots(Game &game) {
	new_data = false;
	fira_message::Frame frame = packet.frame();
	if (game.team->robot_color == RobotColor::Blue) {
		update_team(*game.team.get(), frame.robots_blue());
		update_team(*game.adversary.get(), frame.robots_yellow());
	} else {
		update_team(*game.adversary.get(), frame.robots_blue());
		update_team(*game.team.get(), frame.robots_yellow());
	}

	game.ball.set_position(Geometry::Point::from_simulator(frame.ball().x(), frame.ball().y()));
	game.ball.velocity = Geometry::Vector(Geometry::Point(frame.ball().vx(), frame.ball().vy()));
}

void SimulatorClient::update_team(Team &team, const Repeated<fira_message::Robot>& robots_msg) {
	auto size = std::min(robots_msg.size(), (int) team.robots.size());
	for (int i = 0; i < size; i++) {
		auto& robot = team.robots[i];
		auto& robot_msg = robots_msg.Get(i);
		if (team.inverted_field) {
			robot.pose.position = Geometry::Point::from_simulator(robot_msg.x(), robot_msg.y())
					.inverted_coordinates();
			robot.pose.orientation = Geometry::wrap(robot_msg.orientation() + M_PI);
		} else {
			robot.pose.position = Geometry::Point::from_simulator(robot_msg.x(), robot_msg.y());
			robot.pose.orientation = robot_msg.orientation();
		}
		double vel = std::sqrt(std::pow(robot_msg.vx(), 2) + std::pow(robot_msg.vy(), 2));
		robot.pose.velocity = Velocity{vel, robot_msg.vorientation()};
		robot.pose.wheel_velocity = robot.pose.velocity.to_wheel_velocity(simulated_robot_length);
	}
}

void SimulatorClient::send_commands(Team &team) {
	fira_message::sim_to_ref::Packet cmds_packet;
	for (uint i = 0; i < team.robots.size(); i++) {
		auto& robot = team.robots[i];

		if (team.controlled) {
			fira_message::sim_to_ref::Command* command = cmds_packet.mutable_cmd()->add_robot_commands();
			command->set_yellowteam(team.robot_color == RobotColor::Yellow);
			command->set_id(i);

			WheelVelocity target_wheel_vel = robot.control.control_step(robot.pose, robot.target, elapsed_time.count());
			auto angular_wheel_vel = target_wheel_vel.to_angular(simulated_wheel_radius);
			command->set_wheel_left(angular_wheel_vel.left);
			command->set_wheel_right(angular_wheel_vel.right);
		}
	}
	if (buffer.size() < cmds_packet.ByteSize()) {
		buffer.resize(cmds_packet.ByteSize());
	}
	cmds_packet.SerializeToArray(buffer.data(), (int) cmds_packet.ByteSize());

	client.Send(buffer.data(), (ulong) cmds_packet.ByteSize());
}
