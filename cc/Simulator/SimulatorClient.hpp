#ifndef VSSS_SIMULATORCLIENT_HPP
#define VSSS_SIMULATORCLIENT_HPP


#include <common.pb.h>
#include "packet.pb.h"
#include "replacement.pb.h"
#include "command.pb.h"

#include <evpp/udp/udp_server.h>
#include <evpp/udp/udp_message.h>
#include <evpp/udp/sync_udp_client.h>

class SimulatorClient {
	public:
	fira_message::sim_to_ref::Environment packet;
	evpp::udp::Server server;
	evpp::udp::sync::Client client;
	std::vector<char> buffer;

	SimulatorClient();
	~SimulatorClient() {
		server.Stop(true);
	}
};

#endif //VSSS_SIMULATORCLIENT_HPP
