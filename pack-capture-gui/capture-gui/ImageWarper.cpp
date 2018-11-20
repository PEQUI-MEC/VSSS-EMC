//
// Created by daniel on 17/11/18.
//

#include "ImageWarper.hpp"

using namespace warp;

void ImageWarper::set_offset_R(const unsigned short offset) {
	offset_R = offset;
}

void ImageWarper::set_offset_L(const unsigned short offset) {
	offset_L = offset;
}

bool ImageWarper::add_mat_point(const cv::Point point, const bool isAdjust) {
	if (isAdjust) {
		if (adjust_counter < MAT_SIZE) {
			adjust_mat[adjust_counter++] = point;
			if (adjust_counter == MAT_SIZE)
				std::sort(adjust_mat, adjust_mat+MAT_SIZE, point_comparator);
			return true;
		}
	} else {
		if (warp_counter < MAT_SIZE) {
			warp_mat[warp_counter++] = point;
			if (warp_counter == MAT_SIZE)
				std::sort(warp_mat, warp_mat+MAT_SIZE, point_comparator);
			return true;
		}
	}

	return false;
}

void ImageWarper::execute(cv::Mat &frame) {
	if (is_warp_rdy) {
		// Faz o warp
		cv::Point2f inputQuad[MAT_SIZE];
		cv::Point2f outputQuad[MAT_SIZE];
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
		for (int i = 0; i < adjust_mat[0].y; i++) {
			for (int j = 0; j < 3 * adjust_mat[0].x; j++) {
				frame.at<uchar>(i, j) = 0;
			}
		}

		for (int i = frame.rows; i > adjust_mat[1].y; i--) {
			for (int j = 0; j < 3 * adjust_mat[1].x; j++) {
				frame.at<uchar>(i, j) = 0;
			}
		}

		for (int i = 0; i < adjust_mat[2].y; i++) {
			for (int j = 3 * frame.cols; j > 3 * adjust_mat[2].x; j--) {
				frame.at<uchar>(i, j) = 0;
			}
		}

		for (int i = frame.rows; i > adjust_mat[3].y; i--) {
			for (int j = 3 * frame.cols; j > 3 * adjust_mat[3].x; j--) {
				frame.at<uchar>(i, j) = 0;
			}
		}
	}

	if (is_invert_field) {
		// Inverte o campo
		cv::flip(frame, frame, -1);
	}

}

bool ImageWarper::set_warp_ready(bool is_rdy) {
	if ((is_rdy && warp_counter == MAT_SIZE) || !is_rdy) {
		is_warp_rdy = is_rdy;
		return true;
	}
	return false;
}

bool ImageWarper::set_adjust_ready(bool is_rdy) {
	if ((is_rdy && adjust_counter == MAT_SIZE) || !is_rdy) {
		is_adjust_rdy = is_rdy;
		return true;
	}
	return false;
}

void ImageWarper::set_invert_field(bool is_inverted) {
	is_invert_field = is_inverted;
}

void ImageWarper::reset_all_points() {
	warp_counter = 0;
	adjust_counter = 0;
	is_warp_rdy = false;
	is_adjust_rdy = false;
}

void ImageWarper::clear_warp_points() {
	warp_counter = 0;
	is_warp_rdy = false;
}

void ImageWarper::clear_adjust_points() {
	adjust_counter = 0;
	is_adjust_rdy = false;
}
