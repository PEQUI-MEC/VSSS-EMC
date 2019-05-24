#ifndef VSSS_SIMULATORGUI_HPP
#define VSSS_SIMULATORGUI_HPP

#include <gtkmm.h>
#include "Simulation.hpp"

class SimulationGUI : public Gtk::VBox {
	public:
//		References from other classes
		Ball& ball;
		capture::V4LInterface& interface;
		int argc;
		char** argv;

		std::optional<Simulation> simulation = std::nullopt;
		std::optional<Simulation> simulation_team2 = std::nullopt;

		Gtk::HBox use_simu_hbox;
		Gtk::CheckButton use_simu_button;
		Gtk::Label use_simu_label;

		SimulationGUI(Ball &ball, capture::V4LInterface &interface_ref, int argc, char **argv);
		void enable_simulator_callback();
};

#endif //VSSS_SIMULATORGUI_HPP
