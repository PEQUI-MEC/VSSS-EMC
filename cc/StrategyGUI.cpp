#include "StrategyGUI.hpp"

StrategyGUI::StrategyGUI() {
	configure_options_frame();
	pack_start(options_frame, false, true);
	configureTestFrame();
	pack_start(testFrame, false, true, 5);
	transitions_button.clicked();
	ai_button.clicked();
}

void StrategyGUI::configureTestFrame() {
	std::string labels[5] = {"Goalie Line", "Goalie Offset", "Name 3", "Name 4", "Name 5"};
	double min[5] = {0, 0, 0, 0, 0};
	double max[5] = {160, 100, 100, 100, 100};
	double currentValue[5] = {60, 40, 20, 30, 40};
	double digits[5] = {0, 0, 0, 0, 0};
	double steps[5] = {5, 1, 1, 1, 1};

	for (int i = 0; i < 5; i++) {
		testFrame.setLabel(i, labels[i]);
		testFrame.configureHScale(i, currentValue[i], min[i], max[i], digits[i], steps[i]);
	}
}

void StrategyGUI::configure_options_frame() {
	ai_label.set_label("Use AI");
	ai_hbox.pack_start(ai_button, false, false, 5);
	ai_button.signal_clicked().connect([&](){
		use_ai = ai_button.get_active();
	});
	ai_hbox.pack_start(ai_label, false, false, 0);

	transitions_label.set_label("Transitions (full, half e mindcontrol)");
	transitions_hbox.pack_start(transitions_button, false, false, 5);
	transitions_button.signal_clicked().connect(sigc::mem_fun(*this, &StrategyGUI::set_transitions));
	transitions_hbox.pack_start(transitions_label, false, false, 0);

	options_vbox.pack_start(ai_hbox, false, false, 5);
	options_vbox.pack_start(transitions_hbox, false, false, 5);
	options_frame.set_label("Options");
	options_frame.add(options_vbox);
}

void StrategyGUI::set_transitions() {
//	bool is_active = transitions_button.get_active();
//	strategy.enable_transtions = is_active;
//	strategy.set_transition_mindcontrol_enabled(is_active);
//
////	Desativa full_transtion_enabled e half_transition_enabled caso estejam setadas
//	if(strategy.full_transition_enabled) strategy.set_full_transition_enabled(false);
//	if(strategy.half_transition_enabled) strategy.set_half_transition_enabled(false);
}
