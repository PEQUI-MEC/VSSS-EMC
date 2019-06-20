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
#include <iostream>
#include "opencv2/opencv.hpp"
#include <ImageWarp.hpp>
#include <ImageArt.hpp>
#include <TestOnClick.hpp>

#pragma once

namespace capture {
	class ImageView;
}

class capture::ImageView : public Gtk::DrawingArea {
		Glib::RefPtr<Gdk::Pixbuf> pb;

		onClick::TestOnClick* test_on_click;

		// warp/adjust selected point on click
		const cv::Point no_point = cv::Point(-1, -1);
		cv::Point selected_pt;

		bool has_right_click;

		bool on_button_press_event(GdkEventButton *event) override;
		bool on_button_release_event(GdkEventButton *event) override;
		bool on_motion_notify_event(GdkEventMotion* event) override;

		void select_point(gdouble x, gdouble y, bool is_warp = true);
		bool is_point_selected() const { return selected_pt != no_point; };
	public:

		unsigned char *data;
		int width;
		int height;
		int stride;

		warp::ImageWarp imageWarp;
		art::ImageArt imageArt;

		int gmm_clicks[2][2];

		int gmm_counter = 0;
		bool warp_event_flag = false;
		bool adjust_event_flag = false;
		bool gmm_sample_flag = false;
		bool gmm_ready_flag = false;
		bool split_flag = false;
		int sector = -1;

		explicit ImageView(onClick::TestOnClick &test_controller);
		void set_data(unsigned char *i_data);
		void set_frame_size(int width, int height);
		void disable_image_show();
		void refresh();
		bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
};

#endif /* IMAGEVIEW_HPP_ */
