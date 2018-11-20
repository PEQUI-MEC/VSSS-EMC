//
// Created by daniel on 17/11/18.
//

#ifndef VSSS_IMAGEWARPER_HPP
#define VSSS_IMAGEWARPER_HPP

#include "opencv2/opencv.hpp"

namespace warp {

	const short MAT_SIZE = 4;

	class ImageWarper {
		private:
			struct {
				bool operator() ( cv::Point a, cv::Point b ){
					return (sqrt(pow(a.x,2)+pow(a.y,2)) <=  sqrt(pow(b.x,2)+pow(b.y,2)));
				}
			} point_comparator;

			unsigned short warp_counter = 0;
			unsigned short adjust_counter = 0;
			unsigned short offset_L = 0;
			unsigned short offset_R = 0;
			cv::Point warp_mat[MAT_SIZE];
			cv::Point adjust_mat[MAT_SIZE];
			bool is_warp_rdy = false;
			bool is_adjust_rdy = false;
			bool is_invert_field = false;

		public:
			void set_offset_R(unsigned short offset);
			void set_offset_L(unsigned short offset);
			unsigned short get_offset_R() const { return offset_R; };
			unsigned short get_offset_L() const { return offset_L; };
			bool add_mat_point(cv::Point point, bool isAdjust = false);
			void reset_all_points();
			void clear_warp_points();
			void clear_adjust_points();

			void execute(cv::Mat &frame);

			bool is_adjust_ready() const { return is_adjust_rdy; };
			bool is_warp_ready() const { return is_warp_rdy; };

			bool set_warp_ready(bool is_rdy = true);
			bool set_adjust_ready(bool is_rdy = true);
			void set_invert_field(bool is_inverted = true);

			cv::Point* get_warp_mat() { return warp_mat; };
			cv::Point* get_adjust_mat() { return adjust_mat; };



	};
}

#endif //VSSS_IMAGEWARPER_HPP
