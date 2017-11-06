#ifndef SERIALW_HPP_
#define SERIALW_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include "../pack-capture-gui/capture-gui/Robot.hpp"

#define POSITION 0
#define SPEED 1
#define ORIENTATION 2
#define VECTOR 3
#define PI 3.14159265453

class SerialW
{
public:
	bool Serial_Enabled;
	int	USB;
	std::string port;

	SerialW();

	int start(std::string serial);

	void sendToRobot(Robot r);

	void sendCmdToRobots(std::vector<Robot> robot_list);

	void sendSerial(std::string cmd);

	int readSerial(char* buf, int size);
};
#endif /* CONTROLGUI_HPP_ */
