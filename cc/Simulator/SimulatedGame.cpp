#include "SimulatedGame.hpp"

void SimulatedGame::test_pid() {
    Pose current_pose = game.team->robots[0].pose;
	Pose target_pose = game.team->robots[0].target.pose;

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	auto time_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

	if (time_since_start.count() < 1600) {
		game.team->robots[0].set_target_wheel_velocity(0.8, 0.8);
	} else {
		game.team->robots[0].set_target_wheel_velocity(0.6, 0.6);
	}

    log_data << time_since_start.count() << ", " <<
		current_pose.wheel_velocity.left << ", " << current_pose.wheel_velocity.right << ", "  <<
		target_pose.wheel_velocity.left << ", " << target_pose.wheel_velocity.right << ", "  <<
		current_pose.velocity.linear << ", " << current_pose.velocity.angular << ", "  <<
		target_pose.velocity.linear << ", " << target_pose.velocity.angular << std::endl;
    
    client.send_commands(*game.team.get());
}

bool SimulatedGame::game_loop() {
	if(!client.new_data && !client.new_ref_cmd) return false;

	std::lock_guard<std::mutex> guard(client.data_mutex);

	if (client.new_ref_cmd) {
		client.new_ref_cmd = false;
		switch (client.ref_command.foul()) {
			case VSSRef::Foul::GAME_ON:
				game.playing_game = true;
				break;
			case VSSRef::Foul::FREE_KICK:
			case VSSRef::Foul::PENALTY_KICK:
			case VSSRef::Foul::GOAL_KICK:
			case VSSRef::Foul::FREE_BALL:
			case VSSRef::Foul::KICKOFF:
			case VSSRef::Foul::STOP:
			case VSSRef::Foul::HALT:
				game.stop_game();
				game.ball.reset_ls();
				break;
		}
		game.team->strategy->set_foul(client.ref_command.foul());
		game.adversary->strategy->set_foul(client.ref_command.foul());
	}

	if (game.send_one_command) {
		if (game.team->controlled) {
			client.send_commands(*game.team.get());
		}
		if (game.adversary->controlled) {
			client.send_commands(*game.adversary.get());
		}
		game.send_one_command = false;
	}

	if(!client.new_data) return false;

	client.update_robots(game);
    
    // Para testes
    if (game.playing_game) {
		if (!start_set) {
			start = std::chrono::system_clock::now();
			start_set = true;
		}
        test_pid();
    }
    return true;

	if (game.playing_game) {
		auto inverted_team = game.team->get_inverted_robot_positions();
		auto inverted_adv = game.adversary->get_inverted_robot_positions();
		if (game.team->inverted_field) {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv,
											  game.ball.get_inverted(), game.first_iteration);
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team,
												   game.ball, game.first_iteration);
		} else {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv, game.ball, game.first_iteration);
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team,
												   game.ball.get_inverted(), game.first_iteration);
		}
		game.first_iteration = false;

		if (game.team->controlled) {
			client.send_commands(*game.team.get());
		}
		if (game.adversary->controlled) {
			client.send_commands(*game.adversary.get());
		}
	}

    return true;
}
