#ifndef VSSS_SIMULATORCLIENT_HPP
#define VSSS_SIMULATORCLIENT_HPP

#include <common.pb.h>
#include "packet.pb.h"
#include "replacement.pb.h"
#include "command.pb.h"
#include "vssref_command.pb.h"
#include "vssref_placement.pb.h"

#include <evpp/udp/udp_server.h>
#include <evpp/udp/udp_message.h>
#include <evpp/udp/sync_udp_client.h>
#include <mutex>
#include <Strategy3/Game.hpp>
#include <jsonSaveManager.h>

class SimulatorClient {
	public:
	fira_message::sim_to_ref::Environment packet;
	VSSRef::ref_to_team::VSSRef_Command ref_command;

	evpp::udp::Server vision_server;
	evpp::udp::Server referee_server;
	evpp::udp::sync::Client client;
	evpp::udp::sync::Client referee_client;
	std::vector<char> buffer;
	std::vector<char> placement_buffer;

	bool new_data = false;
	bool new_ref_cmd = false;
	std::mutex data_mutex;

	double simulated_wheel_radius = 0.022;
	double simulated_robot_length = 0.08;

	std::chrono::time_point<std::chrono::high_resolution_clock> last_msg_time;
	std::chrono::duration<double> elapsed_time;

	SimulatorClient();
	~SimulatorClient() {
		vision_server.Stop(true);
		referee_server.Stop(true);
	}

	template<typename T>
	using Repeated = google::protobuf::RepeatedPtrField<T>;

	void update_team(Team &team, const Repeated<fira_message::Robot>& robots_msg);
	void update_robots(Game& game);
	void send_commands(Team &team);
	void send_placement(VSSRef::Frame* frame);
};

#endif //VSSS_SIMULATORCLIENT_HPP
