#include "ImageView.hpp"

using capture::ImageView;
using namespace onClick;

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
		if (event->button == 1) {
			select_point(event->x, event->y);
			if (!is_point_selected())
				warp_event_flag = imageWarp.add_mat_point({static_cast<int>(event->x), static_cast<int>(event->y)});
		} else if (event->button == 3)
			imageWarp.warp_undo();
	}

	if (adjust_event_flag) {
		if (event->button == 1) {
			select_point(event->x, event->y, false);
			if (!is_point_selected())
				adjust_event_flag = imageWarp.add_mat_point({static_cast<int>(event->x), static_cast<int>(event->y)}, true);
		} else if (event->button == 3)
			imageWarp.adjust_undo();
	}

	if (test_on_click->is_active()) {
		if (event->button == 1) {
			test_on_click->select_robot(event->x, event->y);
		} else if (event->button == 3) {
			test_on_click->set_ready(false);
			has_right_click = true;
			test_on_click->select_target(event->x, event->y);
		}
	}
	return true;
}

ImageView::ImageView(TestOnClick &test_controller) : test_on_click(&test_controller),
													 has_right_click(false),
													 data(nullptr),
													 width(0),
													 height(0),
													 stride(0),
													 imageArt(imageWarp.get_warp(),
													 		  imageWarp.get_adjust(),
															  test_controller,
															  width,
															  height),
												      gmm_clicks{{0, 0}, {0, 0}}
												      {
}

void ImageView::set_data(unsigned char *data, int width, int height) {
	this->data = data;
	this->width = width;
	this->height = height;
	this->stride = width * 3;
}

void ImageView::disable_image_show() {
	data = nullptr;
	width = 0;
	height = 0;
	stride = 0;
}

void ImageView::refresh() {
	this->queue_draw();
}

bool ImageView::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
	add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK);
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

bool capture::ImageView::on_motion_notify_event(GdkEventMotion *event) {
	if (is_point_selected()) {
		const auto x = static_cast<int>(event->x);
		const auto y = static_cast<int>(event->y);
		if (warp_event_flag) {
			warp::PointArray& warp_mat = imageWarp.get_warp();
			selected_pt = warp_mat.replace(selected_pt, cv::Point(x, y));
			return true;
		} else if (adjust_event_flag) {
			warp::PointArray& adjust_mat = imageWarp.get_adjust();
			selected_pt = adjust_mat.replace(selected_pt, cv::Point(x, y));
			return true;
		}
	}

	if (Robot2::Command cmd = test_on_click->get_command();
			has_right_click && (cmd == Robot2::Command::UVF || cmd == Robot2::Command::Orientation)) {

		test_on_click->set_orientation(event->x, event->y);
		return true;
	} else if (has_right_click && (cmd == Robot2::Command::Position || cmd == Robot2::Command::Vector)) {
		test_on_click->select_target(event->x, event->y);
		return true;
	}
	return false;
}

bool capture::ImageView::on_button_release_event(GdkEventButton *event) {
	if ((adjust_event_flag || warp_event_flag) && event->button == 1) {
		selected_pt = no_point;
		return true;
	}
	if (test_on_click->is_active() && test_on_click->has_robot() && event->button == 3) {
		has_right_click = false;
		test_on_click->set_ready();
	}
	return false;
}

void capture::ImageView::select_point(const gdouble x, const gdouble y, const bool is_warp) {
	warp::PointArray &mat = is_warp ? imageWarp.get_warp() : imageWarp.get_adjust();

	for (unsigned short index = 0; index < mat.size(); index++) {
		gdouble dist = sqrt(pow(x - mat[index].x, 2) + pow(y - mat[index].y, 2));
		if (dist < 10) { // 10 pixels
			selected_pt = mat[index];
			return;
		}
	}
	selected_pt = no_point;
}
