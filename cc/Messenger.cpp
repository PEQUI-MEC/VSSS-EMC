#include "Messenger.h"

using std::string;
using std::vector;

void Messenger::start_xbee(const std::string &port, int baud) {
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

void Messenger::send_cmds(const vector<Robot> &robots) {
	if (!xbee || ++send_cmd_count <= frameskip) return;
	for (Robot robot : robots) {
		const string msg = [&] {
			switch (robot.cmdType) {
				case POSITION:
					if (robot.target.x != -1 && robot.target.y != -1)
						return position_msg(robot);
					else return string();
				case SPEED:
					return speed_msg(robot);
				case ORIENTATION:
					return orientation_msg(robot);
				case VECTOR:
					return vector_msg(robot);
				case UVF:
					return uvf_msg(robot);
				default:
					return string();
			}
		}();
		if (!msg.empty()) xbee->send(robot.ID, msg);
	}
	update_msg_time();
	send_cmd_count = 0;
}

Messenger::pose Messenger::to_robot_reference(const cv::Point &point, double theta) {
	double x = point.x * (170.0 / 640.0);
	double y = 130.0 - point.y * (130.0 / 480.0);
	double orientation = -theta * 180/M_PI;
	return {x, y, orientation};
}

void Messenger::send_ekf_data(vector<Robot>& robots) {
	for (Robot& robot :robots) {
		pose robot_pose = to_robot_reference(robot.position, robot.orientation);
		string msg = "E" + rounded_str(robot_pose.x) + ";"
					 	 + rounded_str(robot_pose.y) + ";" + rounded_str(robot_pose.theta);

//		if(robot.ID == 'E') ekf_data_file << rounded_str(robot_pose.x) << ',' << rounded_str(robot_pose.y)
//										  << ',' << rounded_str(robot_pose.theta) << '\n';
		xbee->send(robot.ID, msg);
//		if(robot.ID == 'E')
//			std::cout << xbee->send_get_answer(robot.ID, msg) << std::endl;
	}
}

string Messenger::position_msg(Robot robot) {
	auto target_pos = to_robot_reference(robot.target, 0);
	return "P" + rounded_str(target_pos.x) + ";" + rounded_str(target_pos.y)
		       + ";" + rounded_str(robot.vmax);
}

string Messenger::speed_msg(Robot robot) {
	return rounded_str(robot.Vr) + ";" + rounded_str(robot.Vl);
}

string Messenger::orientation_msg(Robot robot) {
	return "O" + rounded_str(robot.targetOrientation * 180/M_PI) + ";" + rounded_str(robot.vmax);
}

string Messenger::vector_msg(Robot robot) {
	double theta = robot.transAngle * (180.0/M_PI);
	return ("V" + rounded_str(theta) + ";" + rounded_str(robot.vmax));
}

string Messenger::uvf_msg(Robot robot) {
	auto target = to_robot_reference(robot.target, 0);
	auto ufv_ref = to_robot_reference(robot.uvf_ref, 0);
	return "U" + rounded_str(target.x) + ";" + rounded_str(target.y)
		   + ";" + rounded_str(ufv_ref.x) + ";" + rounded_str(ufv_ref.y)
		   + ";" + rounded_str(robot.uvf_n) + ";" + rounded_str(robot.vmax);
}

double Messenger::get_battery(char id) {
	if (!xbee) return -1;
	string msg = xbee->send_get_answer(id, "B");
	if (msg.empty() || msg[0] != 'B') return -1;
	return ((stod(msg.substr(1)) - 6.4) / 2.0) * 100;
}

string Messenger::rounded_str(double num) {
	double rounded_num = round(num * 100) / 100;
	std::ostringstream ss;
	ss << rounded_num;
	return ss.str();
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
