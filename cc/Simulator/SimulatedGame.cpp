#include "SimulatedGame.hpp"

time_point before = hrc::from_time_t(0);

void SimulatedGame::process_referee_cmds(bool send_placement) {
	if (client.ref_command_queue.empty()) return;

	auto ref_command = client.ref_command_queue.front();
	client.ref_command_queue.pop();

	std::vector<Target> blue_targets;
	std::vector<Target> yellow_targets;

	switch (ref_command.foul()) {
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

			if(ref_command.teamcolor() == VSSRef::Color::YELLOW) {
				if (game.blue_team().controlled) {
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "penalty_defense");
						client.send_placement(frame);
					}
					blue_targets = placement_config.load_positioning_targets("blue", "penalty_defense");
				}
				if(game.yellow_team().controlled) {
					if((rand() % 10 + 1) >= 3) {
						if (send_placement) {
							VSSRef::Frame* frame = placement_config.load_replacement("yellow", "penalty_attack_1");
							client.send_placement(frame);
						}
						yellow_targets = placement_config.load_positioning_targets("yellow", "penalty_attack_1");
					} else {
						if (send_placement) {
							VSSRef::Frame* frame = placement_config.load_replacement("yellow", "penalty_attack_2");
							client.send_placement(frame);
						}
						yellow_targets = placement_config.load_positioning_targets("yellow", "penalty_attack_2");
					}
				}
			}
			if (ref_command.teamcolor() == VSSRef::Color::BLUE) {
				if (game.yellow_team().controlled) {
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "penalty_defense");
						client.send_placement(frame);
					}
					yellow_targets = placement_config.load_positioning_targets("yellow", "penalty_defense");
				}
				if(game.blue_team().controlled){
					if((rand() % 10 + 1) >= 3){
						if (send_placement) {
							VSSRef::Frame* frame = placement_config.load_replacement("blue", "penalty_attack_1");
							client.send_placement(frame);
						}
						blue_targets = placement_config.load_positioning_targets("blue", "penalty_attack_1");
					} else {
						if (send_placement) {
							VSSRef::Frame* frame = placement_config.load_replacement("blue", "penalty_attack_2");
							client.send_placement(frame);
						}
						blue_targets = placement_config.load_positioning_targets("blue", "penalty_attack_2");
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

			if (ref_command.foulquadrant() == VSSRef::Quadrant::QUADRANT_1) {
				if(game.yellow_team().controlled){
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "free_ball_q1");
						client.send_placement(frame);
					}
					yellow_targets = placement_config.load_positioning_targets("yellow", "free_ball_q1");
				}
				if( game.blue_team().controlled){
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "free_ball_q1");
						client.send_placement(frame);
					}
					blue_targets = placement_config.load_positioning_targets("blue", "free_ball_q1");
				}
			}
			if (ref_command.foulquadrant() == VSSRef::Quadrant::QUADRANT_2) {
				if(game.yellow_team().controlled){
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "free_ball_q2");
						client.send_placement(frame);
					}
					yellow_targets = placement_config.load_positioning_targets("yellow", "free_ball_q2");
				}
				if(game.blue_team().controlled){
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "free_ball_q2");
						client.send_placement(frame);
					}
					blue_targets = placement_config.load_positioning_targets("blue", "free_ball_q2");
				}
			}
			if (ref_command.foulquadrant() == VSSRef::Quadrant::QUADRANT_3){
				if(game.yellow_team().controlled){
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "free_ball_q3");
						client.send_placement(frame);
					}
					yellow_targets = placement_config.load_positioning_targets("yellow", "free_ball_q3");
				}
				if( game.blue_team().controlled){
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "free_ball_q3");
						client.send_placement(frame);
					}
					blue_targets = placement_config.load_positioning_targets("blue", "free_ball_q3");
				}
			}
			if (ref_command.foulquadrant() == VSSRef::Quadrant::QUADRANT_4){
				if(game.yellow_team().controlled){
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "free_ball_q4");
						client.send_placement(frame);
					}
					yellow_targets = placement_config.load_positioning_targets("yellow", "free_ball_q4");
				}
				if(game.blue_team().controlled){
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "free_ball_q4");
						client.send_placement(frame);
					}
					blue_targets = placement_config.load_positioning_targets("blue", "free_ball_q4");
				}
			}
			break;

		case VSSRef::Foul::KICKOFF:
			game.stop_game();
			game.ball.reset_ls();

			if(ref_command.teamcolor() == VSSRef::Color::YELLOW && game.yellow_team().controlled){
				if (send_placement) {
					VSSRef::Frame* frame = placement_config.load_replacement("yellow", "kickoff");
					client.send_placement(frame);
				}
				yellow_targets = placement_config.load_positioning_targets("yellow", "kickoff");
			}
			if(ref_command.teamcolor() == VSSRef::Color::BLUE && game.blue_team().controlled){
				if (send_placement) {
					VSSRef::Frame* frame = placement_config.load_replacement("blue", "kickoff");
					client.send_placement(frame);
				}
				blue_targets = placement_config.load_positioning_targets("blue", "kickoff");
			}
			if(ref_command.teamcolor() == VSSRef::Color::YELLOW && game.blue_team().controlled){
				if (send_placement) {
					VSSRef::Frame* frame = placement_config.load_replacement("blue", "kickoff_defense");
					client.send_placement(frame);
				}
				blue_targets = placement_config.load_positioning_targets("blue", "kickoff_defense");
			}
			if (ref_command.teamcolor() == VSSRef::Color::BLUE && game.yellow_team().controlled){
				if (send_placement) {
					VSSRef::Frame* frame = placement_config.load_replacement("yellow", "kickoff_defense");
					client.send_placement(frame);
				}
				yellow_targets = placement_config.load_positioning_targets("yellow", "kickoff_defense");
			}
			break;
		case VSSRef::Foul::STOP:
		case VSSRef::Foul::HALT:
			game.stop_game();
			game.ball.reset_ls();
			break;
	}
	// print fould:
	switch (ref_command.foul()) {
		case VSSRef::Foul::GAME_ON:
			std::cout << "GAME ON" << std::endl;
			break;
		case VSSRef::Foul::FREE_KICK:
			std::cout << "FREE KICK" << std::endl;
			break;
		case VSSRef::Foul::PENALTY_KICK:
			std::cout << "PENALTY KICK" << std::endl;
			break;
		case VSSRef::Foul::GOAL_KICK:
			std::cout << "GOAL KICK" << std::endl;
			break;
		case VSSRef::Foul::FREE_BALL:
			std::cout << "FREE BALL" << std::endl;
			break;
		case VSSRef::Foul::KICKOFF:
			std::cout << "KICKOFF" << std::endl;
			break;
		case VSSRef::Foul::STOP:
			std::cout << "STOP" << std::endl;
			break;
		case VSSRef::Foul::HALT:
			std::cout << "HALT" << std::endl;
			break;
	}


	bool team_defending = (ref_command.teamcolor() == VSSRef::Color::BLUE && game.team->robot_color == RobotColor::Yellow)
		|| (ref_command.teamcolor() == VSSRef::Color::YELLOW && game.team->robot_color == RobotColor::Blue);

	game.team->strategy->set_foul(ref_command, team_defending, game.now());
	game.adversary->strategy->set_foul(ref_command, !team_defending, game.now());

	if (!game.is_simulated) {
		for (int i = 0; i < 3; i++) {
			if (blue_targets.size() > i && game.blue_team().controlled) {
				game.blue_team().robots[i].go_to_and_stop_orientation(blue_targets[i].pose.position, blue_targets[i].pose.orientation, 0.8);
			}
			if (yellow_targets.size() > i && game.yellow_team().controlled) {
				game.yellow_team().robots[i].go_to_and_stop_orientation(yellow_targets[i].pose.position, yellow_targets[i].pose.orientation, 0.8);
			}
		}
		game.send_one_command = true;
	}
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
