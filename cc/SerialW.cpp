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

void SerialW::sendToRobot(Robot r){
	if (!Serial_Enabled) return;
	std::stringstream cmd;
	double temp0= floor(r.Vr*100)/100;
	double temp1= floor(r.Vl*100)/100;
	cmd<<r.ID<<'@'<< temp0<<";"<<temp1<<"#";
	sendSerial(cmd.str());
	//std::cout<<cmd.str()<<std::endl;
}

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
				cmd<<robot_list[i].ID<<'@'<<"P"<<temp0<<";"<<temp1<<";"<<temp2<<"#"<< std::endl;
			}
			break;
			case SPEED:
			temp0= round(robot_list[i].Vr*100)/100;
			temp1= round(robot_list[i].Vl*100)/100;
			cmd<<robot_list[i].ID<<'@'<<temp0<<";"<<temp1<<"#"<< std::endl;
			break;
			case ORIENTATION:
			temp2 = double(robot_list[i].orientation) - ( - double(robot_list[i].targetOrientation));
			temp0= temp2*180/PI;
			temp1= round(double(robot_list[i].vmax)*100)/100;
			cmd << robot_list[i].ID<<'@'<<"O"<<temp0<<";"<<temp1<<"#"<< std::endl;
			//std::cout << cmd.str() << std::endl;
			break;
			case VECTOR:
			temp0= double(atan2(sin(robot_list[i].orientation-(-robot_list[i].transAngle)),cos(robot_list[i].orientation-(-robot_list[i].transAngle)))*180/PI);
			temp1= round(double(robot_list[i].vmax)*100)/100;
			cmd << robot_list[i].ID<<'@'<<"V"<<temp0<<";"<<temp1<<"#"<< std::endl;
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
				cmd<<robot_list[i].ID<<'@'<<"P"<<temp0<<";"<<temp1<<";"<<temp2<<"#"<< std::endl;
			}
		}
	}
	if (!cmd.str().empty()) sendSerial(cmd.str());
	//std::cout<<cmd.str()<<std::endl;
}

void SerialW::sendSerial(std::string cmd){
	if (!Serial_Enabled) return;
	int result = write(USB, cmd.c_str(), cmd.size());
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
