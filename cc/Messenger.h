#ifndef VSSS_MESSENGER_H
#define VSSS_MESSENGER_H

#include <Robot.hpp>
#include "Xbee.h"
#include <string>
#include <vector>
#include <cmath>
#include <chrono>

#define POSITION 0
#define SPEED 1
#define ORIENTATION 2
#define VECTOR 3

#define DEFAULT_FRAMESKIP 5

class Messenger {
	private:
		Xbee* xbee = nullptr;

		double time_between_msgs;
		int frameskip, send_cmd_count;
		std::chrono::system_clock::time_point previous_msg_time;

		void add_robots();
		std::string position_msg(Robot robot);
		std::string speed_msg(Robot robot);
		std::string orientation_msg(Robot robot);
		std::string vector_msg(Robot robot);
		std::string rounded_str(double num);

	public:
		Messenger();
		void send_cmds(const std::vector<Robot>& robots);
		void send_msg(char id, std::string msg);
		void send_old_format(std::string cmd);
		double get_battery(char id);
		void set_ack_enabled(bool enable);
		ack_count get_ack_count(char id);
		void reset_lost_acks();
		void start_xbee(const std::string& port, int baud = 115200);
		void stop_xbee();
		void update_msg_time();

		void set_frameskipper(int frames) {frameskip = frames;}
		int get_frameskipper() {return frameskip;}
		double get_time() {return time_between_msgs;}
};


#endif //VSSS_MESSENGER_H
