#include "Messenger.h"

using std::string;
using std::vector;
using Pose = Robot2::Pose;
using Command = Robot2::Command;

void Messenger::start_xbee(const string &port, int baud) {
	xbee = new Xbee(port, baud);
	add_robots();
}

void Messenger::stop_xbee() {
	if (!xbee) return;
	delete xbee;
	xbee = nullptr;
}

void Messenger::add_robots() {
	if (!xbee) return;
	xbee->add_robot('A', 0x88a0);
	xbee->add_robot('B', 0xb24a);
	xbee->add_robot('C', 0x215c);
	xbee->add_robot('D', 0x35f6);
	xbee->add_robot('E', 0x97e7);
	xbee->add_robot('F', 0x6b0d);
}

void Messenger::send_msg(char id, string msg) {
	if (!xbee) return;
	xbee->send(id, msg);
}

void Messenger::send_old_format(string cmd) {
	if (!xbee) return;
	char id = cmd[0];
	string msg = cmd.substr(2, cmd.find('#') - 2);
	xbee->send(id, msg);
}

void Messenger::send_ekf_data(const Robot2 &robot) {
	if(!xbee) return;
	Robot2::Pose pose = robot.get_pose();
	xbee->send(robot.ID, 0, float(pose.position.x),
			   float(pose.position.y), float(pose.orientation));
}

void Messenger::send_commands(const std::array<Robot2*, 3> &robots) {
	if (!xbee || ++send_cmd_count <= frameskip) return;
	for (Robot2* robot : robots) {
		send_command(robot->ID, robot->get_target().to_float(), robot->get_command());
	}
	update_msg_time();
	send_cmd_count = 0;
}

void Messenger::send_command(char id, Geometry::FloatPose target, Command command) {
	if(!xbee) return;
	switch (command) {
		case Command::Position:
			xbee->send(id, 1, target.x, target.y, target.velocity);
			break;
		case Command::Vector:
			xbee->send(id, 2, target.orientation, target.velocity);
			break;
		case Command::Pose:
			xbee->send(id, 3, target.x, target.y,
					   target.orientation, target.velocity);
			break;
		case Command::Orientation:
			xbee->send(id, 4, target.orientation);
			break;
		case Command::Angular_Vel:
			xbee->send(id, 5, target.angular_velocity);
			break;
		case Command::None:
			break;
	}
}

double Messenger::get_battery(char id) {
	if (!xbee) return -1;
	string msg = xbee->send_get_answer(id, "B");
	if (msg.empty() || msg[0] != 'B') return -1;
	return ((stod(msg.substr(1)) - 6.4) / 2.0) * 100;
}

void Messenger::set_ack_enabled(bool enable) {
	if (!xbee) return;
	xbee->set_ack_enabled(enable);
}

ack_count Messenger::get_ack_count(char id) {
	if (!xbee) return {-1, -1, -1};
	else return xbee->get_ack_count(id);
}

void Messenger::reset_lost_acks() {
	if (!xbee) return;
	xbee->reset_lost_acks();
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
}

