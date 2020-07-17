#ifndef FILECHOOSER_HPP_
#define FILECHOOSER_HPP_

#include <gtkmm.h>

class FileChooser : public Gtk::Window {
		// Constructor and destructor
	public:
		FileChooser();
		virtual                    ~FileChooser();
		std::string fileName;
		int result;
};

#endif /* FILECHOOSER_HPP_ */
