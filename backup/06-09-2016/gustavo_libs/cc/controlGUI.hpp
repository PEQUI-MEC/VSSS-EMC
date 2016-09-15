/*
 * controlGUI.hpp
 *
 *  Created on: May 8th, 2016
 *      Author: Daniel
 */

#ifndef CONTROLGUI_HPP_
#define CONTROLGUI_HPP_

#include <gtkmm.h>

class ControlGUI: public Gtk::VBox
{
public:
	
	Gtk::Frame fm;
	Gtk::VBox leftBox;
	Gtk::HBox rightBox;

	Gtk::ToggleButton PIDbutton, CGAbutton;

	Gtk::Notebook configTabs;
	Gtk::Label l1, l2, l3;

	Gtk::Table *table;

	Gtk::Label Plabel;
	Gtk::TextView Pbox;
	Gtk::HScale Pbar;

	Gtk::Label Ilabel;
	Gtk::TextView Ibox;
	Gtk::HScale Ibar;

	Gtk::Label Dlabel;
	Gtk::TextView Dbox;
	Gtk::HScale Dbar;

	ControlGUI()
	{

		PIDbutton.set_active(true); // Botao PID habilitado
		PIDbutton.set_label("PID");

		CGAbutton.set_active(false); // Botao CGA habilitado
		CGAbutton.set_label("CGA");

		table = new Gtk::Table(3, 3, true);

		l1.set_text("Perfil 1");
		l2.set_text("Perfil 2");
		l3.set_text("Perfil 3");
		configTabs.set_border_width(5);
		configTabs.append_page(*table, "Perfil 1");
		configTabs.append_page(l2, "Perfil 2");
		configTabs.append_page(l3, "Perfil 3");


		Plabel.set_text("P");
		Ilabel.set_text("I");
		Dlabel.set_text("D");
		
		

		table->attach(Plabel, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL, 10, 5);
		table->attach(Pbox, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL, 10, 5);
		table->attach(Ilabel, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL, 10, 5);
		table->attach(Ibox, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL, 10, 5);
		table->attach(Dlabel, 0, 1, 2, 3, Gtk::FILL, Gtk::FILL, 10, 5);
		table->attach(Dbox, 1, 2, 2, 3, Gtk::FILL, Gtk::FILL, 10, 5);

		table->set_border_width(0);
		table->set_size_request(200, 200);


		leftBox.pack_start(PIDbutton, false, false, 10);
		leftBox.pack_start(CGAbutton, false, false, 10);

		rightBox.pack_start(configTabs);

		pack_start(fm, true, true, 10);
		//pack_start(leftBox, false, false, 0);
		pack_start(rightBox, false, false, 0);
	}

	~ControlGUI()
	{

	}



};

#endif /* CONTROLGUI_HPP_ */
