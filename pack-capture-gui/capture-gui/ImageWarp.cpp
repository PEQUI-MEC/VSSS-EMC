//
// Created by daniel on 17/11/18.
//

#include "ImageWarp.hpp"

using namespace warp;

void PointArray::add(const cv::Point &point) {
	if (is_full())
		return;

	if (point.x <= width / 2 && point.y <= height / 2) {
		do_add(0, point);
	} else if (point.x <= width / 2 && point.y > height / 2) {
		do_add(1, point);
	} else if (point.x > width / 2 && point.y <= height / 2) {
		do_add(2, point);
	} else {
		do_add(3, point);
	}
}

void PointArray::do_add(int index, const cv::Point &point) {
	if (mat.at(index).first != warp::PointArray::EMPTY_POINT) {
		mat.at(index).first = point;
	} else {
		mat.at(index) = {point, order++};
	}
}

void PointArray::undo() {
	auto* find_pair = std::find_if(std::begin(mat), std::end(mat), [this](std::pair<cv::Point, int> pair) {
		return order-1 == pair.second;
	});

	if (find_pair != std::end(mat))
		*find_pair = {EMPTY_POINT, -1};

	if (order > 0) {
		order--;
	}
}

cv::Point PointArray::replace(const cv::Point& old_pt, const cv::Point& new_pt) {
	if (!is_full())
		return old_pt;

	auto* replace_pair = std::find_if(std::begin(mat), std::end(mat), [&old_pt](std::pair<cv::Point, int> pair) {
		return (old_pt.x == pair.first.x && old_pt.y == pair.first.y);
	});

	if (replace_pair != std::end(mat)) {
		replace_pair->first = new_pt;
		return new_pt;
	}

	return old_pt;
}

void PointArray::reset() {
	order = 0;
	for (auto &pair : mat) {
		pair = {EMPTY_POINT, -1};
	}
}

PointArray::PointArray(int &w, int &h) : width(w), height(h) {
	std::fill(std::begin(mat), std::end(mat), std::make_pair(EMPTY_POINT, -1));
}

std::vector<cv::Point> PointArray::get_ordered_points() const {
	std::vector<cv::Point> vec;
	int index = 0;
	while (index < order) {
		auto pair = std::find_if(std::begin(mat), std::end(mat), [index, this](auto pair) {
			return pair.first != EMPTY_POINT && index == pair.second;
		});
		vec.emplace_back(pair->first);
		index++;
	}

	return vec;
}

void ImageWarp::set_offset_R(const unsigned short offset) {
	offset_R = offset;
}

void ImageWarp::set_offset_L(const unsigned short offset) {
	offset_L = offset;
}

void ImageWarp::add_mat_point(const cv::Point &point, const bool isAdjust) {
	if (isAdjust) {
		adjust_mat.add(point);
	} else {
		warp_mat.add(point);
	}
}

void ImageWarp::run(cv::Mat &frame) {
	if (is_warp_rdy) {
		// Faz o warp
		cv::Point2f inputQuad[MAX_POINTS];
		cv::Point2f outputQuad[MAX_POINTS];
		cv::Mat lambda = cv::Mat::zeros(frame.rows, frame.cols, frame.type());

		inputQuad[0] = cv::Point2f(warp_mat[0].x - offset_L, warp_mat[0].y);
		inputQuad[1] = cv::Point2f(warp_mat[2].x + offset_R, warp_mat[2].y);
		inputQuad[2] = cv::Point2f(warp_mat[3].x + offset_R, warp_mat[3].y);
		inputQuad[3] = cv::Point2f(warp_mat[1].x - offset_L, warp_mat[1].y);

		outputQuad[0] = cv::Point2f(0, 0);
		outputQuad[1] = cv::Point2f(frame.cols - 1, 0);
		outputQuad[2] = cv::Point2f(frame.cols - 1, frame.rows - 1);
		outputQuad[3] = cv::Point2f(0, frame.rows - 1);
		lambda = getPerspectiveTransform(inputQuad, outputQuad);
		warpPerspective(frame, frame, lambda, frame.size());
	}

	if (is_adjust_rdy) {
		// Faz o ajuste dos cantos
		cv::rectangle(frame, {0, 0}, adjust_mat[0], cv::Scalar(0,0,0), cv::FILLED);
		cv::rectangle(frame, {0, frame.rows}, adjust_mat[1], cv::Scalar(0,0,0), cv::FILLED);
		cv::rectangle(frame, {frame.cols, 0}, adjust_mat[2], cv::Scalar(0,0,0), cv::FILLED);
		cv::rectangle(frame, {frame.cols, frame.rows}, adjust_mat[3], cv::Scalar(0,0,0), cv::FILLED);
	}

	if (is_invert_field) {
		// Inverte o campo
		cv::flip(frame, frame, -1);
	}
}

bool ImageWarp::set_warp_ready(bool is_rdy) {
	if ((is_rdy && warp_mat.is_full()) || !is_rdy) {
		is_warp_rdy = is_rdy;
		return true;
	}
	return false;
}

bool ImageWarp::set_adjust_ready(bool is_rdy) {
	if ((is_rdy && adjust_mat.is_full()) || !is_rdy) {
		is_adjust_rdy = is_rdy;
		return true;
	}
	return false;
}

void ImageWarp::set_invert_field(bool is_inverted) {
	is_invert_field = is_inverted;
}

void ImageWarp::clear_all_points() {
	warp_mat.reset();
	adjust_mat.reset();
	is_warp_rdy = false;
	is_adjust_rdy = false;
}

void ImageWarp::clear_warp_points() {
	warp_mat.reset();
	is_warp_rdy = false;
}

void ImageWarp::clear_adjust_points() {
	adjust_mat.reset();
	is_adjust_rdy = false;
}

ImageWarp::ImageWarp(int &w, int &h) : warp_mat(w, h), adjust_mat(w, h) {
}
