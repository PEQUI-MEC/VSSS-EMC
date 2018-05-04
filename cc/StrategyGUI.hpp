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

		Gtk::VBox options_vbox;
		Gtk::Frame options_frame;

		Gtk::HBox transitions_hbox;
		Gtk::CheckButton transitions_button;
		Gtk::Label transitions_label;

		void configure_options_frame();
		void set_transitions();

		void configureTestFrame();
		StrategyGUI();
		~StrategyGUI() override = default;;
};

#endif /* STRATEGYGUI_HPP_ */
