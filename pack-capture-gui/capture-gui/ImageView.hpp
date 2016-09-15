/*
 * ImageView.hpp
 *
 *  Created on: Feb 3, 2014
 *      Author: gustavo
 */

#ifndef IMAGEVIEW_HPP_
#define IMAGEVIEW_HPP_
	
#include <gtkmm.h>
#include <cairomm/context.h>
#pragma once
namespace capture {

	class ImageView: public Gtk::DrawingArea {
		
		virtual bool on_button_press_event(GdkEventButton *event){
		if(warp_event_flag){
			//cerr <<"EVENT"<<endl;
			if (event->button == 1)
			{
	
				//cerr << event->x <<"	"<< event->y<< endl;
				warp_mat[warp_counter][0] =  event->x;
				warp_mat[warp_counter][1] =  event->y;
				warp_counter++;
				if(warp_counter==4){
				//	cerr <<"COUNTER END"<<endl;
				warp_event_flag = false;
				hold_warp = true;
				warp_counter=0;
			}
				return true;
			}
          
					return true;
		}
		if(adjust_event_flag){
			if (event->button == 1)
			{
				adj_counter++;
			//cerr << event->x <<"	"<< event->y<< endl;
			cerr << adj_counter << endl;
			if(event->x < width/2){
				if(event->y < height/2){ 
				//cerr <<"ESQUERDA SUPERIOR"<<endl;
				adjust_mat[0][0] = event->x;
				adjust_mat[0][1] = event->y;
				}else{
				//cerr <<"ESQUERDA INFERIOR"<<endl;
				adjust_mat[1][0] = event->x;
				adjust_mat[1][1] = event->y;
				}
			}else{
				if(event->y < height/2){ 
				//cerr <<"DIREITA SUPERIOR"<<endl;
				adjust_mat[2][0] = event->x;
				adjust_mat[2][1] = event->y;
				}else{
				//cerr <<"DIREITA INFERIOR"<<endl;
				adjust_mat[3][0] = event->x;
				adjust_mat[3][1] = event->y;
				}
				}
				if(adj_counter==4){
				adjust_rdy = true;
				adjust_event_flag=false;
				//cerr << "ADJ END"<< endl;
				}
			}
		}
}



			Glib::RefPtr<Gdk::Pixbuf> pb;
			
			
		public:
		unsigned char * data;
			int width, height, stride;
			int warp_mat[4][2];
			int adjust_mat[4][2];
			int warp_counter =0;
			int adj_counter =0;
		    bool warp_event_flag = false;
		    bool adjust_event_flag = false;
		    bool hold_warp = false;
		    bool adjust_rdy = false;
			ImageView() :
					data(0), width(0), height(0), stride(0) {
			}

			void set_data(unsigned char * data, int width, int height) {
				this->data = data;
				this->width = width;
				this->height = height;
				this->stride = width * 3;
			}
			unsigned char * get_data() {
				return this->data;
			}
			int get_width() {
				return this->width;
			}

			int get_height() {
				return this->height;
			}


			void disable_image_show() {
				data = 0;
				width = 0;
				height = 0;
				stride = 0;
			}

			void refresh() {
				this->queue_draw();
			}

			virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
				add_events(Gdk::BUTTON_PRESS_MASK); 
				if (!data) return false;

				pb = Gdk::Pixbuf::create_from_data(data, Gdk::COLORSPACE_RGB, false, 8, width, height, stride);
				Gdk::Cairo::set_source_pixbuf(cr, pb, 0, 0);
				cr->paint();

//				cr->set_line_width(5.0);
//				cr->set_source_rgb(1, 0, 0);
//				cr->move_to(0, 0);
//				cr->line_to(width, height);
//				cr->stroke();
//
//				// http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
//				Pango::FontDescription font;
//				font.set_family("Monospace");
//				font.set_weight(Pango::WEIGHT_BOLD);
//				// http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
//				Glib::RefPtr<Pango::Layout> layout = create_pango_layout("Hi there!");
//				layout->set_font_description(font);
//				int text_width;
//				int text_height;
//				//get the text dimensions (it updates the variables -- by reference)
//				layout->get_pixel_size(text_width, text_height);
//				// Position the text in the middle
//				cr->move_to(10, 10);
//				layout->show_in_cairo_context(cr);

				return true;
			}

	};

}

#endif /* IMAGEVIEW_HPP_ */
