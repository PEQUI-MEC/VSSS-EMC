/*
 * strategyGUI.hpp
 *
 *  Created on: May 8th, 2016
 *      Author: Daniel
 */

#ifndef STRATEGYGUI_HPP_
#define STRATEGYGUI_HPP_

#include <gtkmm.h>
#include <boost/algorithm/string.hpp>
#include "Strategy.hpp"
#include "filechooser.hpp"

class StrategyGUI : public Gtk::VBox {
	public:
		Strategy strategy;

		void configureTestFrame();
		StrategyGUI();
		~StrategyGUI() override = default;;
};

#endif /* STRATEGYGUI_HPP_ */
