#include "filechooser.hpp"
#include <iostream>


// Constructor of the main Window (build ui interface).
FileChooser::FileChooser()
{
    // Initialize the main window and hide the title bar
    this->set_border_width(10);

    // Create the dialog box FileChooser
    Gtk::FileChooserDialog dialog("Please choose a file",Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);

    //Add response buttons the the dialog:
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Done", Gtk::RESPONSE_OK);

    //Add filters, so that only certain file types can be selected:
    Glib::RefPtr<Gtk::FileFilter> filter_text = Gtk::FileFilter::create();
    filter_text->set_name("Text files");
    filter_text->add_mime_type("text/plain");
    dialog.add_filter(filter_text);

    Glib::RefPtr<Gtk::FileFilter> filter_cpp = Gtk::FileFilter::create();
    filter_cpp->set_name("C/C++ files");
    filter_cpp->add_mime_type("text/x-c");
    filter_cpp->add_mime_type("text/x-c++");
    filter_cpp->add_mime_type("text/x-c-header");
    dialog.add_filter(filter_cpp);

    Glib::RefPtr<Gtk::FileFilter> filter_any = Gtk::FileFilter::create();
    filter_any->set_name("Any files");
    filter_any->add_pattern("*");
    dialog.add_filter(filter_any);

    //Show the dialog and wait for a user response:
    result = dialog.run();

    //Handle the response:
    switch(result)
    {
    case(Gtk::RESPONSE_OK):
    {
        // The user selected a file
        std::cout << "Open clicked." << std::endl;
        fileName = dialog.get_filename();
        std::cout << "File selected: " <<  fileName << std::endl;
        break;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
        // The user clicked cancel
        std::cout << "Cancel clicked." << std::endl;
        break;
    }
    default:
    {
        // The user closed the dialog box
        std::cout << "Unexpected button clicked." << std::endl;
        break;
    }
    }
}


// Destructor of the class
FileChooser::~FileChooser()
{}
