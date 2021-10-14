#include "SimulatedGame.hpp"

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
				game.stop_game();
				game.ball.reset_ls();
				break;
			case VSSRef::Foul::PENALTY_KICK:
				game.stop_game();
				game.ball.reset_ls();
				// if(client.ref_command.teamcolor() == VSSRef::Color::YELLOW && game.blue_team().controlled){
				// 	VSSRef::Frame* frame = placement_config.load_replacement("blue", "penalty_defense");
				// 	client.send_placement(frame);
				// }
				// if (client.ref_command.teamcolor() == VSSRef::Color::BLUE && game.yellow_team().controlled){
				// 	VSSRef::Frame* frame = placement_config.load_replacement("yellow", "penalty_defense");
				// 	client.send_placement(frame);
				// }

				if(client.ref_command.teamcolor() == VSSRef::Color::YELLOW && game.yellow_team().controlled){
					if((rand() % 10 + 1) >= 3){
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "penalty1");
						client.send_placement(frame);
					}else{
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "penalty2");
						client.send_placement(frame);
					}
				}

				if(client.ref_command.teamcolor() == VSSRef::Color::BLUE && game.blue_team().controlled){
					if((rand() % 10 + 1) >= 3){
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "penalty1");
						client.send_placement(frame);
					}else{
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "penalty2");
						client.send_placement(frame);
					}
				}
				break;
			case VSSRef::Foul::GOAL_KICK:
			case VSSRef::Foul::FREE_BALL:
			case VSSRef::Foul::KICKOFF:
				game.stop_game();
				game.ball.reset_ls();

				if(client.ref_command.teamcolor() == VSSRef::Color::YELLOW && game.yellow_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("yellow", "kickoff");
						client.send_placement(frame);
					
				}

				if(client.ref_command.teamcolor() == VSSRef::Color::BLUE && game.blue_team().controlled){
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "kickoff");
						client.send_placement(frame);					
				}
				if(client.ref_command.teamcolor() == VSSRef::Color::YELLOW && game.blue_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("blue", "kickoff_defense");
					client.send_placement(frame);
				}
				if (client.ref_command.teamcolor() == VSSRef::Color::BLUE && game.yellow_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("yellow", "kickoff_defense");
					client.send_placement(frame);
				}
				break;
			case VSSRef::Foul::STOP:
			case VSSRef::Foul::HALT:
				game.stop_game();
				game.ball.reset_ls();
				break;
		}
		game.team->strategy->set_foul(client.ref_command);
		game.adversary->strategy->set_foul(client.ref_command);
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
