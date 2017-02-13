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


#include "opencv2/opencv.hpp"
#include <gtkmm.h>
#include <fstream>
#include <string>
#include "SerialW.hpp"
#include "filechooser.cpp"

class ControlGUI: public Gtk::VBox
{
public:
	SerialW s;


	// Flag para saber se o botão PID está pressionado ou não.

	bool Serial_Enabled;
	bool PID_bt_event_flag = false;
	bool PID_test_flag = false;
	// Containers para o conteúdo da interface gráfica
	Gtk::Frame Serial_fm;
	Gtk::Frame Test_fm;
	Gtk::HBox Top_hbox;
	Gtk::VBox Serial_vbox;
	Gtk::VBox Test_vbox;
	Gtk::HBox Serial_hbox[2];
	Gtk::Label *label;

	// Botões e combo box Rádio
	Gtk::Button bt_Serial_Start;
	Gtk::Button bt_Serial_Refresh;
	Gtk::ComboBoxText cb_serial;
	Gtk::ToggleButton button_PID_Test;
	Gtk::Button bt_Serial_test;
	Gtk::ComboBoxText cb_test;
	Gtk::Entry Tbox_V1;
	Gtk::Entry Tbox_V2;

	Gtk::Frame pid_fm;
	Gtk::VBox pid_vbox;
	Gtk::HBox pid_hbox[2];
	Gtk::Button pid_edit_bt;
	Gtk::Button pid_done_bt;
	Gtk::Entry pid_box[3];
	Glib::ustring pid_tmp[4];
	Gtk::ComboBoxText cb_pid;
	bool pid_edit_flag = false;

	ControlGUI()
	{
		PID_bt_event_flag=false;
		PID_bt_event_flag =false;
		Serial_Enabled=false;
		// Adicionar o frame do Serial e sua VBOX
		pack_start(Top_hbox, false, true, 5);
		Top_hbox.pack_start(Serial_fm, false, true, 5);
		Serial_fm.set_label("Serial");
		Serial_fm.add(Serial_vbox);

		Top_hbox.pack_start(Test_fm, false, true, 5);

		Test_fm.set_label("Test");
		Test_fm.add(Test_vbox);
		button_PID_Test.set_label("PID Test on Click");
		Test_vbox.pack_start(button_PID_Test, false, true, 5);

		Serial_hbox[0].pack_start(cb_serial, false, true, 5);
		Serial_hbox[0].pack_start(bt_Serial_Start, false, true, 5);
		Serial_hbox[0].pack_start(bt_Serial_Refresh, false, true, 5);
		Serial_vbox.pack_start(Serial_hbox[0], false, true, 5);

		Serial_hbox[1].pack_start(cb_test, false, true, 5);
		Serial_hbox[1].pack_start(Tbox_V1, false, true, 5);
		Serial_hbox[1].pack_start(Tbox_V2, false, true, 5);
		Serial_hbox[1].pack_start(bt_Serial_test, false, true, 5);
		Serial_vbox.pack_start(Serial_hbox[1], false, true, 5);

		Tbox_V1.set_max_length(6);
		Tbox_V2.set_max_length(6);
		Tbox_V1.set_width_chars(6);
		Tbox_V2.set_width_chars(6);
		cb_test.append("Robo A");
		cb_test.append("Robo B");
		cb_test.append("Robo C");
		cb_test.append("Robo D");
		cb_test.append("All");

		cb_test.set_active(4); // ALL
		Tbox_V1.set_text("3");
		Tbox_V2.set_text("3");


		bt_Serial_Start.set_state(Gtk::STATE_NORMAL);
		cb_serial.set_state(Gtk::STATE_NORMAL);

		Tbox_V1.set_state(Gtk::STATE_INSENSITIVE);
		Tbox_V2.set_state(Gtk::STATE_INSENSITIVE);
		bt_Serial_Refresh.set_state(Gtk::STATE_INSENSITIVE);
		bt_Serial_test.set_state(Gtk::STATE_INSENSITIVE);
		cb_test.set_state(Gtk::STATE_INSENSITIVE);

		bt_Serial_Start.set_label("Start");
		bt_Serial_Refresh.set_label("Refresh");

		bt_Serial_test.set_label("Send");

		_create_pid_frame();

		_update_cb_serial();
		// Conectar os sinais para o acontecimento dos eventos
		button_PID_Test.signal_pressed().connect(sigc::mem_fun(*this, &ControlGUI::_PID_Test));
		bt_Serial_test.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_send_test));
		bt_Serial_Refresh.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_update_cb_serial));
		bt_Serial_Start.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_start_serial));
	}

void _PID_Test(){
	if (PID_test_flag)
	{

		PID_test_flag = false;
		std::cout<<PID_test_flag<<endl;

	}
	else
	{
		PID_test_flag = true;
		std::cout<<PID_test_flag<<endl;
	}

}

void _start_serial(){
		int fd;
		Glib::ustring serial = cb_serial.get_active_text();

		if (serial.size() < 1) return;
		fd = s.start(serial);

	 if(fd != -1)
    {
       	 std::cout<<serial<<" - Connected"<<std::endl;
    }else{

		std::cout<<serial<<" - Error"<<std::endl;
		}

		bt_Serial_Start.set_state(Gtk::STATE_INSENSITIVE);
		cb_serial.set_state(Gtk::STATE_INSENSITIVE);

		Tbox_V1.set_state(Gtk::STATE_NORMAL);
		Tbox_V2.set_state(Gtk::STATE_NORMAL);
		bt_Serial_Refresh.set_state(Gtk::STATE_NORMAL);
		bt_Serial_test.set_state(Gtk::STATE_NORMAL);
		cb_test.set_state(Gtk::STATE_NORMAL);



	}

void _send_test(){
	std::string cmd;
	switch(cb_test.get_active_row_number()){
	case -1:
	return;
	break;

	case 0:
	cmd.append("A");
	cmd.append(Tbox_V1.get_text());
	cmd.append(";");
	cmd.append(Tbox_V2.get_text());
	cmd.append("#");
	s.sendSerial(cmd);
	break;

	case 1:
	cmd.append("B");
	cmd.append(Tbox_V1.get_text());
	cmd.append(";");
	cmd.append(Tbox_V2.get_text());
	cmd.append("#");
	s.sendSerial(cmd);
	break;

	case 2:
	cmd.append("C");
	cmd.append(Tbox_V1.get_text());
	cmd.append(";");
	cmd.append(Tbox_V2.get_text());
	cmd.append("#");
	s.sendSerial(cmd);
	break;

	case 3:
	cmd.append("D");
	cmd.append(Tbox_V1.get_text());
	cmd.append(";");
	cmd.append(Tbox_V2.get_text());
	cmd.append("#");
	s.sendSerial(cmd);
	break;

	case 4:
	cmd.append("A");
	cmd.append(Tbox_V1.get_text());
	cmd.append(";");
	cmd.append(Tbox_V2.get_text());
	cmd.append("#");
	//s.sendSerial(cmd);
	cmd.append("B");
	cmd.append(Tbox_V1.get_text());
	cmd.append(";");
	cmd.append(Tbox_V2.get_text());
	cmd.append("#");
	//s.sendSerial(cmd);
	cmd.append("C");
	cmd.append(Tbox_V1.get_text());
	cmd.append(";");
	cmd.append(Tbox_V2.get_text());
	cmd.append("#");
	//s.sendSerial(cmd);
	cmd.append("D");
	cmd.append(Tbox_V1.get_text());
	cmd.append(";");
	cmd.append(Tbox_V2.get_text());
	cmd.append("#");
	s.sendSerial(cmd);
	break;


		}



	}

void _update_cb_serial(){

std::string port;
int fd;
cb_serial.remove_all();
for(int i = 0; i < 256; ++i)
	{
    port.clear();
    port.append("/dev/ttyUSB");
    port.append(std::to_string(i));

    fd = open(port.c_str(), O_RDWR);
    if(fd != -1)
    {
		 std::cout<<port<<std::endl;
        cb_serial.append(port);
    }
	}
		bt_Serial_Start.set_state(Gtk::STATE_NORMAL);
		cb_serial.set_state(Gtk::STATE_NORMAL);
		bt_Serial_Refresh.set_state(Gtk::STATE_NORMAL);

		Tbox_V1.set_state(Gtk::STATE_INSENSITIVE);
		Tbox_V2.set_state(Gtk::STATE_INSENSITIVE);
		bt_Serial_test.set_state(Gtk::STATE_INSENSITIVE);
		cb_test.set_state(Gtk::STATE_INSENSITIVE);

}

void _create_pid_frame(){

		pack_start(pid_fm, false, true, 5);
		pid_fm.set_label("PID");
		pid_fm.add(pid_vbox);
		pid_vbox.pack_start(pid_hbox[0], false, true, 5);
		pid_vbox.pack_start(pid_hbox[1], false, true, 5);

		pid_hbox[0].pack_start(pid_edit_bt, false, true, 5);
		pid_hbox[0].pack_end(pid_done_bt, false, true, 5);
		pid_edit_bt.set_label("Edit");
		pid_done_bt.set_label("Done");

		cb_pid.append("All Robots");
		cb_pid.append("Robot A");
		cb_pid.append("Robot B");
		cb_pid.append("Robot C");
		cb_pid.append("Robot D");
		cb_pid.set_active(0);

		pid_hbox[1].pack_start(cb_pid, false, true, 5);
		label = new Gtk::Label("P: ");
		pid_hbox[1].pack_start(*label, false, true, 5);
		pid_hbox[1].pack_start(pid_box[0], false, true, 5);
		pid_box[0].set_max_length(5);
		pid_box[0].set_width_chars(6);
		pid_box[0].set_text(Glib::ustring::format("0"));

		label = new Gtk::Label("I: ");
		pid_hbox[1].pack_start(*label, false, true, 5);
		pid_hbox[1].pack_start(pid_box[1], false, true, 5);
		pid_box[1].set_max_length(5);
		pid_box[1].set_width_chars(6);
		pid_box[1].set_text(Glib::ustring::format("0"));

		label = new Gtk::Label("D: ");
		pid_hbox[1].pack_start(*label, false, true, 5);
		pid_hbox[1].pack_start(pid_box[2], false, true, 5);
		pid_box[2].set_max_length(5);
		pid_box[2].set_width_chars(6);
		pid_box[2].set_text(Glib::ustring::format("0"));

		/*pid_box[0].set_state(Gtk::STATE_INSENSITIVE);
		pid_box[1].set_state(Gtk::STATE_INSENSITIVE);
		pid_box[2].set_state(Gtk::STATE_INSENSITIVE);
		pid_done_bt.set_state(Gtk::STATE_INSENSITIVE);
		pid_edit_bt.set_state(Gtk::STATE_INSENSITIVE);
		cb_pid.set_state(Gtk::STATE_INSENSITIVE);*/

		pid_edit_bt.signal_pressed().connect(sigc::mem_fun(*this, &ControlGUI::_event_pid_edit_bt_clicked));
		pid_done_bt.signal_clicked().connect(sigc::mem_fun(*this, &ControlGUI::_event_pid_done_bt_clicked));
	}

	void _event_pid_edit_bt_clicked(){
		if (!pid_edit_flag)
		{
			pid_edit_flag = true;
			pid_edit_bt.set_label("Cancel");
			pid_box[0].set_state(Gtk::STATE_NORMAL);
			pid_box[1].set_state(Gtk::STATE_NORMAL);
			pid_box[2].set_state(Gtk::STATE_NORMAL);
			pid_done_bt.set_state(Gtk::STATE_NORMAL);
			cb_pid.set_state(Gtk::STATE_NORMAL);
			pid_tmp[0] = pid_box[0].get_text();
			pid_tmp[1] = pid_box[1].get_text();
			pid_tmp[2] = pid_box[2].get_text();

		}
		else
		{
			pid_edit_flag = false;
			pid_edit_bt.set_label("Edit");
			pid_box[0].set_state(Gtk::STATE_INSENSITIVE);
			pid_box[1].set_state(Gtk::STATE_INSENSITIVE);
			pid_box[2].set_state(Gtk::STATE_INSENSITIVE);
			pid_done_bt.set_state(Gtk::STATE_INSENSITIVE);
			cb_pid.set_state(Gtk::STATE_INSENSITIVE);
			pid_box[0].set_text(pid_tmp[0]);
			pid_box[1].set_text(pid_tmp[1]);
			pid_box[2].set_text(pid_tmp[2]);
		}
	}

	void _event_pid_done_bt_clicked()
	{
		std::string cmd;
		switch (cb_pid.get_active_row_number())
		{
			case 0:
			cmd.append("AK");
			cmd.append(pid_box[0].get_text());
			cmd.append(";");
			cmd.append(pid_box[1].get_text());
			cmd.append(";");
			cmd.append(pid_box[2].get_text());
			cmd.append("#");

			cmd.append("BK");
			cmd.append(pid_box[0].get_text());
			cmd.append(";");
			cmd.append(pid_box[1].get_text());
			cmd.append(";");
			cmd.append(pid_box[2].get_text());
			cmd.append("#");

			cmd.append("CK");
			cmd.append(pid_box[0].get_text());
			cmd.append(";");
			cmd.append(pid_box[1].get_text());
			cmd.append(";");
			cmd.append(pid_box[2].get_text());
			cmd.append("#");

			cmd.append("DK");
			cmd.append(pid_box[0].get_text());
			cmd.append(";");
			cmd.append(pid_box[1].get_text());
			cmd.append(";");
			cmd.append(pid_box[2].get_text());
			cmd.append("#");
			s.sendSerial(cmd);
			break;

			case 1:
			cmd.append("AK");
			cmd.append(pid_box[0].get_text());
			cmd.append(";");
			cmd.append(pid_box[1].get_text());
			cmd.append(";");
			cmd.append(pid_box[2].get_text());
			cmd.append("#");
			s.sendSerial(cmd);
			break;

			case 2:
			cmd.append("BK");
			cmd.append(pid_box[0].get_text());
			cmd.append(";");
			cmd.append(pid_box[1].get_text());
			cmd.append(";");
			cmd.append(pid_box[2].get_text());
			cmd.append("#");
			s.sendSerial(cmd);
			break;

			case 3:
			cmd.append("CK");
			cmd.append(pid_box[0].get_text());
			cmd.append(";");
			cmd.append(pid_box[1].get_text());
			cmd.append(";");
			cmd.append(pid_box[2].get_text());
			cmd.append("#");
			s.sendSerial(cmd);
			break;

			case 4:
			cmd.append("DK");
			cmd.append(pid_box[0].get_text());
			cmd.append(";");
			cmd.append(pid_box[1].get_text());
			cmd.append(";");
			cmd.append(pid_box[2].get_text());
			cmd.append("#");
			s.sendSerial(cmd);
			break;

			default:
			std::cout << "SET PID ERROR: Could not set PID." << std::endl;
			break;
		}

		std::cout << "Set PID - cmd: " << cmd << std::endl;

		pid_edit_flag = false;
		pid_edit_bt.set_label("Edit");
		pid_done_bt.set_state(Gtk::STATE_INSENSITIVE);
		pid_box[0].set_state(Gtk::STATE_INSENSITIVE);
		pid_box[1].set_state(Gtk::STATE_INSENSITIVE);
		pid_box[2].set_state(Gtk::STATE_INSENSITIVE);
		cb_pid.set_state(Gtk::STATE_INSENSITIVE);

	}

};



#endif /* CONTROLGUI_HPP_ */
