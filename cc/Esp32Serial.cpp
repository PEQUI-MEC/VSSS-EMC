#include "Esp32Serial.hpp"
#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

Esp32Serial::Esp32Serial(const std::string &port, int baud) {
    // Open the serial port
    serial_port = open(port.c_str(), O_RDWR);

    // Print an error message if the serial port cannot be opened
    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    // Get the current terminal settings
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return;
    }

    // Set the serial port settings
    tty.c_cflag &= ~PARENB; // Disable parity bit
    tty.c_cflag &= ~CSTOPB; // Use 1 stop bit
    tty.c_cflag &= ~CSIZE; // Clear number of data bits
    tty.c_cflag |= CS8; // Use 8 data bits per byte
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS flow control
    tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore control lines

    tty.c_lflag &= ICANON; // Disable canonical mode
    tty.c_lflag &= ECHO; // Disable echo
    tty.c_lflag &= ECHOE; // Disable erasing of input line
    tty.c_lflag &= ECHONL; // Disable new-line echo
    tty.c_lflag &= ISIG; // Disable INTR, QUIT, and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable special handling of bytes

    tty.c_oflag &= ~OPOST; // Disable interpreting newline
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return

    tty.c_cc[VTIME] = 10; // Wait for up to 1 second to receive data
    tty.c_cc[VMIN] = 0;

    // Set the baud rate
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    // Save the terminal settings
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return;
    }
}

Esp32Serial::~Esp32Serial() {

}

void Esp32Serial::add_robot_mac(const char ID, const std::string& mac) {

}

void Esp32Serial::send_string_msg(const std::string& msg) {
    // std::cout << "string: " << msg << std::endl;
    if (serial_port > 0) {
        write(serial_port, msg.c_str(), msg.length());
    }
}

void Esp32Serial::send_msg(const char ID, const std::string& msg) {
    std::string full_msg = std::string(1, ID) + '@' + msg;
    if (serial_port > 0) {
        write(serial_port, full_msg.c_str(), full_msg.length());
    }
}

std::string Esp32Serial::send_get_answer(const char ID, const std::string& msg) {
    return std::string();
}

std::string Esp32Serial::receive_msgs_thread() {
    
}