#ifndef VSSS_SIMULATORCLIENT_HPP
#define VSSS_SIMULATORCLIENT_HPP

#include <common.pb.h>
#include "packet.pb.h"
#include "replacement.pb.h"
#include "command.pb.h"

#include <evpp/udp/udp_server.h>
#include <evpp/udp/udp_message.h>
#include <evpp/udp/sync_udp_client.h>
#include <mutex>
#include <Strategy3/Game.hpp>

class SimulatorClient {
	public:
	fira_message::sim_to_ref::Environment packet;
	evpp::udp::Server server;
	evpp::udp::sync::Client client;
	std::vector<char> buffer;

	bool new_data = false;
	std::mutex data_mutex;

	double simulated_wheel_radius = 0.02;
	double simulated_robot_length = 0.08;

	SimulatorClient();
	~SimulatorClient() {
		server.Stop(true);
	}

	template<typename T>
	using Repeated = google::protobuf::RepeatedPtrField<T>;

	void update_team(Team &team, const Repeated<fira_message::Robot>& robots_msg);
	void update_robots(Game& game);
	void send_commands(Team &team);
};

#endif //VSSS_SIMULATORCLIENT_HPP
