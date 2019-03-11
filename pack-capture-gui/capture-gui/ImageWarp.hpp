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
			std::array<cv::Point, MAX_POINTS> mat;
			std::array<cv::Point, MAX_POINTS> unordered_mat;
			unsigned short counter = 0;

		public:
			cv::Point operator[](const unsigned short index) { return mat.at(index); };

			const PointArray *operator->() { return this; };

			cv::Point at(unsigned short index) const { return mat.at(index); };

			// unordered_at é usado para renderizar os pontos na ordem coletada pelo usuário na imagem
			cv::Point unordered_at(unsigned short index) const { return unordered_mat.at(index); };

			void reset() { counter = 0; };

			bool is_full() const { return counter == MAX_POINTS; };
			bool add_point(const cv::Point &point);
			void undo();
			cv::Point replace(cv::Point old_pt, cv::Point new_pt);

			unsigned short size() const { return counter; };
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
			void set_offset_R(unsigned short offset);
			void set_offset_L(unsigned short offset);

			unsigned short get_offset_R() const { return offset_R; };

			unsigned short get_offset_L() const { return offset_L; };
			bool add_mat_point(cv::Point point, bool isAdjust = false);
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
