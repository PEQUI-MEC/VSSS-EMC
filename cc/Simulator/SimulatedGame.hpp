#ifndef VSSS_SIMULATEDGAME_HPP
#define VSSS_SIMULATEDGAME_HPP

#include "SimulatorClient.hpp"

class SimulatedGame {

    Game& game;
    public:
    SimulatorClient client;


    jsonSaveManager placement_config;

    public:
    SimulatedGame(Game& game) : game(game), client(game), placement_config(game, nullptr) {};
    void process_referee_cmds(bool send_placement);
    bool game_loop();
};

#endif //VSSS_SIMULATEDGAME_HPP
