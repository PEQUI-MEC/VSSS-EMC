#ifndef VSSS_MAINLOOP_HPP
#define VSSS_MAINLOOP_HPP

#include <memory>
#include <Strategy3/Team.hpp>
#include <thread>
#include <v4lcap.hpp>

enum class GameState {
	idle, play, test_on_click
};

class MainLoop {
	GameState game_state = GameState::idle;
	bool simulated = false;

	std::unique_ptr<Team> team;
	std::unique_ptr<Team> adversary;
	Ball ball;

	std::thread main_thread;

	capture::v4lcap vcap;

	MainLoop();
	void main_loop();
};

#endif //VSSS_MAINLOOP_HPP
