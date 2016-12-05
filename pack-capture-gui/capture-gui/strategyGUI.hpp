/*
 * strategyGUI.hpp
 *
 *  Created on: May 8th, 2016
 *      Author: Daniel
 */

#ifndef STRATEGYGUI_HPP_
#define STRATEGYGUI_HPP_

#include <gtkmm.h>

class StrategyGUI: public Gtk::VBox
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


	Gtk::Frame menu_fm;
	Gtk::Frame selection_fm;
	Gtk::Frame info_text_fm;
	Gtk::Frame info_img_fm;
	Gtk::TreeView m_TreeView;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

	Gtk::HBox selection_hbox;

	Gtk::VBox menu_vbox;
	Gtk::HBox menu_hbox[2];

	Gtk::VBox info_text_hbox;

	Gtk::Image strategy_img;

	Gtk::Button select_button;
	Gtk::Label strategy_title_label;

	Gtk::ScrolledWindow m_ScrolledWindow;
	Gtk::TextView strategy_description_view;
	Glib::RefPtr<Gtk::TextBuffer> strategy_description_text;

	StrategyGUI()
	{
		createSelectionFrame();
		createMenuFrame();
		createInfoTextFrame();
		createInfoImageFrame();
	}

	void createSelectionFrame()
	{
		pack_start(selection_fm, false, true, 5);
		selection_fm.set_label("Strategy Selected:  ");
		selection_fm.add(selection_hbox);

		strategy_title_label.set_text("*Nome da Estrategia*");
		selection_hbox.set_halign(Gtk::ALIGN_CENTER);
		selection_hbox.pack_start(strategy_title_label, false, true, 5);
	}

	void createMenuFrame()
	{
		pack_start(menu_fm, false, true, 5);
		menu_fm.add(menu_vbox);



  		menu_hbox[0].pack_start(m_TreeView, false, true, 5);
  		menu_hbox[0].set_halign(Gtk::ALIGN_CENTER);
  		m_TreeView.set_size_request(450,200);
  		//Add the TreeView's view columns:
 		//This number will be shown with the default numeric formatting.
		m_TreeView.append_column("Strategy Menu", strategyColumns.strategy_name);
  		//Create the Tree model:
 		m_refTreeModel = Gtk::ListStore::create(strategyColumns);
		m_TreeView.set_model(m_refTreeModel);
		//Fill the TreeView's model
  		Gtk::TreeModel::Row row = *(m_refTreeModel->append());
  		row[strategyColumns.strategy_name] = "Strategy 1";
  		row = *(m_refTreeModel->append());
  		row[strategyColumns.strategy_name] = "Strategy 2";

  		select_button.set_label("Selecionar");
  		menu_hbox[1].pack_start(select_button, false, true, 5);
  		menu_hbox[1].set_halign(Gtk::ALIGN_CENTER);
  		menu_vbox.pack_start(menu_hbox[0], false, true,  5);
  		menu_vbox.pack_start(menu_hbox[1], false, true,  5);
	}

	void createInfoTextFrame()
	{
		pack_start(info_text_fm, false, true, 5);
  		info_text_fm.set_label("Description:  ");
  		info_text_fm.add(m_ScrolledWindow);

  		m_ScrolledWindow.add(strategy_description_view);
  		m_ScrolledWindow.set_min_content_height(200);
  		m_ScrolledWindow.set_min_content_width(450);
		strategy_description_text = Gtk::TextBuffer::create();
  		strategy_description_text->set_text("This is the text from TextBuffer #1.\n\n\n\n");
  		strategy_description_view.set_buffer(strategy_description_text);
	}

	void createInfoImageFrame()
	{
		pack_start(info_img_fm, false, true, 5);
  		info_img_fm.set_label("Picture:  ");
  		strategy_img.set("img/pequi_mecanico.png");
  		info_img_fm.add(strategy_img);
	}

	~StrategyGUI()
	{}







};


#endif /* STRATEGYGUI_HPP_ */
