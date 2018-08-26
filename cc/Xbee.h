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

		constexpr unsigned byte_size() {return 0;}
		template <typename Head, typename... Tail>
		constexpr unsigned byte_size(const Head & head,
									 const Tail &... tail) {
			return sizeof head + byte_size(tail...);
		}

		void set_buffer(uint8_t* buffer){}
		template <typename Head, typename... Tail>
		void set_buffer(uint8_t* buffer, const Head & head,
						const Tail &... tail) {
			std::memcpy(buffer, &head, sizeof(head));
			set_buffer(buffer + sizeof(head), tail...);
		}

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

		template <typename... Data>
		void send(char ID, uint8_t type, const Data &... data) {
			uint8_t buffer[byte_size(type, data...)];
			set_buffer(buffer, type, data...);
			uint8_t ack;
			xbee_connTx(robots[ID].con, &ack, buffer, sizeof buffer);
		}
};

#endif //VSSS_XBEE_H
