#include "Esp32Serial.hpp"

Esp32Serial::Esp32Serial(const std::string &port, int baud) {
    // TODO: set mac for each robot
    add_robot_mac('A', "FF:FF:FF:FF:FF:FF");
    add_robot_mac('B', "FF:FF:FF:FF:FF:FF");
    add_robot_mac('C', "FF:FF:FF:FF:FF:FF");
    add_robot_mac('D', "FF:FF:FF:FF:FF:FF");
    add_robot_mac('E', "FF:FF:FF:FF:FF:FF");
    add_robot_mac('F', "FF:FF:FF:FF:FF:FF");

    
}

Esp32Serial::~Esp32Serial() {

}

void Esp32Serial::add_robot_mac(const char ID, const std::string& mac) {

}

void Esp32Serial::send_msg(const char ID, const std::string& msg) {

}

std::string Esp32Serial::send_get_answer(const char ID, const std::string& msg) {
    return std::string();
}