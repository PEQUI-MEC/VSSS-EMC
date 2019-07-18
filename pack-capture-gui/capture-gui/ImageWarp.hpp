//
// Created by daniel on 17/11/18.
//

#ifndef VSSS_IMAGEWARPER_HPP
#define VSSS_IMAGEWARPER_HPP

#include "opencv2/opencv.hpp"

namespace warp {

	const short MAX_POINTS = 4;

	class PointArray {
		private:
			// Índice do array = quadrante do ponto na imagem:
			// [0] - Segundo quadrante
			// [1] - Terceiro quadrante
			// [2] - Primeiro quadrante
			// [3] - Quarto quadrante
			// Pair: <Ponto, ordem de clique>
			std::array<std::pair<cv::Point, int>, MAX_POINTS> mat;

			int order = 0;
			int &width;
			int &height;

			const cv::Point EMPTY_POINT = {-10, -10};

			void do_add(int index, const cv::Point &point);

		public:
			PointArray(int &w, int &h);
			cv::Point operator[](const unsigned short index) { return mat.at(index).first; };

			const PointArray *operator->() { return this; };

			cv::Point at(unsigned short index) const { return mat.at(index).first; };

			std::vector<cv::Point> get_ordered_points() const;

			void reset();

			bool is_full() const { return order == MAX_POINTS; };
			void add(const cv::Point &point);
			void undo();
			cv::Point replace(const cv::Point& old_pt, const cv::Point& new_pt);

			unsigned short size() const { return order; };
	};

	class ImageWarp {
		private:
			unsigned short offset_L = 0;
			unsigned short offset_R = 0;
			PointArray warp_mat;
			PointArray adjust_mat;
			bool is_warp_rdy = false;
			bool is_adjust_rdy = false;
			bool is_invert_field = false;

		public:
			ImageWarp(int &w, int &h);

			void set_offset_R(unsigned short offset);
			void set_offset_L(unsigned short offset);

			unsigned short get_offset_R() const { return offset_R; };

			unsigned short get_offset_L() const { return offset_L; };
			void add_mat_point(const cv::Point &point, const bool isAdjust = false);
			void clear_all_points();
			void clear_warp_points();
			void clear_adjust_points();

			void run(cv::Mat &frame);

			bool is_adjust_ready() const { return is_adjust_rdy; };

			bool is_warp_ready() const { return is_warp_rdy; };

			bool set_warp_ready(bool is_rdy = true);
			bool set_adjust_ready(bool is_rdy = true);
			void set_invert_field(bool is_inverted = true);

			void warp_undo() { warp_mat.undo(); };

			void adjust_undo() { adjust_mat.undo(); };

			PointArray &get_warp() { return warp_mat; };

			PointArray &get_adjust() { return adjust_mat; };
	};
}

#endif //VSSS_IMAGEWARPER_HPP
