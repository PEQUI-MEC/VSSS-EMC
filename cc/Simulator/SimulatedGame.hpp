#ifndef VSSS_SIMULATEDGAME_HPP
#define VSSS_SIMULATEDGAME_HPP

#include "SimulatorClient.hpp"

class SimulatedGame {

    Game& game;
    SimulatorClient client;


    jsonSaveManager placement_config;

    public:
    SimulatedGame(Game& game) : game(game), client(game), placement_config(game, nullptr) {};
    bool game_loop();
};

#endif //VSSS_SIMULATEDGAME_HPP
