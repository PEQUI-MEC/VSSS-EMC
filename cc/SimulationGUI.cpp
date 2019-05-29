#include <V4LInterface.hpp>
#include "SimulationGUI.hpp"

SimulationGUI::SimulationGUI(capture::V4LInterface &interface_ref, int argc, char **argv) :
							 interface(interface_ref), argc(argc), argv(argv) {

	use_simu_label.set_label("Use simulator");
	use_simu_button.signal_clicked().connect(sigc::mem_fun(*this,
					&SimulationGUI::enable_simulator_callback));
	use_simu_hbox.pack_start(use_simu_button, false, false, 5);
	use_simu_hbox.pack_start(use_simu_label, false, false);
	pack_start(use_simu_hbox, false, true, 5);
}

void SimulationGUI::enable_simulator_callback() {
	if (use_simu_button.get_active()) {
		if (!ros::isInitialized()) {
			ros::init(argc, argv, "vsss");
		}
		simulation.emplace("1", "2", "3", false, interface);
		simulation_team2.emplace("4", "5", "6", true, interface);
		simulation->interface.robotGUI.enable_main_buttons();
	} else {
		simulation.reset();
		simulation_team2.reset();
		interface.robotGUI.enable_main_buttons(false);
	}
}
