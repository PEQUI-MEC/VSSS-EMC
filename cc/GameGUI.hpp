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

	Gtk::HBox swap_team_hbox;
	Gtk::Label swap_team_label;
	Gtk::CheckButton swap_team_button;

	public:
	GameGUI(Game& game);
	void update_strategy_combobox();
	void update_selected_strategy();
};

#endif //VSSS_GAMEGUI_HPP
