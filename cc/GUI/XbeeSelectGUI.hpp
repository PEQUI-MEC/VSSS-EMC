#ifndef VSSS_XBEESELECTGUI_HPP
#define VSSS_XBEESELECTGUI_HPP

#include <Messenger.h>
#include <gtkmm.h>

class XbeeSelectGUI : public Gtk::HBox {
	public:
	Messenger& messenger;

	std::optional<std::string> xbee_port = std::nullopt;
	int xbees_found = 0;

	Gtk::Label xbee_port_label;
	Gtk::ComboBoxText xbee_cb;
	Gtk::Button connect_bt;
	Gtk::Button refresh_bt;

//	Avisa ControlGUI que deve atualizar o estado dos widgets
	sigc::signal<void()> update_widgets_state_signal;

	explicit XbeeSelectGUI(Messenger &messenger);
	~XbeeSelectGUI();

	// essa função não deve ser chamada imediatamente após desconectar o xbee
	// o xbee demora alguns segundos para realmente desconectar
	void start_xbee();

//	Conecta ao xbee automaticamente se só encontrar um
	void auto_start_xbee();

	void refresh_xbee_cb();

	void update_combobox();

	bool has_xbee() { return xbee_port.has_value(); };
};

#endif //VSSS_XBEESELECTGUI_HPP
