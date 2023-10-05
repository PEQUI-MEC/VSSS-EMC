#ifndef VSSS_MESSENGER_H
#define VSSS_MESSENGER_H

#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <Strategy3/Robot3.hpp>
#include <Esp32Serial.hpp>

#define DEFAULT_FRAMESKIP 0

class Messenger {
	private:
		struct pose {
			double x;
			double y;
			double theta;
		};

		std::optional<Esp32Serial> esp32;

		double time_between_msgs;
		int frameskip, send_cmd_count;
		std::chrono::system_clock::time_point previous_msg_time;

	public:
//		std::ofstream ekf_data_file;
		Messenger();
		Messenger(const Messenger&) = delete; // Não permite fazer cópias do objeto Messenger
		Messenger& operator=(const Messenger&) = delete;
		std::string command_to_msg(Target target);
		void send_commands(const std::vector<Robot3> &robots);
		void send_command(char id, Target target);
		void send_ekf_data(const Robot3 &robot);


		std::string rounded_str(double num);

		void send_msg(char id, std::string msg);
		void send_old_format(std::string cmd);
		void start_xbee(const std::string &port, int baud = 115200);
		void stop_xbee(const std::string &port);
		void update_msg_time();
		void set_frameskipper(int frames) { frameskip = frames; }
		int get_frameskipper() { return frameskip; }
		double get_time() { return time_between_msgs; }
		bool using_port(const std::string& port) { return esp32.has_value() && esp32->port == port; }
		bool has_serial() { return esp32.has_value(); }
		std::unordered_map<char, float> read_batteries();
};

#endif //VSSS_MESSENGER_H
