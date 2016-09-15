/*
 * strategyGUI.hpp
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

	ModelColumns strategyColumns;


	Gtk::ScrolledWindow m_ScrolledWindow2;
	Gtk::TreeView m_TreeView;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModel;


	Gtk::VBox left_frame;
	Gtk::VBox right_frame;
	Gtk::Button select_button;
	Gtk::Label strategy_title_label;

	Gtk::ScrolledWindow m_ScrolledWindow;
	Gtk::TextView strategy_description_view;
	Glib::RefPtr<Gtk::TextBuffer> strategy_description_text;

	StrategyGUI()
	{
		strategy_title_label.set_text("Nome da Estrategia");

		//strategy_description_view.set_editable(false);

		//Add the TreeView, inside a ScrolledWindow, with the button underneath:
  		m_ScrolledWindow2.add(m_TreeView);

  		//Only show the scrollbars when they are necessary:
  		m_ScrolledWindow2.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  		//Add the TreeView's view columns:
 		//This number will be shown with the default numeric formatting.
		m_TreeView.append_column("Estrategia", strategyColumns.strategy_name);

  		//Create the Tree model:
 		m_refTreeModel = Gtk::ListStore::create(strategyColumns);
		m_TreeView.set_model(m_refTreeModel);

		//Fill the TreeView's model
  		Gtk::TreeModel::Row row = *(m_refTreeModel->append());
  		row[strategyColumns.strategy_name] = "Estrategia 1";

  		row = *(m_refTreeModel->append());
  		row[strategyColumns.strategy_name] = "Estrategia 2";

  		m_ScrolledWindow2.set_min_content_height(200);
  		m_ScrolledWindow2.set_min_content_width(200);

		strategy_description_text = Gtk::TextBuffer::create();
  		strategy_description_text->set_text("This is the text from TextBuffer #1.\n\n\n\na");
  		strategy_description_view.set_buffer(strategy_description_text);

  		m_ScrolledWindow.add(strategy_description_view);
  		m_ScrolledWindow.set_min_content_height(200);

  		select_button.set_label("Selecionar");

  		//left_frame.add(select_button);
  		left_frame.pack_start(m_ScrolledWindow2, Gtk::PACK_EXPAND_WIDGET, 10);
  		left_frame.pack_start(select_button, Gtk::PACK_SHRINK, 0);
		right_frame.pack_start(strategy_title_label, Gtk::PACK_SHRINK, 0);
		right_frame.pack_start(m_ScrolledWindow, Gtk::PACK_EXPAND_WIDGET, 10);

		

		pack_start(left_frame, false, false, 0);
		pack_start(right_frame, true, true, 20);






	}

	~StrategyGUI()
	{}

	





};


#endif /* STRATEGYGUI_HPP_ */