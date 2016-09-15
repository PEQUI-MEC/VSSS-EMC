/*
 * controlGUI.hpp
 *
 *  Created on: May 8th, 2016
 *      Author: Daniel
 * 
 * Este código cria a interface da aba 'Control' do código do VS.
 * Para liberar os widgets, o botão PID deve estar pressionado.
 * Nas caixas de texto, pode-se usar '.' ou ',' para tratar números
 * do tipo double.
 * 
 * Conta com uma Hscale e uma Entry (o TextBox do Gtkmm) para que possa
 * colocar o valor do PID de cada jogador.
 */

#ifndef CONTROLGUI_HPP_
#define CONTROLGUI_HPP_

#include <gtkmm.h>
#include <fstream>
#include <string>
#include "opencv2/opencv.hpp"

class ControlGUI: public Gtk::VBox
{
public:
	// Flag para saber se o botão PID está pressionado ou não.
	bool PID_bt_event_flag = false;
	
	// Containers para o conteúdo da interface gráfica
	Gtk::Frame PID_fm;
	Gtk::VBox PID_vbox;
	Gtk::HBox PID_hbox[10]; 
	Gtk::Label *label;
	
	// Botões PID, save e load
	Gtk::ToggleButton PID_bt;
	Gtk::Button save_bt, load_bt;
	
	// Botões para alterar valores digitados na TextBox (Entry)
	Gtk::Button buttonP_goleiro, buttonI_goleiro, buttonD_goleiro;
	Gtk::Button buttonP_lenhador, buttonI_lenhador, buttonD_lenhador;
	Gtk::Button buttonP_ojuara, buttonI_ojuara, buttonD_ojuara;
	
	// Variáveis de controle do goleiro
	Gtk::Entry boxP_goleiro, boxI_goleiro, boxD_goleiro;
	Gtk::HScale barP_goleiro, barI_goleiro, barD_goleiro;
	
	// Variáveis de controle do lenhador
	Gtk::Entry boxP_lenhador, boxI_lenhador, boxD_lenhador;
	Gtk::HScale barP_lenhador, barI_lenhador, barD_lenhador;
	
	// Variáveis de controle do ojuara
	Gtk::Entry boxP_ojuara, boxI_ojuara, boxD_ojuara;
	Gtk::HScale barP_ojuara, barI_ojuara, barD_ojuara;

	ControlGUI()
	{
		// Adicionar o frame do PID e sua VBOX
		pack_start(PID_fm, false, true, 5);
		PID_fm.set_label("PID");
		PID_fm.add(PID_vbox);
		
		// Primeira Hbox com o botão PID Config, Save e Load
		PID_bt.set_label("PID Config");
		//PID_bt.set_margin_right(200);
		save_bt.set_label("Save");
		load_bt.set_label("Load");
		PID_hbox[0].set_halign(Gtk::ALIGN_CENTER);
		PID_hbox[0].pack_start(PID_bt, false, true, 5);
		PID_hbox[0].pack_start(save_bt, false, true, 5);
		PID_hbox[0].pack_start(load_bt, false, true, 5);
		PID_vbox.pack_start(PID_hbox[0], false, true, 5);
		
		// Label "Goleiro:"
		label = new Gtk::Label("Goleiro:");
		PID_vbox.pack_start(*label, false, true, 5);
		
		// Hbox com o P do goleiro (label, Hscale [bar], Entry e button)
		label = new Gtk::Label("P");
		barP_goleiro.set_digits(3);
		barP_goleiro.set_increments(0.001,1);
		barP_goleiro.set_range(0,100);
		barP_goleiro.set_size_request(300,-1);
		boxP_goleiro.set_text(Glib::ustring::format(barP_goleiro.get_value()));
		boxP_goleiro.set_max_length(6);
		boxP_goleiro.set_width_chars(6);
		buttonP_goleiro.set_label("GO!");
		PID_hbox[1].set_halign(Gtk::ALIGN_CENTER);
		PID_vbox.pack_start(PID_hbox[1], false, true, 5);
		PID_hbox[1].pack_start(*label, false, true, 5);
		PID_hbox[1].pack_start(barP_goleiro, false, true, 5);
		PID_hbox[1].pack_start(boxP_goleiro, false, true, 5);
		PID_hbox[1].pack_start(buttonP_goleiro, false, true, 5);
		
		// Hbox com o I do goleiro (label, Hscale [bar], Entry e button)
		label = new Gtk::Label("I");
		barI_goleiro.set_digits(3);
		barI_goleiro.set_increments(0.01,1);
		barI_goleiro.set_range(0,100);
		barI_goleiro.set_size_request(300,-1);
		boxI_goleiro.set_text(Glib::ustring::format(barI_goleiro.get_value()));
		boxI_goleiro.set_max_length(6);
		boxI_goleiro.set_width_chars(6);
		buttonI_goleiro.set_label("GO!");
		PID_hbox[2].set_halign(Gtk::ALIGN_CENTER);
		PID_vbox.pack_start(PID_hbox[2], false, true, 5);
		PID_hbox[2].pack_start(*label, false, true, 5);
		PID_hbox[2].pack_start(barI_goleiro, false, true, 5);
		PID_hbox[2].pack_start(boxI_goleiro, false, true, 5);
		PID_hbox[2].pack_start(buttonI_goleiro, false, true, 5);
		
		// Hbox com o D do goleiro (label, Hscale [bar], Entry e button)
		label = new Gtk::Label("D");
		barD_goleiro.set_digits(3);
		barD_goleiro.set_increments(0.001,1);
		barD_goleiro.set_range(0,100);
		barD_goleiro.set_size_request(300,-1);
		boxD_goleiro.set_text(Glib::ustring::format(barD_goleiro.get_value()));
		boxD_goleiro.set_max_length(6);
		boxD_goleiro.set_width_chars(6);
		buttonD_goleiro.set_label("GO!");
		PID_hbox[3].set_halign(Gtk::ALIGN_CENTER);
		PID_vbox.pack_start(PID_hbox[3], false, true, 5);
		PID_hbox[3].pack_start(*label, false, true, 5);
		PID_hbox[3].pack_start(barD_goleiro, false, true, 5);
		PID_hbox[3].pack_start(boxD_goleiro, false, true, 5);
		PID_hbox[3].pack_start(buttonD_goleiro, false, true, 5);
		
		// Label "Lenhador:"
		label = new Gtk::Label("Lenhador:");
		PID_vbox.pack_start(*label, false, true, 5);
		
		// Hbox com o P do lenhador (label, Hscale [bar], Entry e button)
		label = new Gtk::Label("P");
		barP_lenhador.set_digits(3);
		barP_lenhador.set_increments(0.001,1);
		barP_lenhador.set_range(0,100);
		barP_lenhador.set_size_request(300,-1);
		boxP_lenhador.set_text(Glib::ustring::format(barP_lenhador.get_value()));
		boxP_lenhador.set_max_length(6);
		boxP_lenhador.set_width_chars(6);
		buttonP_lenhador.set_label("GO!");
		PID_hbox[4].set_halign(Gtk::ALIGN_CENTER);
		PID_vbox.pack_start(PID_hbox[4], false, true, 5);
		PID_hbox[4].pack_start(*label, false, true, 5);
		PID_hbox[4].pack_start(barP_lenhador, false, true, 5);
		PID_hbox[4].pack_start(boxP_lenhador, false, true, 5);
		PID_hbox[4].pack_start(buttonP_lenhador, false, true, 5);
		
		// Hbox com o I do lenhador (label, Hscale [bar], Entry e button)
		label = new Gtk::Label("I");
		barI_lenhador.set_digits(3);
		barI_lenhador.set_increments(0.001,1);
		barI_lenhador.set_range(0,100);
		barI_lenhador.set_size_request(300,-1);
		boxI_lenhador.set_text(Glib::ustring::format(barI_lenhador.get_value()));
		boxI_lenhador.set_max_length(6);
		boxI_lenhador.set_width_chars(6);
		buttonI_lenhador.set_label("GO!");
		PID_hbox[5].set_halign(Gtk::ALIGN_CENTER);
		PID_vbox.pack_start(PID_hbox[5], false, true, 5);
		PID_hbox[5].pack_start(*label, false, true, 5);
		PID_hbox[5].pack_start(barI_lenhador, false, true, 5);
		PID_hbox[5].pack_start(boxI_lenhador, false, true, 5);
		PID_hbox[5].pack_start(buttonI_lenhador, false, true, 5);
		
		// Hbox com o D do lenhador (label, Hscale [bar], Entry e button)
		label = new Gtk::Label("D");
		barD_lenhador.set_digits(3);
		barD_lenhador.set_increments(0.001,1);
		barD_lenhador.set_range(0,100);
		barD_lenhador.set_size_request(300,-1);
		boxD_lenhador.set_text(Glib::ustring::format(barD_lenhador.get_value()));
		boxD_lenhador.set_max_length(6);
		boxD_lenhador.set_width_chars(6);
		buttonD_lenhador.set_label("GO!");
		PID_hbox[6].set_halign(Gtk::ALIGN_CENTER);
		PID_vbox.pack_start(PID_hbox[6], false, true, 5);
		PID_hbox[6].pack_start(*label, false, true, 5);
		PID_hbox[6].pack_start(barD_lenhador, false, true, 5);
		PID_hbox[6].pack_start(boxD_lenhador, false, true, 5);
		PID_hbox[6].pack_start(buttonD_lenhador, false, true, 5);
		
		// Label "Ojuara:"
		label = new Gtk::Label("Ojuara:");
		PID_vbox.pack_start(*label, false, true, 5);
		
		// Hbox com o P do Ojuara (label, Hscale [bar], Entry e button)
		label = new Gtk::Label("P");
		barP_ojuara.set_digits(3);
		barP_ojuara.set_increments(0.001,1);
		barP_ojuara.set_range(0,100);
		barP_ojuara.set_size_request(300,-1);
		boxP_ojuara.set_text(Glib::ustring::format(barP_ojuara.get_value()));
		boxP_ojuara.set_max_length(6);
		boxP_ojuara.set_width_chars(6);
		buttonP_ojuara.set_label("GO!");
		PID_hbox[7].set_halign(Gtk::ALIGN_CENTER);
		PID_vbox.pack_start(PID_hbox[7], false, true, 5);
		PID_hbox[7].pack_start(*label, false, true, 5);
		PID_hbox[7].pack_start(barP_ojuara, false, true, 5);
		PID_hbox[7].pack_start(boxP_ojuara, false, true, 5);
		PID_hbox[7].pack_start(buttonP_ojuara, false, true, 5);
		
		// Hbox com o I do Ojuara (label, Hscale [bar], Entry e button)
		label = new Gtk::Label("I");
		barI_ojuara.set_digits(3);
		barI_ojuara.set_increments(0.001,1);
		barI_ojuara.set_range(0,100);
		barI_ojuara.set_size_request(300,-1);
		boxI_ojuara.set_text(Glib::ustring::format(barI_ojuara.get_value()));
		boxI_ojuara.set_max_length(6);
		boxI_ojuara.set_width_chars(6);
		buttonI_ojuara.set_label("GO!");
		PID_hbox[8].set_halign(Gtk::ALIGN_CENTER);
		PID_vbox.pack_start(PID_hbox[8], false, true, 5);
		PID_hbox[8].pack_start(*label, false, true, 5);
		PID_hbox[8].pack_start(barI_ojuara, false, true, 5);
		PID_hbox[8].pack_start(boxI_ojuara, false, true, 5);
		PID_hbox[8].pack_start(buttonI_ojuara, false, true, 5);
		
		// Hbox com o D do Ojuara (label, Hscale [bar], Entry e button)
		label = new Gtk::Label("D");
		barD_ojuara.set_digits(3);
		barD_ojuara.set_increments(0.001,1);
		barD_ojuara.set_range(0,100);
		barD_ojuara.set_size_request(300,-1);
		boxD_ojuara.set_text(Glib::ustring::format(barD_ojuara.get_value()));
		boxD_ojuara.set_max_length(6);
		boxD_ojuara.set_width_chars(6);
		buttonD_ojuara.set_label("GO!");
		PID_hbox[9].set_halign(Gtk::ALIGN_CENTER);
		PID_vbox.pack_start(PID_hbox[9], false, true, 5);
		PID_hbox[9].pack_start(*label, false, true, 5);
		PID_hbox[9].pack_start(barD_ojuara, false, true, 5);
		PID_hbox[9].pack_start(boxD_ojuara, false, true, 5);
		PID_hbox[9].pack_start(buttonD_ojuara, false, true, 5);
		
		// Começar tudo (menos o botão PID calib) como insensível
		boxP_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		boxP_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		boxP_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		boxI_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		boxI_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		boxI_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		boxD_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		boxD_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		boxD_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		barP_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		barP_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		barP_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		barI_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		barI_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		barI_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		barD_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		barD_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		barD_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		save_bt.set_state(Gtk::STATE_INSENSITIVE);
		load_bt.set_state(Gtk::STATE_INSENSITIVE);
		buttonP_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		buttonI_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		buttonD_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		buttonP_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		buttonI_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		buttonD_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		buttonP_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		buttonI_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		buttonD_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		
		
		
		
		// Conectar os sinais para o acontecimento dos eventos
		PID_bt.signal_pressed().connect(sigc::mem_fun(*this, &ControlGUI::event_PID_bt_pressed));
		save_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_save_bt_signal_clicked));
		load_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_load_bt_signal_clicked));
		barP_goleiro.signal_value_changed().connect(sigc::mem_fun(*this, &ControlGUI::event_barP_goleiro_value_changed));
		barP_lenhador.signal_value_changed().connect(sigc::mem_fun(*this, &ControlGUI::event_barP_lenhador_value_changed));
		barP_ojuara.signal_value_changed().connect(sigc::mem_fun(*this, &ControlGUI::event_barP_ojuara_value_changed));
		barI_goleiro.signal_value_changed().connect(sigc::mem_fun(*this, &ControlGUI::event_barI_goleiro_value_changed));
		barI_lenhador.signal_value_changed().connect(sigc::mem_fun(*this, &ControlGUI::event_barI_lenhador_value_changed));
		barI_ojuara.signal_value_changed().connect(sigc::mem_fun(*this, &ControlGUI::event_barI_ojuara_value_changed));
		barD_goleiro.signal_value_changed().connect(sigc::mem_fun(*this, &ControlGUI::event_barD_goleiro_value_changed));
		barD_lenhador.signal_value_changed().connect(sigc::mem_fun(*this, &ControlGUI::event_barD_lenhador_value_changed));
		barD_ojuara.signal_value_changed().connect(sigc::mem_fun(*this, &ControlGUI::event_barD_ojuara_value_changed));
		buttonD_ojuara.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_buttonD_ojuara_signal_clicked));
		buttonP_goleiro.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_buttonP_goleiro_signal_clicked));
		buttonI_goleiro.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_buttonI_goleiro_signal_clicked));
		buttonD_goleiro.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_buttonD_goleiro_signal_clicked));
		buttonP_lenhador.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_buttonP_lenhador_signal_clicked));
		buttonI_lenhador.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_buttonI_lenhador_signal_clicked));
		buttonD_lenhador.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_buttonD_lenhador_signal_clicked));
		buttonP_ojuara.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_buttonP_ojuara_signal_clicked));
		buttonI_ojuara.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_buttonI_ojuara_signal_clicked));
		buttonD_ojuara.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::event_buttonD_ojuara_signal_clicked));
		//boxP_goleiro.signal_key_press_event().connect(sigc::mem_fun(*this, &ControlGUI::event_boxP_goleiro_signal_key_press), false);	

	}
	
	void event_PID_bt_pressed() {
		// Define se os botões estão no estado normal ou desativados,
		// controlado pelo botão PID_bt.
		if (!PID_bt_event_flag) {
		PID_bt_event_flag = !PID_bt_event_flag;
		boxP_goleiro.set_state(Gtk::STATE_NORMAL);
		boxP_lenhador.set_state(Gtk::STATE_NORMAL);
		boxP_ojuara.set_state(Gtk::STATE_NORMAL);
		boxI_goleiro.set_state(Gtk::STATE_NORMAL);
		boxI_lenhador.set_state(Gtk::STATE_NORMAL);
		boxI_ojuara.set_state(Gtk::STATE_NORMAL);
		boxD_goleiro.set_state(Gtk::STATE_NORMAL);
		boxD_lenhador.set_state(Gtk::STATE_NORMAL);
		boxD_ojuara.set_state(Gtk::STATE_NORMAL);
		barP_goleiro.set_state(Gtk::STATE_NORMAL);
		barP_lenhador.set_state(Gtk::STATE_NORMAL);
		barP_ojuara.set_state(Gtk::STATE_NORMAL);
		barI_goleiro.set_state(Gtk::STATE_NORMAL);
		barI_lenhador.set_state(Gtk::STATE_NORMAL);
		barI_ojuara.set_state(Gtk::STATE_NORMAL);
		barD_goleiro.set_state(Gtk::STATE_NORMAL);
		barD_lenhador.set_state(Gtk::STATE_NORMAL);
		barD_ojuara.set_state(Gtk::STATE_NORMAL);
		save_bt.set_state(Gtk::STATE_NORMAL);
		load_bt.set_state(Gtk::STATE_NORMAL);
		buttonP_goleiro.set_state(Gtk::STATE_NORMAL);
		buttonI_goleiro.set_state(Gtk::STATE_NORMAL);
		buttonD_goleiro.set_state(Gtk::STATE_NORMAL);
		buttonP_lenhador.set_state(Gtk::STATE_NORMAL);
		buttonI_lenhador.set_state(Gtk::STATE_NORMAL);
		buttonD_lenhador.set_state(Gtk::STATE_NORMAL);
		buttonP_ojuara.set_state(Gtk::STATE_NORMAL);
		buttonI_ojuara.set_state(Gtk::STATE_NORMAL);
		buttonD_ojuara.set_state(Gtk::STATE_NORMAL);
		
	} else {
		PID_bt_event_flag = !PID_bt_event_flag;
		boxP_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		boxP_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		boxP_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		boxI_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		boxI_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		boxI_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		boxD_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		boxD_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		boxD_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		barP_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		barP_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		barP_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		barI_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		barI_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		barI_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		barD_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		barD_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		barD_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		save_bt.set_state(Gtk::STATE_INSENSITIVE);
		load_bt.set_state(Gtk::STATE_INSENSITIVE);
		buttonP_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		buttonI_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		buttonD_goleiro.set_state(Gtk::STATE_INSENSITIVE);
		buttonP_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		buttonI_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		buttonD_lenhador.set_state(Gtk::STATE_INSENSITIVE);
		buttonP_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		buttonI_ojuara.set_state(Gtk::STATE_INSENSITIVE);
		buttonD_ojuara.set_state(Gtk::STATE_INSENSITIVE);
	}
	
	
}

void event_save_bt_signal_clicked() {
	// Salva os valores do PID de cada jogador no arquivo PID_config.txt
	ofstream txtFile;
	txtFile.open("PID_config.txt");
	txtFile << boxP_goleiro.get_text() <<std::endl;
	txtFile << boxI_goleiro.get_text() <<std::endl;
	txtFile << boxD_goleiro.get_text() <<std::endl;
	txtFile << boxP_lenhador.get_text() <<std::endl;
	txtFile << boxI_lenhador.get_text() <<std::endl;
	txtFile << boxD_lenhador.get_text() <<std::endl;
	txtFile << boxP_ojuara.get_text() <<std::endl;
	txtFile << boxI_ojuara.get_text() <<std::endl;
	txtFile << boxD_ojuara.get_text() <<std::endl;
	
	// Fecha o arquivo
	txtFile.close();
}

void event_load_bt_signal_clicked() {
	// Carrega os valores do PID de cada jogador do arquivo PID_config.txt
	ifstream txtFile;
	string linha;
	txtFile.open("PID_config.txt");
	getline(txtFile, linha); boxP_goleiro.set_text(linha.c_str());
	getline(txtFile, linha); boxI_goleiro.set_text(linha.c_str());
	getline(txtFile, linha); boxD_goleiro.set_text(linha.c_str());
	getline(txtFile, linha); boxP_lenhador.set_text(linha.c_str());
	getline(txtFile, linha); boxI_lenhador.set_text(linha.c_str());
	getline(txtFile, linha); boxD_lenhador.set_text(linha.c_str());
	getline(txtFile, linha); boxP_ojuara.set_text(linha.c_str());
	getline(txtFile, linha); boxI_ojuara.set_text(linha.c_str());
	getline(txtFile, linha); boxD_ojuara.set_text(linha.c_str());
	
	//Ajusta as Hscales para os valores lidos no load
	event_buttonP_goleiro_signal_clicked();
	event_buttonI_goleiro_signal_clicked();
	event_buttonD_goleiro_signal_clicked();
	event_buttonP_lenhador_signal_clicked();
	event_buttonI_lenhador_signal_clicked();
	event_buttonD_lenhador_signal_clicked();
	event_buttonP_ojuara_signal_clicked();
	event_buttonI_ojuara_signal_clicked();
	event_buttonD_ojuara_signal_clicked();
	
	// Fecha o arquivo
	txtFile.close();
}

void event_barP_goleiro_value_changed() {
	// Essa função espelha o valor da HScale na TextBox (Entry) correspondente
		boxP_goleiro.set_text(Glib::ustring::format(barP_goleiro.get_value()));
	}
	
void event_barP_lenhador_value_changed() {
	// Essa função espelha o valor da HScale na TextBox (Entry) correspondente
		boxP_lenhador.set_text(Glib::ustring::format(barP_lenhador.get_value()));
	}
	
void event_barP_ojuara_value_changed() {
	// Essa função espelha o valor da HScale na TextBox (Entry) correspondente
		boxP_ojuara.set_text(Glib::ustring::format(barP_ojuara.get_value()));
	}

void event_barI_goleiro_value_changed() {
	// Essa função espelha o valor da HScale na TextBox (Entry) correspondente
		boxI_goleiro.set_text(Glib::ustring::format(barI_goleiro.get_value()));
	}

void event_barI_lenhador_value_changed() {
	// Essa função espelha o valor da HScale na TextBox (Entry) correspondente
		boxI_lenhador.set_text(Glib::ustring::format(barI_lenhador.get_value()));
	}
	
void event_barI_ojuara_value_changed() {
	// Essa função espelha o valor da HScale na TextBox (Entry) correspondente
		boxI_ojuara.set_text(Glib::ustring::format(barI_ojuara.get_value()));
	}
	
void event_barD_goleiro_value_changed() {
	// Essa função espelha o valor da HScale na TextBox (Entry) correspondente
		boxD_goleiro.set_text(Glib::ustring::format(barD_goleiro.get_value()));
	}
	
void event_barD_lenhador_value_changed() {
	// Essa função espelha o valor da HScale na TextBox (Entry) correspondente
		boxD_lenhador.set_text(Glib::ustring::format(barD_lenhador.get_value()));
	}
	
void event_barD_ojuara_value_changed() {
	// Essa função espelha o valor da HScale na TextBox (Entry) correspondente
		boxD_ojuara.set_text(Glib::ustring::format(barD_ojuara.get_value()));
	}
	
void event_buttonP_goleiro_signal_clicked() {
	// Essa função pega o conteúdo da text box (entry) P do goleiro,
	// converte para string, muda o '.' para ',' (para não dar merda depois),
	// e passa o valor para a barra P do goleiro.
		std::string s = boxP_goleiro.get_text();
		std::size_t found;
		found = s.rfind('.');
		if (found!=std::string::npos)
			s[found] = ',';
		try {
			barP_goleiro.set_value( std::stod(s.c_str()) );
		} catch (const std::invalid_argument&) {
	};
}

void event_buttonI_goleiro_signal_clicked() {
	// Essa função pega o conteúdo da text box (entry) I do goleiro,
	// converte para string, muda o '.' para ',' (para não dar merda depois),
	// e passa o valor para a barra I do goleiro.
	std::string s = boxI_goleiro.get_text();
		std::size_t found;
		found = s.rfind('.');
		if (found!=std::string::npos)
			s[found] = ',';
		try {
			barI_goleiro.set_value( std::stod(s.c_str()) );
		} catch (const std::invalid_argument&) {
	};
}


void event_buttonD_goleiro_signal_clicked() {
	// Essa função pega o conteúdo da text box (entry) D do goleiro,
	// converte para string, muda o '.' para ',' (para não dar merda depois),
	// e passa o valor para a barra D do goleiro.
		std::string s = boxD_goleiro.get_text();
		std::size_t found;
		found = s.rfind('.');
		if (found!=std::string::npos)
			s[found] = ',';
		try {
			barD_goleiro.set_value( std::stod(s.c_str()) );
		} catch (const std::invalid_argument&) {
	};
}


void event_buttonP_lenhador_signal_clicked() {
	// Essa função pega o conteúdo da text box (entry) P do lenhador,
	// converte para string, muda o '.' para ',' (para não dar merda depois),
	// e passa o valor para a barra P do lenhador.
		std::string s = boxP_lenhador.get_text();
		std::size_t found;
		found = s.rfind('.');
		if (found!=std::string::npos)
			s[found] = ',';
		try {
			barP_lenhador.set_value( std::stod(s.c_str()) );
		} catch (const std::invalid_argument&) {
	};
}


void event_buttonI_lenhador_signal_clicked() {
		// Essa função pega o conteúdo da text box (entry) I do lenhador,
	// converte para string, muda o '.' para ',' (para não dar merda depois),
	// e passa o valor para a barra I do lenhador.
		std::string s = boxI_lenhador.get_text();
		std::size_t found;
		found = s.rfind('.');
		if (found!=std::string::npos)
			s[found] = ',';
		try {
			barI_lenhador.set_value( std::stod(s.c_str()) );
		} catch (const std::invalid_argument&) {
	};
}


void event_buttonD_lenhador_signal_clicked() {
		// Essa função pega o conteúdo da text box (entry) D do lenhador,
	// converte para string, muda o '.' para ',' (para não dar merda depois),
	// e passa o valor para a barra D do lenhador.
		std::string s = boxD_lenhador.get_text();
		std::size_t found;
		found = s.rfind('.');
		if (found!=std::string::npos)
			s[found] = ',';
		try {			barD_lenhador.set_value( std::stod(s.c_str()) );
		} catch (const std::invalid_argument&) {
	};
}


void event_buttonP_ojuara_signal_clicked() {
		// Essa função pega o conteúdo da text box (entry) P do ojuara,
	// converte para string, muda o '.' para ',' (para não dar merda depois),
	// e passa o valor para a barra P do ojuara.
	std::string s = boxP_ojuara.get_text();
		std::size_t found;
		found = s.rfind('.');
		if (found!=std::string::npos)
			s[found] = ',';
		try {
			barP_ojuara.set_value( std::stod(s.c_str()) );
		} catch (const std::invalid_argument&) {
	};
}


void event_buttonI_ojuara_signal_clicked() {
			// Essa função pega o conteúdo da text box (entry) I do ojuara,
	// converte para string, muda o '.' para ',' (para não dar merda depois),
	// e passa o valor para a barra I do ojuara.
	std::string s = boxI_ojuara.get_text();
		std::size_t found;
		found = s.rfind('.');
		if (found!=std::string::npos)
			s[found] = ',';
		try {
			barI_ojuara.set_value( std::stod(s.c_str()) );
		} catch (const std::invalid_argument&) {
	};
}


void event_buttonD_ojuara_signal_clicked() {
			// Essa função pega o conteúdo da text box (entry) D do ojuara,
	// converte para string, muda o '.' para ',' (para não dar merda depois),
	// e passa o valor para a barra D do ojuara.
		std::string s = boxD_ojuara.get_text();
		std::size_t found;
		found = s.rfind('.');
		if (found!=std::string::npos)
			s[found] = ',';
		try {
			barD_ojuara.set_value( std::stod(s.c_str()) );
		} catch (const std::invalid_argument&) {
	};
}

/*void event_boxP_goleiro_signal_key_press(GdkEventKey* event)
{
		//if (event.keyval == 13)
		//	event_buttonP_goleiro_signal_clicked();
		return;
}*/

};



#endif /* CONTROLGUI_HPP_ */
