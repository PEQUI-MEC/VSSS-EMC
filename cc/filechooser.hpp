#ifndef FILECHOOSER_HPP_
#define FILECHOOSER_HPP_

#include <gtkmm.h>

class FileChooser : public Gtk::Window
{
	// Constructor and destructor
public:
	FileChooser();
	virtual					~FileChooser();
	std::string fileName;
	int result;

protected:


	//Member widgets:
	Gtk::Button             buttonLoad;
	Gtk::Button             buttonQuit;
	Gtk::Grid               mainGrid;
};










#endif /* FILECHOOSER_HPP_ */
