#include "MainLoop.hpp"

MainLoop::MainLoop() {
	team = std::make_unique<Team>(true, true);
	adversary = std::make_unique<Team>(false, false);
	main_thread = std::thread(&MainLoop::main_loop, this);
}

void MainLoop::main_loop() {
	if (simulated) {
//		get robot positions from simulator
	} else {
//		get robot positions from image
	}

	if (game_state == GameState::play) {
		team->strategy->run_strategy(team->robots, adversary->robots, ball);
		if(adversary->controlled) {
			adversary->strategy->run_strategy(adversary->robots, team->robots, ball);
		}
	}

	if (game_state == GameState::play || game_state == GameState::test_on_click) {
		if (simulated) {
//			send command to simulator
		} else {
//			send command to robots
		}
	}
}

