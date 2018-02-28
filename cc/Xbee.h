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

	public:
		Xbee(const std::string &port, int baud);
		~Xbee();
		void add_robot(char id, uint16_t addr);
		int send(char id, const std::string &message);
		std::string send_get_answer(char id, const std::string &message);
		std::vector<message> send_get_answer_all(const std::string &message);
		std::stack<message> get_messages();
		void update_ack(char id, int ack);
		ack_count get_ack_count(char id);
		void reset_lost_acks();
		std::string get_string(xbee_pkt *pkt);
};


#endif //VSSS_XBEE_H
