#include "GameGUI.hpp"
#include <typeinfo>

GameGUI::GameGUI(Game &game) : game(game) {
	add(vbox);
	set_label("Game Settings");
	team_strategy_label.set_label("Team Strategy:");
	vbox.pack_start(team_strategy_label, false, true, 5);
	vbox.pack_start(team_strategy, false, true, 5);
	adv_strategy_label.set_label("Adv Strategy:");
	vbox.pack_start(adv_strategy_label, false, true, 5);
	vbox.pack_start(adv_strategy, false, true, 5);
	team_strategy.signal_changed().connect([&](){
		game.set_strategy(*game.team, team_strategy.get_active_text());
	});
	adv_strategy.signal_changed().connect([&](){
		game.set_strategy(*game.adversary, adv_strategy.get_active_text());
	});

	swap_team_label.set_label("Swap Teams");
	swap_team_hbox.pack_start(swap_team_button, false, true, 5);
	swap_team_hbox.pack_start(swap_team_label, false, true, 5);
	swap_team_button.signal_clicked().connect([&](){
		game.swap_teams();
		update_selected_strategy();
	});
	vbox.pack_start(swap_team_hbox);

	update_strategy_combobox();
}

void GameGUI::update_strategy_combobox() {
	team_strategy.remove_all();
	for (auto& [name, strategy] : game.strategies) {
		team_strategy.append(name);
	}
	adv_strategy.remove_all();
	for (auto& [name, strategy] : game.strategies) {
		adv_strategy.append(name);
	}
	update_selected_strategy();
}

void GameGUI::update_selected_strategy() {
	for (auto& [name, strategy] : game.strategies) {
		if (game.team->strategy != nullptr && typeid(*strategy) == typeid(*game.team->strategy)) {
			team_strategy.set_active_text(name.c_str());
		}
		if (game.adversary->strategy != nullptr && typeid(*strategy) == typeid(*game.adversary->strategy)) {
			adv_strategy.set_active_text(name.c_str());
		}
	}
}
