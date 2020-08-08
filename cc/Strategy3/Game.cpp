#include "Game.hpp"
#include <Strategy2/AIStrategy.hpp>
#include "NoStrategy.hpp"

Game::Game() {
	robot_count = 3;
	strategies.emplace("Manual Strategy", new Strategy3());
	strategies.emplace("AI Strategy", new AIStrategy());
	strategies.emplace("No Strategy", new NoStrategy());
	team = std::make_unique<Team>(robot_count, 0, true, false, RobotColor::Blue);
	set_strategy(*team, "Manual Strategy");
	adversary = std::make_unique<Team>(robot_count, robot_count, true, true, RobotColor::Yellow);
	set_strategy(*adversary, "No Strategy");
}

void Game::swap_teams() {
	team.swap(adversary);
}

void Game::set_strategy(Team &team, std::string strategy_name) {
	if (team.strategy == nullptr || typeid(*team.strategy) != typeid(*strategies[strategy_name])) {
		team.strategy.reset(strategies[strategy_name]->clone());
	}
}
