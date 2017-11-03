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

class StrategyGUI: public Gtk::VBox
{
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

	char * unconstant_char(const char * c_str) {
		char * uc;
		int len;
		// tamanho da string
		for(len = 0; c_str[len] != '\0'; len++);
		// aloca nova string
		uc = (char *) malloc(len*sizeof(char));
		// copia dados
		for(int i = 0; i < len; i++) {
			uc[i] = c_str[i];
		}
		return uc;
	}
	void _event_loadFormation_bt_clicked() {
		std::ifstream txtFile;
		std::string line;
		char * tmp_substr;

		if(formation_box.get_active_row_number() == 0) {
			bt_loadFormation.set_sensitive(false);
			bt_deleteFormation.set_sensitive(false);
			return;
		}

		txtFile.open("formation.txt");

		if(txtFile.is_open()) {
			getline(txtFile, line);
			// formações salvas
			int n = atoi(line.c_str());
			// para cada prox linha
			for(int i = 0; i < n; i++) {
				// move o cursor enquanto não é a linha que eu quero
				getline(txtFile, line);
				if(i == formation_box.get_active_row_number() - 1) {
					std::vector<std::string> strs;
					boost::split(strs, line, boost::is_any_of(" "));

					// pega robot 1 x position value
					formation_positions[0].x = atof(strs.at(1).c_str());
					// pega robot 1 y position value
					formation_positions[0].y = atof(strs.at(2).c_str());
					// pega robot 1 theta position value
					formation_orientations[0] = atof(strs.at(3).c_str());

					// pega robot 2 x position value
					formation_positions[1].x = atof(strs.at(4).c_str());
					// pega robot 2 y position value
					formation_positions[1].y = atof(strs.at(5).c_str());
					// pega robot 2 theta position value
					formation_orientations[1] = atof(strs.at(6).c_str());

					// pega robot 3 x position value
					formation_positions[2].x = atof(strs.at(7).c_str());
					// pega robot 3 y position value
					formation_positions[2].y = atof(strs.at(8).c_str());
					// pega robot 3 theta position value
					formation_orientations[2] = atof(strs.at(9).c_str());
				}
			}
			// atualiza interface
			update_interface_flag = true;
			std::cout << "Begin positioning...\n";
			// valores disponíveis: formation_positions e formation_orientations
			// isso é atualizado no camcap

			// manda atualizar
			updating_formation_flag = true;

			txtFile.close();
		}
		else {
			// não existem formações salvas
			std::cout << "You removed the file before I could read it.\n";
		}
	}
	void _event_deleteFormation_bt_clicked() {
		std::ofstream tmpTxtFile;
		std::ifstream txtFile;
		std::string formationName;
		std::string line;

		txtFile.open("formation.txt");
		tmpTxtFile.open("tmp_formation.txt");

		if (txtFile.is_open() && tmpTxtFile.is_open()) {
			getline(txtFile, line);
			// formações salvas
			int n = atoi(line.c_str());
			// se só tem uma formação salva, apaga o arquivo
			if(n-1 == 0) {
				std::cout << "There was only one formation saved. Formation file deleted.\n";
				txtFile.close();
				tmpTxtFile.close();

				// desabilita os botões
				formation_flag = false;
				bt_createFormation.set_active(false);
				bt_saveFormation.set_sensitive(false);
				formation_name_entry.set_sensitive(false);
				// salva as alterações
				txtFile.close();
				tmpTxtFile.close();
				// remove os arquivos
				system("rm -rf formation.txt");
				system("rm -rf tmp_formation.txt");
				// atualiza a interface
				loadSavedFormations();
				return;
			}
			// decrementa o número de formações no novo arquivo
			tmpTxtFile << (n-1) << std::endl;
			// para cada prox linha
			for(int i = 0; i < n; i++) {
				// move o cursor enquanto não é a linha que eu quero e copia os dados para o novo arquivo
				getline(txtFile, line);
				// ignora a linha selecionada
				if(i == formation_box.get_active_row_number() - 1)
					continue;
				tmpTxtFile << line << std::endl;
			}

			// desabilita os botões
			formation_flag = false;
			bt_createFormation.set_active(false);
			bt_saveFormation.set_sensitive(false);
			formation_name_entry.set_sensitive(false);
			// salva as alterações
			txtFile.close();
			tmpTxtFile.close();
			// agora as alterações não são mais temporárias
			system("rm -rf formation.txt");
			system("mv tmp_formation.txt formation.txt");
			// atualiza a interface
			loadSavedFormations();
		}
		else {
			std::cout << "Could not delete formation.\n";
		}
	}
	void _event_saveFormation_bt_clicked() {
		std::ofstream tmpTxtFile;
		std::ifstream txtFile;
		std::string formationName;
		std::string line;

		if(!formation_flag) {
			std::cout << "Synchronicity error. Flag should be set.\n";
			return;
		}

		txtFile.open("formation.txt");
		tmpTxtFile.open("tmp_formation.txt");

		// verifica se o nome foi preenchido
		formationName = formation_name_entry.get_text();
		if(formationName == "") {
			std::cout << "Your formation needs to have a name.";
			return;
		}

		// coloca a string no formato certo para ser salva no arquivo
		for(int i = 0; formationName[i] != '\0'; i++) {
			if(!(formationName[i] >= 'a' && formationName[i] <= 'z') && !(formationName[i] >= 'A' && formationName[i] <= 'Z')) {
				formationName[i] = '_';
			}
		}

		if (txtFile.is_open() && tmpTxtFile.is_open()) {
			getline(txtFile, line);
			// formações salvas
			int n = atoi(line.c_str());
			// incrementa o número de formações no novo arquivo
			tmpTxtFile << (n+1) << std::endl;
			// para cada prox linha
			for(int i = 0; i < n; i++) {
				// move o cursor enquanto não é a linha que eu quero e copia os dados para o novo arquivo
				getline(txtFile, line);
				tmpTxtFile << line << std::endl;
			}
			// grava os novos dados no arquivo
			tmpTxtFile << formationName << " ";
			for(int i = 0; i < 3; i++) {
				// não importa deixar o " " no final pois na leitura ele é ignorado
				tmpTxtFile << formation_positions[i].x << " " << formation_positions[i].y << " " << formation_orientations[i] << " ";
			}
			tmpTxtFile << std::endl;

			// desabilita os botões
			formation_flag = false;
			bt_createFormation.set_active(false);
			bt_saveFormation.set_sensitive(false);
			formation_name_entry.set_sensitive(false);
			// salva as alterações
			txtFile.close();
			tmpTxtFile.close();
			// agora as alterações não são mais temporárias
			system("rm -rf formation.txt");
			system("mv tmp_formation.txt formation.txt");
			// atualiza a interface
			loadSavedFormations();
		}
		else if(tmpTxtFile.is_open()) { // primeira formação salva
			tmpTxtFile << 1 << std::endl;
			// grava os novos dados no arquivo
			tmpTxtFile << formationName << " ";
			for(int i = 0; i < 3; i++) {
				// não importa deixar o " " no final pois na leitura ele é ignorado
				tmpTxtFile << formation_positions[i].x << " " << formation_positions[i].y << " " << formation_orientations[i] << " ";
			}
			tmpTxtFile << std::endl;

			// desabilita os botões
			formation_flag = false;
			bt_createFormation.set_active(false);
			bt_saveFormation.set_sensitive(false);
			formation_name_entry.set_sensitive(false);
			// salva as alterações
			tmpTxtFile.close();
			// agora as alterações não são mais temporárias
			system("mv tmp_formation.txt formation.txt");
			// atualiza a interface
			loadSavedFormations();
		}
		else {
			std::cout << "Could not save formation.\n";
		}
	}
	void _event_createFormation_bt_clicked() {
		formation_flag = !formation_flag;

		bt_createFormation.set_active(!formation_flag);
		bt_saveFormation.set_sensitive(formation_flag);
		formation_name_entry.set_sensitive(formation_flag);
	}
	void _event_formation_box_changed() {
		if(formation_box.get_active_row_number() == 0) {
			bt_loadFormation.set_sensitive(false);
			bt_deleteFormation.set_sensitive(false);
			return;
		}
		bt_loadFormation.set_sensitive(true);
		bt_deleteFormation.set_sensitive(true);
	}
	/// formation.txt:
	///	Nº de formações
	/// formation_name r1x r1y r1o r2x r2y r2o r3x r3y r3o
	void loadSavedFormations() {
		std::ifstream txtFile;
		std::string line;
		char * formationName;

		txtFile.open("formation.txt");

		formation_box.remove_all();
		formation_box.append("None");

		if(txtFile.is_open()) {
			getline(txtFile, line);
			// formações salvas
			int n = atoi(line.c_str());
			// para cada prox linha
			for(int i = 0; i < n; i++) {
				getline(txtFile, line);
				// pega o nome da formação
				formationName = strtok(unconstant_char(line.c_str()), " ");
				formation_box.append(formationName);
			}

			txtFile.close();
		}
		else {
			// não existem formações salvas
			// std::cout << "There's no saved robot formations.\n";
		}

		// seleciona o None como padrão
		formation_box.set_active(0);
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

	Gtk::ScrolledWindow m_ScrolledWindow;
	Gtk::TextView strategy_description_view;
	Glib::RefPtr<Gtk::TextBuffer> strategy_description_text;

	Strategy strategy;

	bool updating_formation_flag = false;

	Gtk::Frame formation_fm;
	bool formation_flag = false;
	bool update_interface_flag = false;
	Gtk::Grid formation_grid;
	Gtk::Button bt_loadFormation;
	Gtk::Button bt_deleteFormation;
	Gtk::ToggleButton bt_createFormation;
	Gtk::Button bt_saveFormation;
	Gtk::ComboBoxText formation_box;
	Gtk::Entry formation_name_entry;
	cv::Point formation_positions[3]; // é atualizado só se a flag estiver ativada
	float formation_orientations[3]; // é atualizado só se a flag estiver ativada

	StrategyGUI()
	{
		createFormationFrame();
		pack_start(strategy.testFrame, false, true, 5);
		configureTestFrame();
		//createMenuFrame();
		//createInfoTextFrame();
		//createInfoImageFrame();
	}

	void configureTestFrame() {
		std::string labels[5] = {"Goalie Line", "Name 2", "Name 3", "Name 4", "Name 5"};
		double min[5] = {0, 0, 0, 0, 0};
		double max[5] = {160, 100, 100, 100, 100};
		double currentValue[5] = {0, 10, 20, 30, 40};
		double digits[5] = {0, 0, 0, 0, 0};
		double steps[5] = {5, 1, 1, 1, 1};

		for (int i = 0; i < 5; i++) {
			strategy.testFrame.setLabel(i, labels[i]);
			strategy.testFrame.configureHScale(i, currentValue[i], min[i], max[i], digits[i], steps[i]);
		}
	}

	void createFormationFrame() {
		pack_start(formation_fm, false, true, 5);
		formation_fm.add(formation_grid);
		formation_fm.set_label("Formation");
		formation_grid.set_halign(Gtk::ALIGN_CENTER);

		bt_createFormation.set_label("Create Formation");
		loadSavedFormations();
		bt_loadFormation.set_label("Load");
		bt_deleteFormation.set_label("Delete");
		formation_name_entry.set_max_length(30);
		formation_name_entry.set_width_chars(30);
		formation_name_entry.set_text(Glib::ustring::format("Formation_name"));
		bt_saveFormation.set_label("Save");

		formation_grid.set_border_width(10);
		formation_grid.set_column_spacing(5);

		formation_grid.attach(bt_createFormation, 0, 0, 1, 1);
		formation_grid.attach(formation_box, 0, 1, 1, 1);
		formation_grid.attach(bt_loadFormation, 1, 1, 1, 1);
		formation_grid.attach(bt_deleteFormation, 2, 1, 1, 1);
		formation_grid.attach(formation_name_entry, 0, 2, 1, 1);
		formation_grid.attach(bt_saveFormation, 1, 2, 1, 1);


		formation_box.set_sensitive(false); // habilita quando der start
		bt_createFormation.set_sensitive(false); // habilita quando der start

		bt_loadFormation.set_sensitive(false); // habilita só quando selecionar uma formação
		bt_deleteFormation.set_sensitive(false); // habilita só quando selecionar uma formação
		bt_saveFormation.set_sensitive(false); // habilita só quando o createFormation está ativado
		formation_name_entry.set_sensitive(false); // habilita só quando o createFormation está ativado

		formation_box.signal_changed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_formation_box_changed));
		bt_createFormation.signal_pressed().connect(sigc::mem_fun(*this, &StrategyGUI::_event_createFormation_bt_clicked));
		bt_saveFormation.signal_clicked().connect(sigc::mem_fun(*this, &StrategyGUI::_event_saveFormation_bt_clicked));
		bt_loadFormation.signal_clicked().connect(sigc::mem_fun(*this, &StrategyGUI::_event_loadFormation_bt_clicked));
		bt_deleteFormation.signal_clicked().connect(sigc::mem_fun(*this, &StrategyGUI::_event_deleteFormation_bt_clicked));
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
