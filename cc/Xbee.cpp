#include <iostream>
#include "Xbee.h"

using std::string;
using std::pair;
using std::vector;

Xbee::Xbee(const string &port, int baud) {
	using std::cout; using std::endl;
	serial = new serial::Serial(port, static_cast<uint32_t>(baud), serial::Timeout::simpleTimeout(1000));
	std::cout << "Is the serial port open?";
	if(serial->isOpen())
		cout << " Yes." << endl;
	else
		cout << " No." << endl;
}

Xbee::~Xbee() {
	delete serial;
	serial = nullptr;
	robots.clear();
}

void Xbee::add_robot(char id, uint16_t addr) {
	robots[id] = addr;
}

int Xbee::send(char id, const string &message) {
	if (robots.count(id) == 0) return -1;
	serial->write(message + "\n");
}

string Xbee::send_get_answer(char id, const string &message) {
	if (robots.count(id) == 0) "erro";
	serial->write(message + "\n");
	auto answer = serial->readline(12);
	std::cout << answer << std::endl;
	return answer;
}
