#ifndef VSSS_MESSENGER_H
#define VSSS_MESSENGER_H

#include "Xbee.h"
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <Strategy3/Robot3.hpp>

#define DEFAULT_FRAMESKIP 0

class Messenger {
	private:
		struct pose {
			double x;
			double y;
			double theta;
		};

		std::vector<Xbee> xbees;
		unsigned xbee_index = 0;

		double time_between_msgs;
		int frameskip, send_cmd_count;
		std::chrono::system_clock::time_point previous_msg_time;

		void add_robots(Xbee &xbee);
		std::string rounded_str(double num);

	public:
//		std::ofstream ekf_data_file;
		Messenger();
		void send_commands(const std::vector<Robot3> &robots);
		void send_command(char id, Target target);
		void send_ekf_data(const Robot3 &robot);

		void send_msg(char id, std::string msg);
		void send_old_format(std::string cmd);
		double get_battery(char id);
		void set_ack_enabled(bool enable);
		ack_count get_ack_count(char id);
		void reset_lost_acks();
		unsigned int start_xbee(const std::string &port, int baud = 115200);
		void stop_xbee(int index);
		void update_msg_time();
		void set_frameskipper(int frames) { frameskip = frames; }
		int get_frameskipper() { return frameskip; }
		double get_time() { return time_between_msgs; }
		Xbee& next_xbee();
		bool has_xbee() const { return !xbees.empty(); };
};

#endif //VSSS_MESSENGER_H
