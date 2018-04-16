#include "Messenger.h"

using std::string;
using std::vector;

void Messenger::start_xbee(const std::string &port, int baud) {
	xbee = new Xbee(port, baud);
	add_robots();
}

void Messenger::stop_xbee() {
	if(!xbee) return;
	delete xbee;
	xbee = nullptr;
}

void Messenger::add_robots(){
	if(!xbee) return;
	xbee->add_robot('A', 0x88a0);
	xbee->add_robot('B', 0xb24a);
	xbee->add_robot('C', 0x215c);
	xbee->add_robot('D', 0x35f6);
	xbee->add_robot('E', 0x97e7);
	xbee->add_robot('F', 0x6b0d);
}

void Messenger::send_msg(char id, string msg){
	if(!xbee) return;
	xbee->send(id,msg);
}

void Messenger::send_old_format(string cmd){
	if(!xbee) return;
	char id = cmd[0];
	string msg = cmd.substr(2,cmd.find('#')-2);
	xbee->send(id,msg);
}

void Messenger::send_cmds(const vector<Robot>& robots) {
	if(!xbee || ++send_cmd_count <= frameskip) return;
	for(Robot robot : robots){
		string msg;
		switch (robot.cmdType){
			case POSITION:
				if(robot.target.x != -1 && robot.target.y != -1)
					msg = position_msg(robot);
				break;
			case SPEED:
				msg = speed_msg(robot);
				break;
			case ORIENTATION:
				msg = orientation_msg(robot);
				break;
			case VECTOR:
				msg = vector_msg(robot);
				break;
			default:
				if(robot.target.x != -1 && robot.target.y != -1)
					msg = position_msg(robot);
		}
		if(!msg.empty()) xbee->send(robot.ID,msg);
	}
	update_msg_time();
	send_cmd_count = 0;
}

string Messenger::position_msg(Robot robot) {
	double diff_x = robot.target.x - robot.position.x;
	double diff_y = robot.target.y - robot.position.y;

	double transTarget_x = cos(robot.orientation)*diff_x + sin(robot.orientation)*diff_y;
	double transTarget_y = -(-sin(robot.orientation)*diff_x + cos(robot.orientation)*diff_y);

	double pos_x = transTarget_x*(150.0/640.0);
	double pos_y = transTarget_y*(130.0/480.0);

	return ("P"+ rounded_str(pos_x)+";"+ rounded_str(pos_y)+";"+ rounded_str(robot.vmax));
}

string Messenger::speed_msg(Robot robot) {
	return (rounded_str(robot.Vr)+";"+ rounded_str(robot.Vl));
}

string Messenger::orientation_msg(Robot robot) {
	double orientation = (robot.orientation + robot.targetOrientation)*180/M_PI;
	return ("O"+ rounded_str(orientation)+";"+ rounded_str(robot.vmax));
}

string Messenger::vector_msg(Robot robot) {
	double orientation = atan2(sin(robot.orientation + robot.transAngle),
							  cos(robot.orientation + robot.transAngle));
	orientation = orientation*(180.0/M_PI);
	return ("V"+ rounded_str(orientation)+";"+ rounded_str(robot.vmax));
}

double Messenger::get_battery(char id) {
	if(!xbee) return -1;
	string msg = xbee->send_get_answer(id,"B");
	if(msg.empty() || msg[0] != 'B') return -1;
	return ((stod(msg.substr(1)) - 6.4)/2.0)*100;
}

string Messenger::rounded_str(double num) {
	double rounded_num = round(num*100)/100;
	std::ostringstream ss;
	ss << rounded_num;
	return ss.str();
}

void Messenger::set_ack_enabled(bool enable) {
	if(!xbee) return;
	xbee->set_ack_enabled(enable);
}

ack_count Messenger::get_ack_count(char id) {
	if(!xbee) return {-1,-1,-1};
	else return xbee->get_ack_count(id);
}

void Messenger::reset_lost_acks() {
	if(!xbee) return;
	xbee->reset_lost_acks();
}

void Messenger::update_msg_time() {
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> time_diff = now - previous_msg_time;
	time_between_msgs = time_diff.count();
	previous_msg_time = now;
}

Messenger::Messenger(){
	setlocale(LC_ALL,"C");
	send_cmd_count = 0;
	frameskip = DEFAULT_FRAMESKIP;
	previous_msg_time = std::chrono::system_clock::now();
}
