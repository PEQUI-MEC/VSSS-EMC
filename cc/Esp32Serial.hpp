#ifndef ESP32SERIAL_HPP
#define ESP32SERIAL_HPP

#include <string>

class Esp32Serial {
    public:
    std::string port;
    int serial_port = -1;

    std::vector<std::string> received_messages;
    
    Esp32Serial(const std::string &port, int baud);
    ~Esp32Serial();
    void add_robot_mac(const char ID, const std::string& mac);
    void send_string_msg(const std::string& msg);
    void send_msg(const char ID, const std::string& msg);
    std::string send_get_answer(const char ID, const std::string& msg);
    std::string receive_msgs_thread();
};

#endif // ESP32SERIAL_HPP