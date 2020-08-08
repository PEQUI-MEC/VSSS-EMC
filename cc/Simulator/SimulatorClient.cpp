#include "SimulatorClient.hpp"

SimulatorClient::SimulatorClient() {
	client.Connect("127.0.0.1", 20011);
	server.SetMessageHandler([&](evpp::EventLoop* loop, evpp::udp::MessagePtr& msg) {
		packet.ParseFromArray(msg->data(), (int) msg->size());
		if (packet.has_frame()) {
			fira_message::Frame detection = packet.frame();
			for (auto& robot : detection.robots_blue()) {
				std::printf("Blue robot: x: %f, y: %f, o: %f\n", robot.x(), robot.y(), robot.orientation());
			}
			for (auto& robot : detection.robots_yellow()) {
				std::printf("Yellow robot: x: %f, y: %f, o: %f\n", robot.x(), robot.y(), robot.orientation());
			}
			std::printf("Ball: x: %f, y: %f\n", detection.ball().x(), detection.ball().y());
			std::cout << std::endl;

			fira_message::sim_to_ref::Packet cmd_packet;
			fira_message::sim_to_ref::Command* command = cmd_packet.mutable_cmd()->add_robot_commands();
			for (unsigned i = 0; i < 3; i++) {
				command->set_yellowteam(true);
				command->set_id(i);
				command->set_wheel_left(10);
				command->set_wheel_right(10);
				if (buffer.size() < cmd_packet.ByteSize()) {
					buffer.resize((ulong) cmd_packet.ByteSize());
				}
				cmd_packet.SerializeToArray(buffer.data(), cmd_packet.ByteSize());

				client.Send(buffer.data(), (ulong) cmd_packet.ByteSize());
			}
		}
	});
	server.Init(10020);
	server.Start();
}
