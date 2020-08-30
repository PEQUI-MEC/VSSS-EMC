#include "XbeeSelectGUI.hpp"
#include <fcntl.h>

XbeeSelectGUI::XbeeSelectGUI(Messenger &messenger) : messenger(messenger) {
	xbee_port_label.set_text("XBee Port:");
	connect_bt.set_label("Connect");
	refresh_bt.set_label("Refresh");

	pack_start(xbee_port_label, false, false, 5);
	pack_start(xbee_cb, false, false, 5);
	pack_start(connect_bt, false, false, 5);
	pack_start(refresh_bt, false, false, 5);

	connect_bt.signal_clicked().connect(sigc::mem_fun(this, &XbeeSelectGUI::start_xbee));
	refresh_bt.signal_clicked().connect(sigc::mem_fun(this, &XbeeSelectGUI::refresh_xbee_cb));
	show_all();
}

XbeeSelectGUI::~XbeeSelectGUI() {
	if (xbee_port.has_value()) messenger.stop_xbee(xbee_port.value());
	update_widgets_state_signal.emit();
}

void XbeeSelectGUI::start_xbee() {
	std::string port = xbee_cb.get_active_text();
	if (port.empty()) return;
	messenger.start_xbee(port);
	xbee_port = port;
	update_widgets_state_signal.emit();
}

void XbeeSelectGUI::auto_start_xbee() {
	if (xbees_found == 1) {
		start_xbee();
	}
}

void XbeeSelectGUI::refresh_xbee_cb() {
	if (xbee_port.has_value()) messenger.stop_xbee(xbee_port.value());
	xbee_port = std::nullopt;
	update_combobox();
	update_widgets_state_signal.emit();
}

void XbeeSelectGUI::update_combobox() {
	xbee_cb.remove_all();
	xbees_found = 0;

	for (int i = 0; i < 256; ++i) {
		std::string port = "/dev/ttyUSB" + std::to_string(i);
		if (!messenger.using_port(port)) {
			int fd = open(port.c_str(), O_RDWR);
			if (fd != -1) {
				xbee_cb.append(port);
				close(fd);
				xbees_found++;
			}
		}
	}

	// Caso tenha algum dispositivo, atualizar a combo box
	if (xbees_found > 0)
		xbee_cb.set_active(0);
}
