#ifndef VSSS_SIMULATORGUI_HPP
#define VSSS_SIMULATORGUI_HPP

#include <gtkmm.h>
#include "Simulation.hpp"

class SimulationGUI : public Gtk::VBox {
	public:
//		References from other classes
		std::array<Robot2 *, 3> &robots;
		Ball& ball;
		Strategy2 strategy;
		capture::V4LInterface& interface;
		int argc;
		char** argv;

		std::optional<Simulation> simulation = std::nullopt;

		Gtk::HBox use_simu_hbox;
		Gtk::CheckButton use_simu_button;
		Gtk::Label use_simu_label;

		SimulationGUI(std::array<Robot2 *, 3> &robots, Ball &ball, Strategy2 &strategy_ref,
					  capture::V4LInterface &interface_ref, int argc, char **argv);
		void enable_simulator_callback();
};

#endif //VSSS_SIMULATORGUI_HPP
