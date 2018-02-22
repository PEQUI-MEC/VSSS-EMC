#ifndef VSSS_XBEE_H
#define VSSS_XBEE_H

#include <xbee.h>
#include <vector>
#include <string>
#include <stack>
#include <cstring>
#include <unordered_map>

struct robot_xbee {
	char id;
	uint16_t addr;
	struct xbee_con *con;
};

struct message {
	char id;
	std::string data;
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
		std::string get_string(xbee_pkt *pkt);
};


#endif //VSSS_XBEE_H
