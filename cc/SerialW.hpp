#ifndef SERIALW_HPP_
#define SERIALW_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include "Robot.hpp"
class SerialW
{
public:
	bool Serial_Enabled;
	int	USB;
	std::string port;
SerialW()
	{
		Serial_Enabled =false;
	}

int start(std::string serial){
	struct termios tty;
	struct termios tty_old;
	memset (&tty, 0, sizeof tty);
	
	USB = open(serial.c_str(), O_RDWR| O_NOCTTY);
    if(USB != -1)
    {
        Serial_Enabled=true;
    }else{
		
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
cfsetospeed (&tty, (speed_t)B57600);
cfsetispeed (&tty, (speed_t)B57600);

/* Setting other Port Stuff */
tty.c_cflag     &=  ~PARENB;            // Make 8n1
tty.c_cflag     &=  ~CSTOPB;
tty.c_cflag     &=  ~CSIZE;
tty.c_cflag     |=  CS8;

tty.c_cflag     &=  ~CRTSCTS;           // no flow control
tty.c_cc[VMIN]   =  1;                  // read doesn't block
tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
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
	

void sendToRobot(Robot r){
	stringstream cmd;
	double temp0= floor(r.Vr*100)/100;
	double temp1= floor(r.Vl*100)/100;
	cmd<<r.ID<< temp0<<";"<<temp1<<"#";
	sendSerial(cmd.str());
	std::cout<<cmd.str()<<std::endl;
	}
void sendToThree(Robot r1,Robot r2,Robot r3){
	stringstream cmd;
	double temp0= round(r1.Vr*100)/100;
	double temp1= round(r1.Vl*100)/100;
	cmd<<r1.ID<< temp0<<";"<<temp1<<"#";
	 temp0= round(r2.Vr*100)/100;
	 temp1= round(r2.Vl*100)/100;
	cmd<<r2.ID<< temp0<<";"<<temp1<<"#";
	 temp0= round(r3.Vr*100)/100;
	 temp1= round(r3.Vl*100)/100;
	cmd<<r3.ID<< temp0<<";"<<temp1<<"#";
	sendSerial(cmd.str());
	std::cout<<cmd.str()<<std::endl;
	}


void sendSerial(std::string cmd){

int n_written = write( USB, cmd.c_str(),cmd.size());
	
	}
	
std::string readSerial(){
	
	int n = 0,
    spot = 0;
	char buf = '\0';

	/* Whole response*/
	char response[1024];
	memset(response, '\0', sizeof response);

do {
    n = read( USB, &buf, 1 );
    sprintf( &response[spot], "%c", buf );
    spot += n;
} while( buf != '\r' && n > 0);

if (n < 0) {
    std::cout << "Error reading: " << strerror(errno) << std::endl;
}
else if (n == 0) {
    std::cout << "Read nothing!" << std::endl;
}
else {
    std::cout << "Response: " << response << std::endl;
}
	
	return std::string(response);
	
	}	
};
#endif /* CONTROLGUI_HPP_ */
