#ifndef VSSS_XBEESELECTGUI_HPP
#define VSSS_XBEESELECTGUI_HPP

#include <Messenger.h>
#include <gtkmm.h>

class XbeeSelectGUI : public Gtk::HBox {
	public:
	Messenger& messenger;

	std::optional<unsigned int> xbee_index;
	int xbees_found = 0;

	Gtk::Label radio_xbee_lbl;
	Gtk::ComboBoxText radio_xbee_cb;
	Gtk::Button radio_connect_bt;
	Gtk::Button radio_refresh_bt;
//	Gtk::Button add_bt;
//	std::optional<Gtk::Button> remove_bt;

	XbeeSelectGUI(Messenger &messenger, bool removable);

	// essa função não deve ser chamada imediatamente após desconectar o xbee
	// o xbee demora alguns segundos para realmente desconectar
	void start_xbee();

//	Conecta ao xbee automaticamente se só encontrar um
	void auto_start_xbee();

	void refresh_xbee_cb();
};

#endif //VSSS_XBEESELECTGUI_HPP
