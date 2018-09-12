#include <gtkmm.h>
#include "CamCap.hpp"

int main(int argc, char **argv) {

	#ifdef CUDA_FOUND
		std::cout << "CUDA is enabled." << std::endl;
	#else
		std::cout << "CUDA not found." << std::endl;
	#endif

	Gtk::Main kit(argc, argv);
	Gtk::Window window;

	GdkScreen *screen = gdk_screen_get_default();


	//window.set_border_width(10);
	window.maximize();
	window.set_title("Pequi Mecânico - VSSS EMC");


	bool isLowRes = gdk_screen_get_width(screen) <= 1600;
	CamCap camcap(gdk_screen_get_width(screen), gdk_screen_get_height(screen), isLowRes);
	window.add(camcap);

	window.show_all();

	camcap.interface.visionGUI.hideGMM();
	camcap.interface.fr_camCalib_offline.hide();
	camcap.control.update_ack_interface();

	Gtk::Main::run(window);

	return EXIT_SUCCESS;
}
