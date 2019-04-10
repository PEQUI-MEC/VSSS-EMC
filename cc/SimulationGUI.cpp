#include "SimulationGUI.hpp"

SimulationGUI::SimulationGUI(Simulation& simulation_ref):
								simulation(simulation_ref) {
	use_simu_label.set_label("Use simulator");
	use_simu_button.signal_clicked().connect(sigc::mem_fun(*this,
					&SimulationGUI::enable_simulator_callback));
	use_simu_hbox.pack_start(use_simu_button, false, false, 5);
	use_simu_hbox.pack_start(use_simu_label, false, false);
	pack_start(use_simu_hbox, false, true, 5);
}

void SimulationGUI::enable_simulator_callback() {
	if (use_simu_button.get_active()) {
		simulation.start_ros_thread();
	} else {
		simulation.stop_ros_thread();
	}
}
