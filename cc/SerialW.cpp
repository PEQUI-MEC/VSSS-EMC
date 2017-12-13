#include "SerialW.hpp"

SerialW::SerialW() {
	Serial_Enabled =false;
}

int SerialW::start(std::string serial){
	struct termios tty;
	struct termios tty_old;
	memset (&tty, 0, sizeof tty);

	USB = open(serial.c_str(), O_RDWR| O_NOCTTY);
	if(USB != -1) {
		Serial_Enabled=true;
	}
	else{

		Serial_Enabled=false;
		return USB;
	}



	/* Error Handling */
	if ( tcgetattr ( USB, &tty ) != 0 ) {
		std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
	}

	/* Save old tty parameters */
	tty_old = tty;

	/* Set Baud Rate */
	cfsetospeed (&tty, (speed_t)B115200);


	tty.c_oflag = 0;    // no remapping, no delays
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	tty.c_cflag &= ~(CSIZE | PARENB);
	tty.c_cflag |= CS8;
	/* Setting other Port Stuff */
	//tty.c_cflag     &=  ~PARENB;            // Make 8n1
	//tty.c_cflag     &=  ~CSTOPB;
	//tty.c_cflag     &=  ~CSIZE;
	//tty.c_cflag     |=  CS8;

	tty.c_cflag     &=  ~CRTSCTS;           // no flow control
	tty.c_cc[VMIN]   =  1;                  // read doesn't block
	tty.c_cc[VTIME]  = 5;                  // 0.5 seconds read timeout
	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

	/* Make raw */
	cfmakeraw(&tty);

	/* Flush Port, then applies attributes */
	tcflush( USB, TCIFLUSH );

	if ( tcsetattr ( USB, TCSANOW, &tty ) != 0) {
		std::cout << "Error " << errno << " from tcsetattr" << std::endl;
	}

	return USB;
}

// void SerialW::sendAPIToRobot(std::string msg){
// 	if (!Serial_Enabled) return;
// 	std::stringstream cmd;
// 	double temp0= floor(r.Vr*100)/100;
// 	double temp1= floor(r.Vl*100)/100;
// 	cmd<<r.ID<<'@'<< temp0<<";"<<temp1<<"#";
// 	sendAPISerial(r, cmd.str());
// 	// std::cout<<cmd.str()<<std::endl;
// }

void SerialW::sendCmdToRobots(std::vector<Robot> robot_list){
	if (!Serial_Enabled) return;
	std::stringstream cmd;
	double temp0, temp1, temp2, temp3, temp4;
	for (int i = 0; i < 3; i++){
		switch (robot_list[i].cmdType){
			case POSITION:
			if (robot_list.at(i).target.x != -1 && robot_list.at(i).target.x != -1)
			{
				temp3 = double(robot_list[i].target.x - robot_list[i].position.x);
				temp4 = double(robot_list[i].target.y - robot_list[i].position.y);

				robot_list[i].transTarget.x = round(cos(robot_list[i].orientation)*temp3 + sin(robot_list[i].orientation)*temp4);
				robot_list[i].transTarget.y = round(-(-sin(robot_list[i].orientation)*temp3 + cos(robot_list[i].orientation)*temp4));

				temp0= round(double(robot_list[i].transTarget.x)*(150.0/640.0)*100)/100;
				temp1= round(double(robot_list[i].transTarget.y)*(130.0/480.0)*100)/100;
				temp2= round(double(robot_list[i].vmax)*100)/100;
				cmd<<robot_list[i].ID<<'@'<<"P"<<temp0<<";"<<temp1<<";"<<temp2<<"#";
			}
			break;
			case SPEED:
			temp0= round(robot_list[i].Vr*100)/100;
			temp1= round(robot_list[i].Vl*100)/100;
			cmd<<robot_list[i].ID<<'@'<<temp0<<";"<<temp1<<"#";
			break;
			case ORIENTATION:
			temp2 = double(robot_list[i].orientation) - ( - double(robot_list[i].targetOrientation));
			temp0= temp2*180/PI;
			temp0 = round(temp0*100)/100;
			temp1= round(double(robot_list[i].vmax)*100)/100;
			cmd << robot_list[i].ID<<'@'<<"O"<<temp0<<";"<<temp1<<"#";
			//std::cout << cmd.str() << std::endl;
			break;
			case VECTOR:
			temp0= double(atan2(sin(robot_list[i].orientation-(-robot_list[i].transAngle)),cos(robot_list[i].orientation-(-robot_list[i].transAngle)))*180/PI);
			temp0 = round(temp0*100)/100;
			temp1= round(double(robot_list[i].vmax)*100)/100;
			cmd << robot_list[i].ID<<'@'<<"V"<<temp0<<";"<<temp1<<"#";
			// cout << robot_list[i].ID<<'@'<<"V"<<temp0<<";"<<temp1<<"#"<< endl;
			break;
			default:
			if (robot_list.at(i).target.x != -1 && robot_list.at(i).target.x != -1)
			{
				temp3 = double(robot_list[i].target.x - robot_list[i].position.x);
				temp4 = double(robot_list[i].target.y - robot_list[i].position.y);

				robot_list[i].transTarget.x = round(cos(robot_list[i].orientation)*temp3 + sin(robot_list[i].orientation)*temp4);
				robot_list[i].transTarget.y = round(-(-sin(robot_list[i].orientation)*temp3 + cos(robot_list[i].orientation)*temp4));

				temp0= round(double(robot_list[i].transTarget.x)*(150.0/640.0)*100)/100;
				temp1= round(double(robot_list[i].transTarget.y)*(130.0/480.0)*100)/100;
				temp2= round(double(robot_list[i].vmax)*100)/100;
				cmd<<robot_list[i].ID<<'@'<<"P"<<temp0<<";"<<temp1<<";"<<temp2<<"#";
			}
		}
	}
	if (!cmd.str().empty()) sendAPISerial(cmd.str().c_str());
	// std::cout << cmd.str().c_str() << std::endl;
}

void SerialW::sendSerial(std::string cmd){
	if (!Serial_Enabled) return;
	int result = write(USB, cmd.c_str(), cmd.size());
}

std::vector<uint8_t> stringToInt(std::string cmd) {
	std::vector<uint8_t> vec;
	// data
	for (int i = 0; i < cmd.size(); i++) {
		vec.push_back(uint8_t(cmd[i]));
	}
	return vec;
}

void SerialW::sendAPISerialText(std::string cmd) {
	if (!Serial_Enabled) return;
	uint8_t start_delimiter = 0x7E;
	uint8_t frame_type = 0x01;
	uint8_t frame_id = 0x01;
	uint8_t option = 0x00;
	uint16_t address = 0x4444;
	uint16_t lenght = uint16_t(5+cmd.size());

	std::vector<uint8_t> hex_message = stringToInt(cmd);

	uint8_t checksum = generateChecksum(frame_type, frame_id, address, option, cmd);

	uint8_t *msg = (uint8_t *) malloc(9+cmd.size());
	msg[0] = start_delimiter;
	msg[1] = uint8_t(lenght >> 8);
	msg[2] = uint8_t(lenght);
	msg[3] = frame_type;
	msg[4] = frame_id;
	msg[5] = uint8_t(address >> 8);
	msg[6] = uint8_t(address);
	msg[7] = option;
	for (size_t i = 0; i < cmd.size(); i++) {
			msg[i+8] = (uint8_t) cmd[i];
	}
	// msg[cmd.size()+8] = checksum;
	// for (size_t i = 0; i < 9+cmd.size(); i++) {
	// 	printf("%02x ",msg[i]);
	// 	/* code */
	// }


	int result = write(USB, msg, 9+cmd.size());
	// std::cout << "size = " << 9+cmd.size() << std::endl;
	free(msg);

}

void SerialW::sendAPISerial(std::string cmd) {
	if (!Serial_Enabled) return;
	//  if (cmd[0] != robot.ID) return;
	uint8_t start_delimiter = 0x7E;
	uint8_t frame_type = 0x01;
	uint8_t frame_id = 0x01;
	uint8_t option = 0x00;
	uint16_t address = 0x4444;
	uint16_t lenght = uint16_t(5+cmd.size());

	std::vector<uint8_t> hex_message = stringToInt(cmd);

	uint8_t checksum = generateChecksum(frame_type, frame_id, address, option, cmd);

	uint8_t *msg = (uint8_t *) malloc(9+cmd.size());
	msg[0] = start_delimiter;
	msg[1] = uint8_t(lenght >> 8);
	msg[2] = uint8_t(lenght);
	msg[3] = frame_type;
	msg[4] = frame_id;
	msg[5] = uint8_t(address >> 8);
	msg[6] = uint8_t(address);
	msg[7] = option;
	for (size_t i = 0; i < cmd.size(); i++) {
			msg[i+8] = (uint8_t) cmd[i];
	}
	msg[cmd.size()+8] = checksum;
	for (size_t i = 0; i < 9+cmd.size(); i++) {
		// printf("%02x ",msg[i]);
		/* code */
	}


	int result = write(USB, msg, 9+cmd.size());
	// std::cout << "size = " << 9+cmd.size() << std::endl;
	free(msg);

}

uint8_t SerialW::generateChecksum(uint8_t type, uint8_t id, uint16_t address, uint8_t option, std::string cmd) {
	uint8_t check = 0xFF - type - id - (uint8_t) address - uint8_t(address >> 8) - option;
	for (size_t i = 0; i < cmd.size(); i++) {
			check -= (uint8_t) cmd[i];
	}

	return check;
}

int SerialW::readSerial(char* buf, int size){
	if (!Serial_Enabled) return -2;

	fd_set set;
	struct timeval timeout;
	int rv;

	FD_ZERO(&set); /* clear the set */
	FD_SET(USB, &set); /* add our file descriptor to the set */

	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;

	rv = select(USB + 1, &set, NULL, NULL, &timeout);
	if(rv == -1) {
		perror("select"); /* an error accured */
			return -1;
	}

	else if(rv == 0) {
		//printf("timeout \n"); /* a timeout occured */
		return 0;
	}
	else{
		int result = read( USB, buf , size); /* there was data to read */
		buf[size-1] = '\0';
		//printf(buf);printf("\n");
		return 1;
	}

}
