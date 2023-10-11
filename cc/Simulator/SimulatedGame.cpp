#include "SimulatedGame.hpp"

time_point before = hrc::from_time_t(0);

double total_reposition_distance(std::vector<Target>& target, std::vector<Robot3>& robots) {
	double total_distance = 0;
	for (int i = 0; i < 3; i++) {
		total_distance += distance(target[i].pose.position, robots[i].get_position());
	}
	return total_distance;
}

// Defined in Strategy3.cpp
extern std::array<std::array<int, 3>, 6> all_possible_permutations();

std::vector<Target> select_best_reposition(std::vector<Target> targets, std::vector<Robot3>& robots) {
	std::array<std::array<int, 3>, 6> permutations = all_possible_permutations();
	auto best_target_order = *std::min_element(permutations.begin(), permutations.end(),
									[&](std::array<int, 3> p1, std::array<int, 3> p2) {
		std::vector<Target> targets1 = {targets[p1[0]], targets[p1[1]], targets[p1[2]]};
		std::vector<Target> targets2 = {targets[p2[0]], targets[p2[1]], targets[p2[2]]};
		return total_reposition_distance(targets1, robots) < total_reposition_distance(targets2, robots);
	});
	return {targets[best_target_order[0]], targets[best_target_order[1]], targets[best_target_order[2]]};
}

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

			if (ref_command.teamcolor() == VSSRef::Color::YELLOW) {
				if (game.blue_team().controlled) {
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "free_kick_defense");
						client.send_placement(frame);
					}
					blue_targets = placement_config.load_positioning_targets("blue", "free_kick_defense");
				}
				if (game.yellow_team().controlled) {
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "free_kick_attack");
						client.send_placement(frame);
					}
					yellow_targets = placement_config.load_positioning_targets("yellow", "free_kick_attack");
				}
			} else if (ref_command.teamcolor() == VSSRef::Color::BLUE) {
				if (game.yellow_team().controlled) {
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "free_kick_defense");
						client.send_placement(frame);
					}
					yellow_targets = placement_config.load_positioning_targets("yellow", "free_kick_defense");
				}
				if (game.blue_team().controlled) {
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "free_kick_attack");
						client.send_placement(frame);
					}
					blue_targets = placement_config.load_positioning_targets("blue", "free_kick_attack");
				}
			}

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

			if (ref_command.teamcolor() == VSSRef::Color::YELLOW) {
				if (game.blue_team().controlled) {
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "goal_kick_defense");
						client.send_placement(frame);
					}
					blue_targets = placement_config.load_positioning_targets("blue", "goal_kick_defense");
				}
				if (game.yellow_team().controlled) {
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "goal_kick_attack");
						client.send_placement(frame);
					}
					yellow_targets = placement_config.load_positioning_targets("yellow", "goal_kick_attack");
				}
			} else if (ref_command.teamcolor() == VSSRef::Color::BLUE) {
				if (game.yellow_team().controlled) {
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("yellow", "goal_kick_defense");
						client.send_placement(frame);
					}
					yellow_targets = placement_config.load_positioning_targets("yellow", "goal_kick_defense");
				}
				if (game.blue_team().controlled) {
					if (send_placement) {
						VSSRef::Frame* frame = placement_config.load_replacement("blue", "goal_kick_attack");
						client.send_placement(frame);
					}
					blue_targets = placement_config.load_positioning_targets("blue", "goal_kick_attack");
				}
			}

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
			game.stop_game();
			game.ball.reset_ls();
			break;
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
		if (game.blue_team().controlled && blue_targets.size() >= 3) {
			auto best_target_order = select_best_reposition(blue_targets, game.blue_team().robots);
			for (int i = 0; i < 3; i++) {
				game.blue_team().robots[i].go_to_and_stop_orientation(best_target_order[i].pose.position,
					best_target_order[i].pose.orientation, 0.5);
			}
		}
		if (game.yellow_team().controlled && yellow_targets.size() >= 3) {
			auto best_target_order = select_best_reposition(yellow_targets, game.yellow_team().robots);
			for (int i = 0; i < 3; i++) {
				game.yellow_team().robots[i].go_to_and_stop_orientation(best_target_order[i].pose.position,
					best_target_order[i].pose.orientation, 0.5);
			}
		}
	}
	if (blue_targets.size() > 0 || yellow_targets.size() > 0) {
		game.automatic_positioning = true; // is set to false on game.stop_game()
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
