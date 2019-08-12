#ifndef VSSS_SIMULATORGUI_HPP
#define VSSS_SIMULATORGUI_HPP

#include <gtkmm.h>
#include "Simulation.hpp"
#include <Strategy2/Team.h>

class SimulationGUI : public Gtk::VBox {
	public:
//		References from other classes
		capture::V4LInterface& interface;
    Teams& current_team;
		int argc;
		char** argv;

		std::optional<Simulation> simulation = std::nullopt;
		std::optional<Simulation> simulation_team2 = std::nullopt;

    Gtk::Frame simu_frame;
    Gtk::Grid simu_grid;

		//Gtk::HBox use_simu_hbox;
		Gtk::CheckButton use_simu_button;
    Gtk::Label team_title_label;
    Gtk::RadioButton r_team_button, l_team_button;

		SimulationGUI(capture::V4LInterface &interface_ref, Teams& curr_team, int argc, char **argv);
		void enable_simulator_callback();
    void select_team_callback();

  private:
    void change_team(Teams new_team);
};

#endif //VSSS_SIMULATORGUI_HPP
