#include "StrategyGUI.hpp"

StrategyGUI::StrategyGUI() {
	pack_start(strategy.testFrame, false, true, 5);
	configureTestFrame();
}

void StrategyGUI::configureTestFrame() {
	std::string labels[5] = {"Goalie Line", "Goalie Offset", "Name 3", "Name 4", "Name 5"};
	double min[5] = {0, 0, 0, 0, 0};
	double max[5] = {160, 100, 100, 100, 100};
	double currentValue[5] = {60, 40, 20, 30, 40};
	double digits[5] = {0, 0, 0, 0, 0};
	double steps[5] = {5, 1, 1, 1, 1};

	for (int i = 0; i < 5; i++) {
		strategy.testFrame.setLabel(i, labels[i]);
		strategy.testFrame.configureHScale(i, currentValue[i], min[i], max[i], digits[i], steps[i]);
	}
}
