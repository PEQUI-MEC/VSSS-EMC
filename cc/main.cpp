/*
 * main.cpp
 *
 *  Created on: Feb 19, 2015
 *      Author: gustavo
 */


/**
 * incluir: "/usr/include/gtkmm-3.0;/usr/lib/x86_64-linux-gnu/gtkmm-3.0/include;/usr/include/atkmm-1.6;/usr/include/gtk-3.0/unix-print;/usr/include/gdkmm-3.0;/usr/lib/x86_64-linux-gnu/gdkmm-3.0/include;/usr/include/giomm-2.4;/usr/lib/x86_64-linux-gnu/giomm-2.4/include;/usr/include/pangomm-1.4;/usr/lib/x86_64-linux-gnu/pangomm-1.4/include;/usr/include/glibmm-2.4;/usr/lib/x86_64-linux-gnu/glibmm-2.4/include;/usr/include/gtk-3.0;/usr/include/at-spi2-atk/2.0;/usr/include/at-spi-2.0;/usr/include/dbus-1.0;/usr/lib/x86_64-linux-gnu/dbus-1.0/include;/usr/include/gtk-3.0;/usr/include/gio-unix-2.0/;/usr/include/mirclient;/usr/include/mircommon;/usr/include/cairo;/usr/include/pango-1.0;/usr/include/harfbuzz;/usr/include/pango-1.0;/usr/include/atk-1.0;/usr/include/cairo;/usr/include/cairomm-1.0;/usr/lib/x86_64-linux-gnu/cairomm-1.0/include;/usr/include/cairo;/usr/include/pixman-1;/usr/include/freetype2;/usr/include/libpng12;/usr/include/sigc++-2.0;/usr/lib/x86_64-linux-gnu/sigc++-2.0/include;/usr/include/gdk-pixbuf-2.0;/usr/include/libpng12;/usr/include/glib-2.0;/usr/lib/x86_64-linux-gnu/glib-2.0/include"
 */

#include <gtkmm.h>
#include "CamCap.hpp"


int main(int argc, char **argv) {

    Gtk::Main kit(argc, argv);
    Gtk::Window window;

    GdkScreen *screen = gdk_screen_get_default();


    //window.set_border_width(10);
    window.maximize();
    window.set_title("Pequi Mecânico - VSSS EMC");


    CamCap camcap(gdk_screen_get_width(screen), gdk_screen_get_height(screen));
    window.add(camcap);

    window.show_all();


    camcap.interface.visionGUI.hideGMM();

    Gtk::Main::run(window);

    return EXIT_SUCCESS;
}
