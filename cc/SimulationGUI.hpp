#ifndef VSSS_SIMULATORGUI_HPP
#define VSSS_SIMULATORGUI_HPP

#include <gtkmm.h>
#include "Simulation.hpp"

class SimulationGUI : public Gtk::VBox {
	public:
		Simulation& simulation;

		Gtk::HBox use_simu_hbox;
		Gtk::CheckButton use_simu_button;
		Gtk::Label use_simu_label;

		bool enable_simulator = false;

		SimulationGUI(Simulation &simulation_ref);
		void enable_simulator_callback();
};

#endif //VSSS_SIMULATORGUI_HPP
