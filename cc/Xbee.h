#ifndef VSSS_XBEE_H
#define VSSS_XBEE_H

#include <xbee.h>
#include <vector>
#include <string>
#include <stack>
#include <cstring>
#include <unordered_map>

struct message {
	char id;
	std::string data;
};

struct ack_count {
	int lost;
	int total;
	double lost_rate;
};

struct robot_xbee {
	char id;
	uint16_t addr;
	xbee_con *con;
	ack_count acks;
};

class Xbee {
	private:
		struct xbee *xbee;
		std::unordered_map<char, robot_xbee> robots;
		std::string get_string(xbee_pkt *pkt);
		void update_ack(char id, int ack);

	public:
		Xbee(const std::string &port, int baud);
		~Xbee();
		void add_robot(char id, uint16_t addr);
		int send(char id, const std::string &message);
		std::string send_get_answer(char id, const std::string &message);
		std::vector<message> send_get_answer(const std::string &message);
		std::vector<message> get_messages();
		ack_count get_ack_count(char id);
		void reset_lost_acks();
		void set_ack_enabled(char id, bool enable);
		void set_ack_enabled(bool enable);
		bool is_ack_enabled(char id);
};

#endif //VSSS_XBEE_H
