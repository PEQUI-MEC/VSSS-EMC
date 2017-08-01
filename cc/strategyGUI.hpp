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
#include "filechooser.hpp"

class StrategyGUI: public Gtk::VBox
{
protected:
	Gtk::Grid constants_grid;
	Gtk::CheckButton areaLimitX_bt;
	Gtk::CheckButton banheira_bt;
	Gtk::CheckButton areasDivision_bt;
	Gtk::CheckButton goalCenter_bt;
	Gtk::CheckButton goalMin_bt;
	Gtk::CheckButton goalMax_bt;
	Gtk::CheckButton defenseLine_bt;
	Gtk::CheckButton deslocamentoZagaAtaque_bt;
	Gtk::CheckButton ballRadius_bt;
	Gtk::CheckButton goalArea_bt;
	Gtk::CheckButton sideRectangles_bt;

	Gtk::Grid loadAnn_grid;
	Gtk::CheckButton useAnn_bt;
	Gtk::Button bt_loadAnn;

private:
	bool deslocamentoZagaAtaque_flag = false;
	bool goalSize_flag = false;
	bool defenseLine_flag = false;
	bool goalMax_flag = false;
	bool goalMin_flag = false;
	bool goalCenter_flag = false;
	bool areasDivision_flag = false;
	bool banheira_flag = false;
	bool areaLimitX_flag = false;
	bool ballRadius_flag = false;
	bool goalArea_flag = false;
	bool sideRectangles_flag = false;

	std::string annName = "ANN not loaded";



	void _event_sideRectangles_bt_clicked()
	{
		sideRectangles_flag = !sideRectangles_flag;
	}

	void _event_ballRadius_bt_clicked()
	{
		ballRadius_flag = !ballRadius_flag;
	}

	void _event_goalArea_bt_clicked()
	{
		goalArea_flag = !goalArea_flag;
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

	void _event_goalCenter_bt_clicked()
	{
		goalCenter_flag = !goalCenter_flag;
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

	void _event_deslocamentoZagaAtaque_bt_clicked()
	{
		deslocamentoZagaAtaque_flag = !deslocamentoZagaAtaque_flag;
	}

	void _event_useAnn_bt_clicked() {
		if(strategy.useAnn_flag) {
			strategy.useAnn_flag = false;
			strategy_title_label.set_text("Default Strategy");
		}
		else {
			strategy.useAnn_flag = true;
			strategy_title_label.set_text(annName);
		}
	}

	void _event_loadAnn_bt_clicked() {
		// abre o popup pra pegar o nome do arquivo
		FileChooser selectAnnWindow;

		if (selectAnnWindow.result == Gtk::RESPONSE_OK) {
			// cria ann passando o arquivo e seta na classe
		  if(!strategy.set_ann(selectAnnWindow.filename.c_str()))
			{
				std::cout << "Error: could not load ANN." << std::endl;
			}
			else
			{
				// se ocorreu tudo com sucesso, da um cout e habilita botão de usar ann
				annName = selectAnnWindow.filename;
				std::cout << "ANN \"" << annName << "\" successfully loaded!" << std::endl;
				useAnn_bt.set_sensitive(true);
			}
		}
		else
		    return;
	}

public:
	//Tree model columns:
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
		public:

		ModelColumns() {
			add(strategy_name);
		}

		Gtk::TreeModelColumn<Glib::ustring> strategy_name;
	};

	ModelColumns strategyColumns;


	Gtk::Frame menu_fm;
	Gtk::Frame selection_fm;
	Gtk::Frame info_text_fm;
	Gtk::Frame info_img_fm;
	Gtk::Frame constants_fm;
	Gtk::Frame selectAnn_fm;
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

	Strategy strategy;

	bool get_ballRadius_flag()
	{
		return ballRadius_flag;
	}

	bool get_goalArea_flag()
	{
		return goalArea_flag;
	}

	bool get_sideRectangles_flag()
	{
		return sideRectangles_flag;
	}

	bool get_deslocamentoZagaAtaque_flag()
	{
		return deslocamentoZagaAtaque_flag;
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

	bool get_goalCenter_flag()
	{
		return goalCenter_flag;
	}

	bool get_areasDivision_flag()
	{
		return areasDivision_flag;
	}

	bool get_banheira_flag()
	{
		return banheira_flag;
	}


	bool get_areaLimitX_flag()
	{
		return areaLimitX_flag;
	}

	void set_AnnName(std::string newAnnName) {
		annName = newAnnName;
	}
	std::string get_AnnName() {
		return annName;
	}

	StrategyGUI()
	{
		createCheckConstantsFrame();
		createSelectAnnFrame();
		createSelectionFrame();
		//createMenuFrame();
		//createInfoTextFrame();
		//createInfoImageFrame();
	}

	void createCheckConstantsFrame()
	{
		pack_start(constants_fm, false, true, 5);
		constants_fm.add(constants_grid);
		constants_fm.set_label("Draw Constants");

		areaLimitX_bt.set_label("Area Limit X");
		banheira_bt.set_label("Banheira");
		areasDivision_bt.set_label("Areas Division");
		goalCenter_bt.set_label("Goal Center");
		goalMin_bt.set_label("Goal Min.");
		goalMax_bt.set_label("Goal Max.");
		defenseLine_bt.set_label("Defense Line");
		deslocamentoZagaAtaque_bt.set_label("Deslocamento Zaga Ataque");
		ballRadius_bt.set_label("Ball Radius");
		goalArea_bt.set_label("Goal Area");
		sideRectangles_bt.set_label("Side Rectangles");

		constants_grid.set_border_width(10);
		constants_grid.set_column_spacing(5);

		constants_grid.attach(ballRadius_bt, 0, 0, 1, 1);
		constants_grid.attach(goalArea_bt, 1, 0, 1, 1);

		constants_grid.attach(sideRectangles_bt, 0, 1, 1, 1);
		constants_grid.attach(areaLimitX_bt, 1, 1, 1, 1);

		constants_grid.attach(banheira_bt, 0, 2, 1, 1);
		constants_grid.attach(areasDivision_bt, 1, 2, 1, 1);

		constants_grid.attach(goalCenter_bt, 0, 3, 1, 1);
		constants_grid.attach(goalMin_bt, 1, 3, 1, 1);

		constants_grid.attach(goalMax_bt, 0, 4, 1, 1);
		constants_grid.attach(defenseLine_bt, 1, 4, 1, 1);

		constants_grid.attach(deslocamentoZagaAtaque_bt, 0, 5, 2, 1);

		ballRadius_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_ballRadius_bt_clicked));
		goalArea_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_goalArea_bt_clicked));
		sideRectangles_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_sideRectangles_bt_clicked));
		areaLimitX_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_areaLimitX_bt_clicked));
		banheira_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_banheira_bt_clicked));
		areasDivision_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_areasDivision_bt_clicked));
		goalCenter_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_goalCenter_bt_clicked));
		goalMin_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_goalMin_bt_clicked));
		goalMax_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_goalMax_bt_clicked));
		defenseLine_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_defenseLine_bt_clicked));
		deslocamentoZagaAtaque_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_deslocamentoZagaAtaque_bt_clicked));
	}

	void createSelectAnnFrame()
	{
		pack_start(selectAnn_fm, false, true, 5);
		selectAnn_fm.add(loadAnn_grid);
		selectAnn_fm.set_label("Select ANN");
		loadAnn_grid.set_halign(Gtk::ALIGN_CENTER);

		bt_loadAnn.set_label("Load ANN");
		useAnn_bt.set_label("Use loaded ANN");

		loadAnn_grid.set_border_width(10);
		loadAnn_grid.set_column_spacing(5);

		loadAnn_grid.attach(bt_loadAnn, 0, 0, 1, 1);
		loadAnn_grid.attach(useAnn_bt, 1, 0, 1, 1);

		useAnn_bt.set_sensitive(false);

		bt_loadAnn.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_loadAnn_bt_clicked));
		useAnn_bt.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_useAnn_bt_clicked));
	}

	void createSelectionFrame()
	{
		pack_start(selection_fm, false, true, 5);
		selection_fm.set_label("Strategy Selected:  ");
		selection_fm.add(selection_hbox);
		strategy_title_label.set_text("Default Strategy");
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

	~StrategyGUI() {
	}
};


#endif /* STRATEGYGUI_HPP_ */
