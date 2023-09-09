#include "Messenger.h"

using std::string;
using std::vector;

void Messenger::start_xbee(const string &port, int baud) {
	esp32.emplace(port, baud);
}

void Messenger::stop_xbee(const string &port) {
	if (esp32.has_value() && esp32->port == port) {
		esp32.reset();
	}
}

void Messenger::send_msg(char id, string msg) {
	if (esp32.has_value()) {
		esp32->send_msg(id, msg);
	}
}

void Messenger::send_old_format(string cmd) {
	char id = cmd[0];
	string msg = cmd.substr(2, cmd.find('#') - 2);
	send_msg(id, msg);
}

void Messenger::send_commands(const std::vector<Robot3> &robots) {
	if (esp32.has_value() || ++send_cmd_count <= frameskip) return;
	std::string all_msgs;
	for (const Robot3& robot : robots) {
		if (robot.role != Role::None) {
			std::string msg = command_to_msg(robot.target);
			if (!msg.empty()) {
				if (!all_msgs.empty()) all_msgs += ',';
				all_msgs += std::string(1, robot.ID) + '@' + msg;
			}
		}
	}
	if (!all_msgs.empty()) {
		esp32->send_string_msg(all_msgs);
	}
	update_msg_time();
	send_cmd_count = 0;
}

constexpr float robot_size = 0.0675f;
std::string Messenger::command_to_msg(Target target) {
	switch (target.command) {
		case Command::Position:
			return "P" + rounded_str(target.pose.position.x * 100)
							+ ";" + rounded_str(target.pose.position.y * 100)
									+ ";" + rounded_str(target.pose.velocity.linear);
		case Command::Velocity: // tratar casos em que velocity.linear != 0
			return "W" + rounded_str(target.pose.velocity.angular*robot_size/2)
					+ ";" + rounded_str(-target.pose.velocity.angular*robot_size/2);
		case Command::Vector:
			return ("V" + rounded_str(target.pose.orientation * 180.0f/M_PI)
							+ ";" + rounded_str(target.pose.velocity.linear));
		case Command::UVF:
			return "U" + rounded_str(target.pose.position.x * 100) + ";" + rounded_str(target.pose.position.y * 100)
					+ ";" + rounded_str(target.reference.x * 100) + ";" + rounded_str(target.reference.y * 100)
					+ ";" + rounded_str(1.2) + ";" + rounded_str(target.pose.velocity.linear);
		case Command::Orientation:
			return "O" + rounded_str(target.pose.orientation * 180/M_PI)
							+ ";" + rounded_str(target.pose.velocity.linear);
		case Command::WheelVelocity:
			return "W" + rounded_str(target.pose.wheel_velocity.left)
					+ ";" + rounded_str(target.pose.wheel_velocity.right);
		default:
			return string();
	}
}

void Messenger::send_command(char id, Target target) {
	if(!esp32.has_value()) return;
	const string msg = command_to_msg(target);
	if (!msg.empty()) {
		esp32->send_msg(id, msg);
//		if(id == 'A') std::cout << msg << std::endl;
	}
}

void Messenger::send_ekf_data(const Robot3 &robot) {
	if(!esp32.has_value() || robot.role == Role::None) return;
	string msg = "E" + rounded_str(robot.pose.position.x * 100) + ";"
				 + rounded_str(robot.pose.position.y * 100)
				 + ";" + rounded_str(robot.pose.orientation * 180/M_PI);
	esp32->send_msg(robot.ID, msg);
//	if(robot.get_ID() == 'A') std::cout << msg << std::endl;
}

double Messenger::get_battery(char id) {
	if (!esp32.has_value()) return -1;
	string msg = esp32->send_get_answer(id, "B");
	if (msg.empty() || msg[0] != 'B') return -1;
	else return stod(msg.substr(1));
}

string Messenger::rounded_str(double num) {
	double rounded_num = round(num * 100) / 100;
	std::ostringstream ss;
	ss << rounded_num;
	return ss.str();
}

void Messenger::update_msg_time() {
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> time_diff = now - previous_msg_time;
	time_between_msgs = time_diff.count();
	previous_msg_time = now;
}

Messenger::Messenger()
//		: ekf_data_file("ekf_data.csv")
{
	setlocale(LC_ALL, "C");
	send_cmd_count = 0;
	frameskip = DEFAULT_FRAMESKIP;
	previous_msg_time = std::chrono::system_clock::now();
	time_between_msgs = 0;
}