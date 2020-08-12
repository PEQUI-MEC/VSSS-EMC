#ifndef VSSS_GAME_HPP
#define VSSS_GAME_HPP

#include <memory>
#include "Types.hpp"
#include "Team.hpp"
#include "Ball.hpp"

class Game {
	public:
	std::unique_ptr<Team> team;
	std::unique_ptr<Team> adversary;
	Ball ball;

	std::map<std::string, std::unique_ptr<Strategy>> strategies;

	bool is_simulated = false;
	bool playing_game = false;

	unsigned robot_count = 0;

	Game();
	void swap_teams();
	void set_strategy(Team& team, std::string strategy_name);
	std::optional<std::string> get_strategy_name(Team& team);
};

#endif //VSSS_GAME_HPP
