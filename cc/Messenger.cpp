#include "Messenger.h"

using namespace std;

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
	xbee->add_robot('A', 0x1212);
	xbee->add_robot('B', 0x25f8);
	xbee->add_robot('C', 0x3816);
	xbee->add_robot('D', 0x7878);
	xbee->add_robot('E', 0x9090);
	xbee->add_robot('F', 0x6c50);
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

vector<message> Messenger::sendCMDs(vector<Robot> robots) {
	vector<message> acks;
	if(!xbee) return acks;
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
		if(!msg.empty()){
			int ack = xbee->send(robot.ID,msg);
			acks.push_back({robot.ID, to_string(ack)});
		}
	}
	return acks;
}

string Messenger::position_msg(Robot robot) {
	double diff_x = robot.target.x - robot.position.x;
	double diff_y = robot.target.y - robot.position.y;

	double transTarget_x = cos(robot.orientation)*diff_x + sin(robot.orientation)*diff_y;
	double transTarget_y = -(-sin(robot.orientation)*diff_x + cos(robot.orientation)*diff_y);

	double pos_x = round(transTarget_x*(150.0/640.0)*100.0)/100;
	double pos_y = round(transTarget_y*(130.0/480.0)*100.0)/100;
	double vel = round(robot.vmax*100.0)/100;

	return ("P"+to_string(pos_x)+";"+to_string(pos_y)+";"+to_string(vel));
}

string Messenger::speed_msg(Robot robot) {
	double speed_r = round(robot.Vr*100)/100;
	double speed_l = round(robot.Vl*100)/100;
	return (to_string(speed_r)+";"+to_string(speed_l));
}

string Messenger::orientation_msg(Robot robot) {
	double orientation = (robot.orientation + robot.targetOrientation)*180/M_PI;
	orientation = round(orientation*100)/100;
	double vmax = round(robot.vmax*100.0)/100;
	return ("O"+to_string(orientation)+";"+to_string(vmax));
}

string Messenger::vector_msg(Robot robot) {
	double orientation= atan2(sin(robot.orientation + robot.transAngle),cos(robot.orientation + robot.transAngle));
	orientation = round(orientation*(180.0/M_PI)*100)/100;
	double vmax = round(robot.vmax*100)/100;
	return ("V"+to_string(orientation)+";"+to_string(vmax));
}

string Messenger::to_string(double num) {
	ostringstream ss;
	ss << num;
	return ss.str();
}

double Messenger::get_battery(char id) {
	string msg = xbee->send_get_answer(id,"B");
	if(msg.empty() || msg[0] != 'B') return -1;
	msg.erase(0,1);
	return ((stod(msg) - 6.4)/2.0)*100; // % of battery
}