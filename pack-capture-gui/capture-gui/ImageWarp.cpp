//
// Created by daniel on 17/11/18.
//

#include "ImageWarp.hpp"

using namespace warp;

bool PointArray::add_point(const cv::Point& point) {
	if (counter < MAT_SIZE) {
		mat.at(counter++) = point;
		if (is_full())
			std::sort(mat.begin(), mat.end(), []( cv::Point a, cv::Point b ) {
				return (sqrt(pow(a.x,2)+pow(a.y,2)) <=  sqrt(pow(b.x,2)+pow(b.y,2)));
			});
		return true;
	}
	return false;
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

void ImageWarp::execute(cv::Mat &frame) {
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

void ImageWarp::reset_all_points() {
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
