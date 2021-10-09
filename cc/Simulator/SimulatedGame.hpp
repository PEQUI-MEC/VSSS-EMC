#ifndef VSSS_SIMULATEDGAME_HPP
#define VSSS_SIMULATEDGAME_HPP

#include "SimulatorClient.hpp"


#include <iostream>
#include <fstream>
#include <chrono>

class SimulatedGame {

    SimulatorClient client;
    Game& game;

    std::ofstream log_data;

    std::chrono::system_clock::time_point start;
    bool start_set = false;

    public:
    SimulatedGame(Game& game) : game(game),
                log_data("velocity_logs.csv") {
        start = std::chrono::system_clock::now();
    };
    bool game_loop();
    void test_pid();
};

#endif //VSSS_SIMULATEDGAME_HPP
