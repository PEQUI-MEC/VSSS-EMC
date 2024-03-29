#ifndef VSSS_GAMEGUI_HPP
#define VSSS_GAMEGUI_HPP

#include <Strategy3/Game.hpp>
#include <gtkmm.h>

class GameGUI : public Gtk::Frame {
	Game& game;

	Gtk::VBox vbox;

	Gtk::Label team_strategy_label;
	Gtk::ComboBoxText team_strategy;
	Gtk::Label adv_strategy_label;
	Gtk::ComboBoxText adv_strategy;

	Gtk::HBox inverted_hbox;
	Gtk::Label inverted_label;

	Gtk::HBox swap_team_hbox;
	Gtk::Label swap_team_label;
	Gtk::CheckButton swap_team_button;

	Gtk::HBox use_simulator_hbox;
	Gtk::Label use_simulator_label;

	public:
	Gtk::CheckButton use_simulator_button;
	Gtk::CheckButton inverted_button;

	GameGUI(Game& game);
	void update_strategy_combobox();
	void update_selected_strategy();
};

#endif //VSSS_GAMEGUI_HPP
