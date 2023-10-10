#ifndef VSSS_GAME_HPP
#define VSSS_GAME_HPP

#include <memory>
#include <vision/Tags.hpp>
#include "Types.hpp"
#include "Team.hpp"
#include "Ball.hpp"
#include <chrono>

class Game {
	public:
	std::unique_ptr<Team> team;
	std::unique_ptr<Team> adversary;
	Ball ball;

	std::map<std::string, std::unique_ptr<Strategy>> strategies;

	bool is_simulated = false;
	bool playing_game = false;
	bool first_iteration = false;
	bool send_one_command = false;
	bool automatic_positioning = false;

	unsigned robot_count = 0;
	int simulation_id = 0;

	time_point simulated_time = hrc::from_time_t(0);

	Game(int simulation_id = 0);
	void swap_teams();
	void set_strategy(Team& team, std::string strategy_name);
	std::optional<std::string> get_strategy_name(Team& team);
	Team& yellow_team();
	Team& blue_team();
	Tags to_tags();
	void stop_game();
	time_point now();
};

#endif //VSSS_GAME_HPP
