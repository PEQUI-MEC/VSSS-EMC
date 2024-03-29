#include "Messenger.h"

using std::string;
using std::vector;

void Messenger::start_xbee(const string &port, int baud) {
	xbees.emplace_back(port, baud);
	add_robots(xbees.back());
	used_ports.insert(port);
	next_xbee_it = xbees.begin();
	xbees.back().set_ack_enabled(measuring_acks);
}

void Messenger::stop_xbee(const string &port) {
	if (!using_port(port)) return;
	used_ports.erase(port);
	auto xbee_it = std::find_if(xbees.begin(), xbees.end(), [&](Xbee& xbee) {
		return xbee.port == port;
	});
	xbees.erase(xbee_it);
	next_xbee_it = xbees.begin();
}

void Messenger::add_robots(Xbee &xbee) {
	xbee.add_robot('A', 0x88a0);
	xbee.add_robot('B', 0xb24a);
	xbee.add_robot('C', 0x215c);
	xbee.add_robot('D', 0x35f6);
	xbee.add_robot('E', 0x97e7);
	xbee.add_robot('F', 0x6b0d);
}

void Messenger::send_msg(char id, string msg) {
	if (!has_xbee()) return;
	next_xbee().send(id, msg);
}

void Messenger::send_old_format(string cmd) {
	if (!has_xbee() || cmd.empty()) return;
	char id = cmd[0];
	string msg = cmd.substr(2, cmd.find('#') - 2);
	next_xbee().send(id, msg);
}

void Messenger::send_commands(const std::vector<Robot3> &robots) {
	if (!has_xbee() || ++send_cmd_count <= frameskip) return;
	for (const Robot3& robot : robots) {
		if (robot.role != Role::None)
			send_command(robot.ID, robot.target);
	}
	update_msg_time();
	send_cmd_count = 0;
}

constexpr float robot_size = 0.0675f;
void Messenger::send_command(char id, Target target) {
	if(!has_xbee()) return;
	const string msg = [&] {
		switch (target.command) {
			case Command::Position:
				return "P" + rounded_str(target.pose.position.x * 100)
							 + ";" + rounded_str(target.pose.position.y * 100)
									 + ";" + rounded_str(target.pose.velocity.linear);
			case Command::Velocity: // tratar casos em que velocity.linear != 0
				return rounded_str(target.pose.velocity.angular*robot_size/2)
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
				return rounded_str(target.pose.wheel_velocity.right)
					   + ";" + rounded_str(target.pose.wheel_velocity.left);
			default:
				return string();
		}
	}();
	if (!msg.empty()) {
		next_xbee().send(id, msg);
//		if(id == 'A') std::cout << msg << std::endl;
	}
}

void Messenger::send_ekf_data(const Robot3 &robot) {
	if(!has_xbee() || robot.role == Role::None) return;
	string msg = "E" + rounded_str(robot.pose.position.x * 100) + ";"
				 + rounded_str(robot.pose.position.y * 100)
				 + ";" + rounded_str(robot.pose.orientation * 180/M_PI);
	next_xbee().send(robot.ID, msg);
//	if(robot.get_ID() == 'A') std::cout << msg << std::endl;
}

double Messenger::get_battery(char id) {
	if (!has_xbee()) return -1;
	string msg = next_xbee().send_get_answer(id, "B");
	if (msg.empty() || msg[0] != 'B') return -1;
	else return stod(msg.substr(1));
}

string Messenger::rounded_str(double num) {
	double rounded_num = round(num * 100) / 100;
	std::ostringstream ss;
	ss << rounded_num;
	return ss.str();
}

void Messenger::set_ack_enabled(bool enable) {
	for (auto& xbee : xbees) {
		xbee.set_ack_enabled(enable);
	}
	measuring_acks = enable;
}

ack_count Messenger::get_ack_count(char id) {
	if (!has_xbee()) return {-1, -1, -1};
	else {
		ack_count total{0, 0, 0};
		for (auto& xbee : xbees) {
			ack_count count = xbee.get_ack_count(id);
			total.lost += count.lost;
			total.total += count.total;
		}
		if (total.total == 0) return {0, 0, 0};
		total.lost_rate = double(total.lost) / double(total.total) * 100;
		return total;
	}
}

void Messenger::reset_lost_acks() {
	for (auto& xbee : xbees) {
		xbee.reset_lost_acks();
	}
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

Xbee &Messenger::next_xbee() {
	auto& current_xbee = *next_xbee_it++;
	if (next_xbee_it == xbees.end()) next_xbee_it = xbees.begin();
	return current_xbee;
}
