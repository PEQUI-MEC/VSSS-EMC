#include "SimulatedGame.hpp"

time_point before = hrc::from_time_t(0);

void SimulatedGame::process_referee_cmds(bool send_placement) {
	if (!client.new_ref_cmd) return;
	
	client.new_ref_cmd = false;

	switch (client.ref_command.foul()) {
		case VSSRef::Foul::GAME_ON:
			game.playing_game = true;
			game.first_iteration = true;
			break;
		case VSSRef::Foul::FREE_KICK:
			game.stop_game();
			game.ball.reset_ls();
			break;
		case VSSRef::Foul::PENALTY_KICK:
			game.stop_game();
			game.ball.reset_ls();

			if(send_placement && client.ref_command.teamcolor() == VSSRef::Color::YELLOW) {
				if (game.blue_team().controlled) {
					VSSRef::Frame* frame = placement_config.load_replacement("blue", "penalty_defense");
					client.send_placement(frame);
				}
				if(game.yellow_team().controlled) {
					if((rand() % 10 + 1) >= 3) {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "penalty_attack_1");
						client.send_placement(frame);
					} else {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "penalty_attack_2");
						client.send_placement(frame);
					}
				}
			}
			if (send_placement && client.ref_command.teamcolor() == VSSRef::Color::BLUE) {
				if (game.yellow_team().controlled) {
					VSSRef::Frame* frame = placement_config.load_replacement("yellow", "penalty_defense");
					client.send_placement(frame);
				}
				if(game.blue_team().controlled){
					if((rand() % 10 + 1) >= 3){
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "penalty_attack_1");
						client.send_placement(frame);
					} else {
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "penalty_attack_2");
						client.send_placement(frame);
					}
				}
			}
			break;
		case VSSRef::Foul::GOAL_KICK:
			game.stop_game();
			game.ball.reset_ls();
			break;
		case VSSRef::Foul::FREE_BALL:
			game.stop_game();
			game.ball.reset_ls();

			if (send_placement && client.ref_command.foulquadrant() == VSSRef::Quadrant::QUADRANT_1) {
				if(game.yellow_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("yellow", "free_ball_q1");
					client.send_placement(frame);
				}
				if( game.blue_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("blue", "free_ball_q1");
					client.send_placement(frame);
				}
			}
			if (send_placement && client.ref_command.foulquadrant() == VSSRef::Quadrant::QUADRANT_2) {
				if(game.yellow_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("yellow", "free_ball_q2");
					client.send_placement(frame);
				}
				if( game.blue_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("blue", "free_ball_q2");
					client.send_placement(frame);
				}
			}
			if (send_placement && client.ref_command.foulquadrant() == VSSRef::Quadrant::QUADRANT_3){
				if(game.yellow_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("yellow", "free_ball_q3");
					client.send_placement(frame);
				}
				if( game.blue_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("blue", "free_ball_q3");
					client.send_placement(frame);
				}
			}
			if (send_placement && client.ref_command.foulquadrant() == VSSRef::Quadrant::QUADRANT_4){
				if(game.yellow_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("yellow", "free_ball_q4");
					client.send_placement(frame);
				}
				if( game.blue_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("blue", "free_ball_q4");
					client.send_placement(frame);
				}
			}
			break;

		case VSSRef::Foul::KICKOFF:
			game.stop_game();
			game.ball.reset_ls();

			if(send_placement && client.ref_command.teamcolor() == VSSRef::Color::YELLOW && game.yellow_team().controlled){
				VSSRef::Frame* frame = placement_config.load_replacement("yellow", "kickoff");
					client.send_placement(frame);
				
			}
			if(send_placement && client.ref_command.teamcolor() == VSSRef::Color::BLUE && game.blue_team().controlled){
					VSSRef::Frame* frame = placement_config.load_replacement("blue", "kickoff");
					client.send_placement(frame);					
			}
			if(send_placement && client.ref_command.teamcolor() == VSSRef::Color::YELLOW && game.blue_team().controlled){
				VSSRef::Frame* frame = placement_config.load_replacement("blue", "kickoff_defense");
				client.send_placement(frame);
			}
			if (send_placement && client.ref_command.teamcolor() == VSSRef::Color::BLUE && game.yellow_team().controlled){
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
	bool team_defending = (client.ref_command.teamcolor() == VSSRef::Color::BLUE && game.team->robot_color == RobotColor::Yellow)
		|| (client.ref_command.teamcolor() == VSSRef::Color::YELLOW && game.team->robot_color == RobotColor::Blue);

	game.team->strategy->set_foul(client.ref_command, team_defending, game.now());
	game.adversary->strategy->set_foul(client.ref_command, !team_defending, game.now());
}


bool SimulatedGame::game_loop() {
	if(!client.new_data && !client.new_ref_cmd &&
		!(game.playing_game && game.first_iteration))
		return false;

	std::lock_guard<std::mutex> guard(client.data_mutex);

	process_referee_cmds(true);

	if (game.send_one_command) {
		client.send_commands(*game.team.get(), *game.adversary.get());
		game.send_one_command = false;
	}

	if (!client.new_data && !game.first_iteration)
		return false;

	if(client.new_data) {
		client.update_robots(game);
	}

	if (game.playing_game) {
		auto inverted_team = game.team->get_inverted_robot_adversary();
		auto inverted_adv = game.adversary->get_inverted_robot_adversary();
		if (game.team->inverted_field) {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv,
											  game.ball.get_inverted(), game.first_iteration, true, game.team->inverted_field, game.now());
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team,
												   game.ball, game.first_iteration, true, game.adversary->inverted_field, game.now());
		} else {
			game.team->strategy->run_strategy(game.team->robots, inverted_adv, game.ball, game.first_iteration, true, game.team->inverted_field, game.now());
			game.adversary->strategy->run_strategy(game.adversary->robots, inverted_team,
												   game.ball.get_inverted(), game.first_iteration, true, game.adversary->inverted_field, game.now());
		}
		game.first_iteration = false;

		client.send_commands(*game.team.get(), *game.adversary.get());
	}

	return true;
}
