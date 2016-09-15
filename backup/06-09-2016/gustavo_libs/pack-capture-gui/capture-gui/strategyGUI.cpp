/*
 * strategy.hpp
 *
 *  Created on: May 8th, 2016
 *      Author: Daniel
 */

#ifndef STRATEGYGUI_HPP_
#define STRATEGYGUI_HPP_

#include <gtkmm.h>

class StrategyGUI: public Gtk::HBox
{
public:
	//Tree model columns:
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

	ModelColumns()
	{ add(strategy_name); }

	Gtk::TreeModelColumn<Glib::ustring> strategy_name;
	};


	Gtk::Box left_box;
	Gtk::Box right_box;
	Gtk::Button select_button;
	Gtk::Label strategy_title_label;

	StrategyGUI()
	{
		strategy_title_label.set_text("Estrategia Selecionada");

		right_box.pack_start(strategy_title_label, false, false, 10);
	}

	~StrategyGUI()
	{}

	





};


#endif /* STRATEGYGUI_HPP_ */