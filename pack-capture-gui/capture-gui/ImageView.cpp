#include "ImageView.hpp"

using capture::ImageView;

bool ImageView::on_button_press_event(GdkEventButton *event) {

	if (gmm_sample_flag && event->button == 1) {

		gmm_clicks[gmm_counter][0] = static_cast<int>(event->x);
		gmm_clicks[gmm_counter][1] = static_cast<int>(event->y);
		if (gmm_counter == 1) {
			gmm_counter = 0;
			gmm_ready_flag = true;
		} else gmm_counter++;
	}

	if (split_flag) {
		if (event->x <= std::round(width / 2) && event->y <= std::round(height / 2)) sector = 0;
		else if (event->x <= std::round(width / 2) && event->y > std::round(height / 2)) sector = 2;
		else if (event->x > std::round(width / 2) && event->y <= std::round(height / 2)) sector = 1;
		else if (event->x > std::round(width / 2) && event->y > std::round(height / 2)) sector = 3;
	}

	if (warp_event_flag) {
		if (event->button == 1)
			warp_event_flag = imageWarp.add_mat_point({static_cast<int>(event->x), static_cast<int>(event->y)});
		if (event->button == 3)
			imageWarp.warp_undo();
	}

	if (adjust_event_flag) {
		if (event->button == 1)
			adjust_event_flag = imageWarp.add_mat_point({static_cast<int>(event->x), static_cast<int>(event->y)}, true);
		if (event->button == 3)
			imageWarp.adjust_undo();
	}

	if (test_on_click->is_active()) {
		if (event->button == 1) {
			test_on_click->select_robot(event->x, event->y);
		} else if (event->button == 3) {
			test_on_click->select_target(event->x, event->y);
		}
	}
	return true;
}

ImageView::ImageView(TestOnClick &test_controller) : test_on_click(&test_controller), data(nullptr), width(0), height(0), stride(0),
	imageArt(imageWarp.get_warp(), imageWarp.get_adjust(), test_controller){
}

void ImageView::set_data(unsigned char *data, int width, int height) {
	this->data = data;
	this->width = width;
	this->height = height;
	this->stride = width * 3;
}

void ImageView::disable_image_show() {
	data = 0;
	width = 0;
	height = 0;
	stride = 0;
}

void ImageView::refresh() {
	this->queue_draw();
}

bool ImageView::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
	add_events(Gdk::BUTTON_PRESS_MASK);
	if (!data) return false;

	pb = Gdk::Pixbuf::create_from_data(data, Gdk::COLORSPACE_RGB, false, 8, width, height, stride);
	Gdk::Cairo::set_source_pixbuf(cr, pb, 0, 0);
	cr->paint();

//    				cr->set_line_width(5.0);
//    				cr->set_source_rgb(1, 0, 0);
//    				cr->move_to(0, 0);
//    				cr->line_to(width, height);
//    				cr->stroke();
//
//    				// http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
//    				Pango::FontDescription font;
//    				font.set_family("Monospace");
//    				font.set_weight(Pango::WEIGHT_BOLD);
//    				// http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
//    				Glib::RefPtr<Pango::Layout> layout = create_pango_layout("Hi there!");
//    				layout->set_font_description(font);
//    				int text_width;
//    				int text_height;
//    				//get the text dimensions (it updates the variables -- by reference)
//    				layout->get_pixel_size(text_width, text_height);
//    				// Position the text in the middle
//    				cr->move_to(10, 10);
//    				layout->show_in_cairo_context(cr);

	return true;
}
