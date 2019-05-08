#include <V4LInterface.hpp>
#include "SimulationGUI.hpp"

SimulationGUI::SimulationGUI(std::array<Robot2 *, 3> &robots, Ball &ball, Strategy2 &strategy_ref,
							 capture::V4LInterface &interface_ref, int argc, char **argv) :
							 robots(robots), ball(ball), strategy(strategy_ref),
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
		simulation.emplace(robots, ball, strategy, interface);
		simulation->interface.robotGUI.enable_main_buttons();
	} else {
		simulation.reset();
		interface.robotGUI.enable_main_buttons(false);
	}
}
