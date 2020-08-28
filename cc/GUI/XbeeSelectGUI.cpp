#include "XbeeSelectGUI.hpp"
#include <fcntl.h>

XbeeSelectGUI::XbeeSelectGUI(Messenger &messenger, bool removable) : messenger(messenger) {
	radio_xbee_lbl.set_text("XBee Port:");
	radio_connect_bt.set_label("Connect");
	radio_refresh_bt.set_label("Refresh");
//	add_bt.set_label(" + ");
//	if (removable) {
//		remove_bt.emplace();
//		remove_bt.value().set_label(" - ");
//	}
	pack_start(radio_xbee_lbl, false, false, 5);
	pack_start(radio_xbee_cb, false, false, 5);
	pack_start(radio_connect_bt, false, false, 5);
	pack_start(radio_refresh_bt, false, false, 5);
	pack_start(radio_xbee_lbl, false, false, 5);
//	pack_start(add_bt, false, false, 5);
//	if (remove_bt.has_value()) pack_start(remove_bt.value(), false, false, 5);

	refresh_xbee_cb();
}

void XbeeSelectGUI::start_xbee() {
	Glib::ustring serial = radio_xbee_cb.get_active_text();
	if (serial.empty()) return;

	xbee_index = messenger.start_xbee(serial);
}

void XbeeSelectGUI::auto_start_xbee() {
	if (xbees_found == 1) {
		start_xbee();
	}
}

void XbeeSelectGUI::refresh_xbee_cb() {
	if (xbee_index.has_value()) messenger.stop_xbee(xbee_index.value());
	xbee_index = std::nullopt;

	radio_xbee_cb.remove_all();
	xbees_found = 0;

	for (int i = 0; i < 256; ++i) {
		std::string port = "/dev/ttyUSB";
		port.append(std::to_string(i));

		int fd = open(port.c_str(), O_RDWR);
		if (fd != -1) {
			std::cout << port << std::endl;
			radio_xbee_cb.append(port);
			close(fd);
			xbees_found++;
		}
	}

	// Caso tenha algum dispositivo, atualizar a combo box
	if (xbees_found > 0)
		radio_xbee_cb.set_active(0);
}
