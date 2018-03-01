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
#include <string>
#include "Messenger.h"
#include "TestFrame.hpp"
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
// system_clock::now
#include <iostream>
#include <ctime>
#include <chrono>

class ControlGUI: public Gtk::VBox
{
public:
	Messenger messenger;

	const static int TOTAL_ROBOTS = 6;

	TestFrame testFrame;

	// Flag para saber se o botão PID está pressionado ou não.
	bool PID_test_flag = false;
	// Containers para o conteúdo da interface gráfica
	Gtk::Frame Serial_fm;
	Gtk::Frame Test_fm;
	Gtk::HBox Top_hbox;
	Gtk::VBox Serial_vbox;
	Gtk::VBox Test_vbox;
	Gtk::HBox Serial_hbox[3];
	Gtk::Label *label;
	Gtk::Button bt_send_cmd;
	Gtk::Entry send_cmd_box;

	// Botões e combo box Rádio
	Gtk::Button bt_Serial_Start;
	Gtk::Button bt_Robot_Status;
	Gtk::Button bt_Serial_Refresh;
	Gtk::ComboBoxText cb_serial;
	Gtk::ToggleButton button_PID_Test;
	Gtk::Button bt_Serial_test;
	Gtk::ComboBoxText cb_test;
	Gtk::Entry Tbox_V1;
	Gtk::Entry Tbox_V2;

	Gtk::Grid status_grid;
	Gtk::Frame status_fm;
	Gtk::Image status_img[TOTAL_ROBOTS];
	Gtk::Label robots_lb[TOTAL_ROBOTS];
	Gtk::Label status_lb[TOTAL_ROBOTS];
	Gtk::Label lastUpdate_lb;
	Gtk::ProgressBar battery_bar[TOTAL_ROBOTS];
		Gtk::Label dropped_frames[TOTAL_ROBOTS];
		Gtk::Button bt_reset_ack;

	Gtk::Frame pid_fm;
	Gtk::VBox pid_vbox;
	Gtk::HBox pid_hbox[2];
	Gtk::Button pid_edit_bt;
	Gtk::Button pid_send_bt;
	Gtk::Entry pid_box[3];
	Glib::ustring pid_tmp[4];
	Gtk::ComboBoxText cb_pid_robot;
	Gtk::ComboBoxText cb_pid_type;
	bool pid_edit_flag = false;

	ControlGUI();

	bool get_PID_test_flag();

	void set_PID_test_flag(bool input);

	void configureTestFrame();

	void _send_command();

	void _PID_Test();

	// Gets battery % and robot id to update a single robot's battery status
	void updateInterfaceStatus(double battery, int id);

	// update the battery status of all robots
	void _robot_status();

	void _start_serial();

	bool isFloat(std::string value);

	void _send_test();

	void _update_cb_serial();

	void _create_status_frame();

	// Função para verificar se os valores digitados nos campos
	// de PID são válidos: apenas números e um único ponto
	bool checkPIDvalues();
	
	int get_robot_pos(char id);
    char get_robot_id(int pos);
		void update_dropped_frames();
		void reset_lost_acks();
};



#endif /* CONTROLGUI_HPP_ */
