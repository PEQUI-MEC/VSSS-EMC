#ifndef VSSS_SIMULATEDGAME_HPP
#define VSSS_SIMULATEDGAME_HPP

#include "SimulatorClient.hpp"

class SimulatedGame {

    SimulatorClient client;
    Game& game;

    public:
    SimulatedGame(Game& game) : game(game){};
    bool game_loop();
};

#endif //VSSS_SIMULATEDGAME_HPP