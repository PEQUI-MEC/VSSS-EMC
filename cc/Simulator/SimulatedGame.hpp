#ifndef VSSS_SIMULATEDGAME_HPP
#define VSSS_SIMULATEDGAME_HPP

#include "SimulatorClient.hpp"

class SimulatedGame {

    Game& game;
    SimulatorClient client;

    // placement targets
	std::vector<Target> blue_targets;
	std::vector<Target> yellow_targets;
    jsonSaveManager placement_config;

    public:
    SimulatedGame(Game& game) : game(game), client(game), placement_config(game, nullptr) {};
    void process_referee_cmds(bool send_placement);
    void reposition_robots();
    bool game_loop();
};

#endif //VSSS_SIMULATEDGAME_HPP
