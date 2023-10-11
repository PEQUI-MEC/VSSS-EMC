#include "Game.hpp"
#include <Strategy2/AIStrategy.hpp>
#include "NoStrategy.hpp"
#include "JoystickStrategy.hpp"

Game::Game(int simulation_id) : simulation_id(simulation_id) {
	robot_count = 3;
	strategies.emplace("Manual Strategy", new Strategy3());
	strategies.emplace("AI Strategy", new AIStrategy());
	strategies.emplace("Joystick 0", new JoystickStrategy("/dev/input/js0"));
	strategies.emplace("No Strategy", new NoStrategy());
	team = std::make_unique<Team>(robot_count, 0, false, RobotColor::Blue);
	set_strategy(*team, "Manual Strategy");
	adversary = std::make_unique<Team>(robot_count, 0, true, RobotColor::Yellow);
	set_strategy(*adversary, "No Strategy");
}

void Game::swap_teams() {
	team.swap(adversary);
}

void Game::set_strategy(Team &team, std::string strategy_name) {
	if (team.strategy == nullptr || typeid(*team.strategy) != typeid(*strategies[strategy_name])) {
		team.strategy.reset(strategies[strategy_name]->clone());
	}
//	Times sem estratégia não são controlados, não usam a pick-a-tag
	team.controlled = typeid(*team.strategy) != typeid(*strategies["No Strategy"]);
	team.using_joystick = typeid(*team.strategy) == typeid(*strategies["Joystick 0"]);
	team.strategy->params = team.params;
}

std::optional<std::string> Game::get_strategy_name(Team &team) {
	if (team.strategy == nullptr) return std::nullopt;

	for(auto& [name, strategy] : strategies) {
		if (typeid(*team.strategy) == typeid(*strategy))
			return name;
	}
	return std::nullopt;
}

Team &Game::yellow_team() {
	if (team->robot_color == RobotColor::Yellow) return *team;
	else return *adversary;
}

Team &Game::blue_team() {
	if (team->robot_color == RobotColor::Blue) return *team;
	else return *adversary;
}

Tags Game::to_tags() {
	Tags tags;
	auto& yellow = yellow_team();
	tags.yellow.resize(yellow.robots.size());
	std::transform(yellow.robots.begin(), yellow.robots.end(), tags.yellow.begin(),
				   [&](Robot3& robot) {return robot.to_tag(yellow.inverted_field);});
	tags.yellow_has_orientation = yellow.controlled;
	auto& blue = blue_team();
	tags.blue.resize(blue.robots.size());
	std::transform(blue.robots.begin(), blue.robots.end(), tags.blue.begin(),
				   [&](Robot3& robot) {return robot.to_tag(blue.inverted_field);});
	tags.blue_has_orientation = blue.controlled;
	tags.ball.position = ball.position.to_cv_point();
	tags.found_ball = true;
	return tags;
}

void Game::stop_game() {
	for (auto& robot : team->robots) {
			robot.stop();
		}
	for (auto& robot : adversary->robots) {
		robot.stop();
	}
	send_one_command = true;
	playing_game = false;
	std::cout << "STOP THE GAME" << std::endl;
}

time_point Game::now() {
	if (is_simulated) {
		return simulated_time;
	} else {
		return hrc::now();
	}
}
