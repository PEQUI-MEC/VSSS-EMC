#ifndef VSSS_SIMULATEDGAME_HPP
#define VSSS_SIMULATEDGAME_HPP

#include "SimulatorClient.hpp"

class SimulatedGame {

    SimulatorClient client;
    Game& game;

    jsonSaveManager placement_config;

    public:
    SimulatedGame(Game& game) : game(game), placement_config(game, nullptr) {};
    bool game_loop();
};

#endif //VSSS_SIMULATEDGAME_HPP