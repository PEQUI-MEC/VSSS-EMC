/*
 * strategyGUI.hpp
 *
 *  Created on: May 8th, 2016
 *      Author: Daniel
 */

#ifndef STRATEGYGUI_HPP_
#define STRATEGYGUI_HPP_

#include <gtkmm.h>
#include "Strategy.hpp"

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
	Gtk::Frame constants_fm;
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

	Strategy strats;

protected:
	Gtk::Grid constants_grid;
	Gtk::CheckButton fieldLength_bt;
	Gtk::CheckButton totalFieldLength_bt;
	Gtk::CheckButton fieldWwidth_bt;
	Gtk::CheckButton goalSize_bt;
	Gtk::CheckButton areaSize_bt;
	Gtk::CheckButton areaLimitX_bt;
	Gtk::CheckButton banheira_bt;
	Gtk::CheckButton areasDivision_bt;
	Gtk::CheckButton banheiraOffset_bt;
	Gtk::CheckButton goalCenterX_bt;
	Gtk::CheckButton goalCenterY_bt;
	Gtk::CheckButton goalMin_bt;
	Gtk::CheckButton goalMax_bt;
	Gtk::CheckButton defenseLine_bt;
	Gtk::CheckButton offsetRatio_bt;
	Gtk::CheckButton coneRatio_bt;
	Gtk::CheckButton deslocamentoZagaAtaque_bt;

private:
	bool deslocamentoZagaAtaque_flag = false;
	bool goalSize_flag = false;
	bool fieldWwidth_flag = false;
	bool totalFieldLength_flag = false;
	bool fieldLength_flag = false;
	bool coneRatio_flag = false;
	bool offsetRatio_flag = false;
	bool defenseLine_flag = false;
	bool goalMax_flag = false;
	bool goalMin_flag = false;
	bool goalCenterY_flag = false;
	bool goalCenterX_flag = false;
	bool banheiraOffset_flag = false;
	bool areasDivision_flag = false;
	bool banheira_flag = false;
	bool areaLimitX_flag = false;
	bool areaSize_flag = false;

public:

	bool get_deslocamentoZagaAtaque_flag()
	{
		return deslocamentoZagaAtaque_flag;
	}

	bool get_goalSize_flag()
	{
		return goalSize_flag;
	}

	bool get_fieldWwidth_flag()
	{
		return fieldWwidth_flag;
	}

	bool get_totalFieldLength_flag()
	{
		return totalFieldLength_flag;
	}

	bool get_fieldLength_flag()
	{
		return fieldLength_flag;
	}

	bool get_coneRatio_flag()
	{
		return coneRatio_flag;
	}

	bool get_offsetRatio_flag()
	{
		return offsetRatio_flag;
	}

	bool get_defenseLine_flag()
	{
		return defenseLine_flag;
	}

	bool get_goalMax_flag()
	{
		return goalMax_flag;
	}

	bool get_goalMin_flag()
	{
		return goalMin_flag;
	}

	bool get_goalCenterY_flag()
	{
		return goalCenterY_flag;
	}

	bool get_goalCenterX_flag()
	{
		return goalCenterX_flag;
	}

	bool get_banheiraOffset_flag()
	{
		return banheiraOffset_flag;
	}

	bool get_areasDivision_flag()
	{
		return areasDivision_flag;
	}

	bool get_banheira_flag()
	{
		return banheira_flag;
	}

	bool get_areaSize_flag()
	{
		return areaSize_flag;
	}

	bool get_areaLimitX_flag()
	{
		return areaLimitX_flag;
	}

	StrategyGUI()
	{
		createCheckConstantsFrame();
		//createSelectionFrame();
		//createMenuFrame();
		//createInfoTextFrame();
		//createInfoImageFrame();
	}

	void createCheckConstantsFrame()
	{
		pack_start(constants_fm, false, true, 5);
		constants_fm.add(constants_grid);
		constants_fm.set_label("Draw Constants");

		fieldLength_bt.set_label("Field Length");
		totalFieldLength_bt.set_label("Total Field Length");
		fieldWwidth_bt.set_label("Field Width");
		goalSize_bt.set_label("Goal Size");
		areaSize_bt.set_label("Area Size");
		areaLimitX_bt.set_label("Area Limit X");
		banheira_bt.set_label("Banheira");
		areasDivision_bt.set_label("Areas Division");
		banheiraOffset_bt.set_label("Banheira Offset");
		goalCenterX_bt.set_label("Goal Center X");
		goalCenterY_bt.set_label("Goal Center Y");
		goalMin_bt.set_label("Goal Min.");
		goalMax_bt.set_label("Goal Max.");
		defenseLine_bt.set_label("Defense Line");
		offsetRatio_bt.set_label("Offset Ratio");
		coneRatio_bt.set_label("Cone Ratio");
		deslocamentoZagaAtaque_bt.set_label("Deslocamento Zaga Ataque");

		constants_grid.set_border_width(10);
		constants_grid.set_column_spacing(5);

		constants_grid.add(fieldLength_bt);
		constants_grid.add(totalFieldLength_bt);
		constants_grid.add(fieldWwidth_bt);

		constants_grid.attach_next_to(goalSize_bt, fieldLength_bt, Gtk::POS_BOTTOM, 1, 1);
		constants_grid.attach_next_to(areaSize_bt, totalFieldLength_bt, Gtk::POS_BOTTOM, 1, 1);
		constants_grid.attach_next_to(areaLimitX_bt, fieldWwidth_bt, Gtk::POS_BOTTOM, 1, 1);

		constants_grid.attach_next_to(banheira_bt, goalSize_bt, Gtk::POS_BOTTOM, 1, 1);
		constants_grid.attach_next_to(areasDivision_bt, areaSize_bt, Gtk::POS_BOTTOM, 1, 1);
		constants_grid.attach_next_to(banheiraOffset_bt, areaLimitX_bt, Gtk::POS_BOTTOM, 1, 1);

		constants_grid.attach_next_to(goalCenterX_bt, banheira_bt, Gtk::POS_BOTTOM, 1, 1);
		constants_grid.attach_next_to(goalCenterY_bt, areasDivision_bt, Gtk::POS_BOTTOM, 1, 1);
		constants_grid.attach_next_to(goalMin_bt, banheiraOffset_bt, Gtk::POS_BOTTOM, 1, 1);

		constants_grid.attach_next_to(goalMax_bt, goalCenterX_bt, Gtk::POS_BOTTOM, 1, 1);
		constants_grid.attach_next_to(defenseLine_bt, goalCenterY_bt, Gtk::POS_BOTTOM, 1, 1);
		constants_grid.attach_next_to(offsetRatio_bt, goalMin_bt, Gtk::POS_BOTTOM, 1, 1);

		constants_grid.attach_next_to(coneRatio_bt, goalMax_bt, Gtk::POS_BOTTOM, 1, 1);
		constants_grid.attach_next_to(deslocamentoZagaAtaque_bt, defenseLine_bt, Gtk::POS_BOTTOM, 1, 1);

		fieldLength_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_fieldLength_bt_clicked));
		totalFieldLength_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_totalFieldLength_bt_clicked));
		fieldWwidth_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_fieldWwidth_bt_clicked));
		goalSize_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_goalSize_bt_clicked));
		areaSize_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_areaSize_bt_clicked));
		areaLimitX_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_areaLimitX_bt_clicked));
		banheira_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_banheira_bt_clicked));
		areasDivision_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_areasDivision_bt_clicked));
		banheiraOffset_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_banheiraOffset_bt_clicked));
		goalCenterX_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_goalCenterX_bt_clicked));
		goalCenterY_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_goalCenterY_bt_clicked));
		goalMin_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_goalMin_bt_clicked));
		goalMax_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_goalMax_bt_clicked));
		defenseLine_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_defenseLine_bt_clicked));
		offsetRatio_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_offsetRatio_bt_clicked));
		coneRatio_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_coneRatio_bt_clicked));
		deslocamentoZagaAtaque_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_deslocamentoZagaAtaque_bt_clicked));
	}

private:
	void _event_fieldLength_bt_clicked()
	{
		fieldLength_flag = !fieldLength_flag;
	}

	void _event_totalFieldLength_bt_clicked()
	{
		totalFieldLength_flag = !totalFieldLength_flag;
	}

	void _event_fieldWwidth_bt_clicked()
	{
		fieldWwidth_flag = !fieldWwidth_flag;
	}

	void _event_goalSize_bt_clicked()
	{
		goalSize_flag = !goalSize_flag;
	}

	void _event_areaSize_bt_clicked()
	{
		areaSize_flag = !areaSize_flag;
	}

	void _event_areaLimitX_bt_clicked()
	{
		areaLimitX_flag = !areaLimitX_flag;
	}

	void _event_banheira_bt_clicked()
	{
		banheira_flag = !banheira_flag;
	}

	void _event_areasDivision_bt_clicked()
	{
		areasDivision_flag = !areasDivision_flag;
	}

	void _event_banheiraOffset_bt_clicked()
	{
		banheiraOffset_flag = !banheiraOffset_flag;
	}

	void _event_goalCenterX_bt_clicked()
	{
		goalCenterX_flag = !goalCenterX_flag;
	}

	void _event_goalCenterY_bt_clicked()
	{
		goalCenterY_flag = !goalCenterY_flag;
	}

	void _event_goalMin_bt_clicked()
	{
		goalMin_flag = !goalMin_flag;
	}

	void _event_goalMax_bt_clicked()
	{
		goalMax_flag = !goalMax_flag;
	}

	void _event_defenseLine_bt_clicked()
	{
		defenseLine_flag = !defenseLine_flag;
	}

	void _event_offsetRatio_bt_clicked()
	{
		offsetRatio_flag = !offsetRatio_flag;
	}

	void _event_coneRatio_bt_clicked()
	{
		coneRatio_flag = !coneRatio_flag;
	}

	void _event_deslocamentoZagaAtaque_bt_clicked()
	{
		deslocamentoZagaAtaque_flag = !deslocamentoZagaAtaque_flag;
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

public:
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
