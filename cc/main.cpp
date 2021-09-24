#include <gtkmm.h>
#include "CamCap.hpp"

void init_python() {
	Py_Initialize();

	char path[PATH_MAX];
	ssize_t size = readlink("/proc/self/exe", path, PATH_MAX);

	auto exec_path = std::string(path, (ulong) size);
	auto dir_end = exec_path.find_last_of('/');
	auto dir = exec_path.substr(0, dir_end) + "/python";
	std::cout << "Python dir: " << dir << std::endl;

	auto py_dir = PyUnicode_FromString(dir.c_str());
	auto sys_path = PySys_GetObject("path");
	PyList_Append(sys_path, py_dir);
}

int main(int argc, char **argv) {

	#ifdef CUDA_FOUND
		std::cout << "CUDA is enabled." << std::endl;
	#else
		std::cout << "CUDA not found." << std::endl;
	#endif

	init_python();

	Gtk::Main kit(argc, argv);
	Gtk::Window window;

	window.set_title("Pequi Mecânico - VSSS EMC");

	auto screen_width = Gdk::screen_width();
	bool isLowRes = screen_width <= 1920;
//	bool isLowRes = true;

	CamCap camcap(isLowRes);
	window.add(camcap);

	window.show_all();

	camcap.interface.fr_camCalib_offline.hide();
	camcap.interface.controlGUI.update_ack_interface();

	window.maximize();

	Gtk::Main::run(window);

	//Py_Finalize();

	return EXIT_SUCCESS;
}
