#ifndef ESP32SERIAL_HPP
#define ESP32SERIAL_HPP

#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <unordered_map>

class Esp32Serial {
    public:
    std::string port;
    int serial_port = -1;

    std::vector<std::string> received_messages;

    bool stop_receive_thread = false;
    std::thread receive_thread;
    std::thread send_thread;

    std::unordered_map<char, float> batteries;

    std::queue<std::string> serial_queue;
    
    Esp32Serial(const std::string &port, int baud);
    ~Esp32Serial();
    void add_robot_mac(const char ID, const std::string& mac);
    void send_string_msg(const std::string& msg);
    void send_msg(const char ID, const std::string& msg);
    void receive_msgs_thread();
    void send_msg_thread();
};

#endif // ESP32SERIAL_HPP