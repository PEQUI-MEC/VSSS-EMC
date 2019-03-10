//
// Created by daniel on 17/11/18.
//

#include "ImageWarp.hpp"

using namespace warp;

bool PointArray::add_point(const cv::Point &point) {
	if (counter < MAX_POINTS) {
		mat.at(counter) = point;
		unordered_mat.at(counter) = point;
		counter++;

		if (is_full())
			std::sort(mat.begin(), mat.end(), [](cv::Point a, cv::Point b) {
				return (sqrt(pow(a.x, 2) + pow(a.y, 2)) <= sqrt(pow(b.x, 2) + pow(b.y, 2)));
			});
		return true;
	}
	return false;
}

void PointArray::undo() {
	if (counter == MAX_POINTS) {
		counter--;
		mat = unordered_mat;
	} else if (counter > 0) {
		counter--;
	}
}

cv::Point PointArray::replace(cv::Point old_pt, cv::Point new_pt) {
	cv::Point* replace_pt = std::find(std::begin(mat), std::end(mat), old_pt);
	if (replace_pt != std::end(mat)) {
		*replace_pt = new_pt;
		replace_pt = std::find(std::begin(unordered_mat), std::end(unordered_mat), old_pt);
		*replace_pt = new_pt;
		return new_pt;
	}
	return old_pt;
};

void ImageWarp::set_offset_R(const unsigned short offset) {
	offset_R = offset;
}

void ImageWarp::set_offset_L(const unsigned short offset) {
	offset_L = offset;
}

bool ImageWarp::add_mat_point(const cv::Point point, const bool isAdjust) {
	if (isAdjust) {
		return adjust_mat.add_point(point);
	} else {
		return warp_mat.add_point(point);
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
