#include <V4LInterface.hpp>
#include "SimulationGUI.hpp"

SimulationGUI::SimulationGUI(capture::V4LInterface &interface_ref, Teams& curr_team, int argc, char **argv) :
							 interface(interface_ref), current_team(curr_team), argc(argc), argv(argv) {

  simu_frame.set_label("Simulator Settings");
  use_simu_button.set_label("Use simulator");
  team_title_label.set_label(" Select Team to Config.:");
  l_team_button.set_label("Left Team");
  r_team_button.set_label("Right Team");

  simu_frame.add(simu_grid);
  simu_frame.set_border_width(10);

  simu_grid.set_row_spacing(10);
  simu_grid.set_column_spacing(6);
  simu_grid.set_border_width(10);
  simu_grid.attach(use_simu_button, 0, 0, 1, 1);
  simu_grid.attach(team_title_label, 0, 2, 1, 1);
  simu_grid.attach(l_team_button, 1, 2, 1, 1);
  simu_grid.attach(r_team_button, 2, 2, 1, 1);

  r_team_button.join_group(l_team_button);

  l_team_button.set_state(Gtk::STATE_INSENSITIVE);
  r_team_button.set_state(Gtk::STATE_INSENSITIVE);

	pack_start(simu_frame, false, true, 5);

  use_simu_button.signal_toggled().connect(sigc::mem_fun(*this,
					&SimulationGUI::enable_simulator_callback));
  l_team_button.signal_toggled().connect(sigc::mem_fun(*this,
					&SimulationGUI::select_team_callback));
}

void SimulationGUI::enable_simulator_callback() {
	if (use_simu_button.get_active()) {
		if (!ros::isInitialized()) {
			ros::init(argc, argv, "vsss");
		}
    select_team_callback();
    l_team_button.set_state(Gtk::STATE_NORMAL);
    r_team_button.set_state(Gtk::STATE_NORMAL);
		simulation.emplace("1", "2", "3", false, interface);
		simulation_team2.emplace("4", "5", "6", true, interface);
		simulation->interface.robotGUI.enable_main_buttons();
	} else {
    l_team_button.set_state(Gtk::STATE_INSENSITIVE);
    r_team_button.set_state(Gtk::STATE_INSENSITIVE);

		simulation.reset();
		simulation_team2.reset();
		interface.robotGUI.enable_main_buttons(false);
    
    change_team(Teams::Real);
	}
}

void SimulationGUI::select_team_callback() {
  if (l_team_button.get_active())
    change_team(Teams::LeftSimu);
  else
    change_team(Teams::RightSimu);
}

void SimulationGUI::change_team(Teams new_team) {
  current_team = new_team;
  interface.robotGUI.update_robots();
}
