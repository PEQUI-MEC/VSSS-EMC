//
// Created by daniel on 25/11/18.
//

#ifndef VSSS_IMAGEART_HPP
#define VSSS_IMAGEART_HPP

#include "opencv2/opencv.hpp"
#include "vision.hpp"
#include "ImageWarp.hpp"
#include "Robot2.h"

namespace art {

	class ImageArt {
		private:
			bool is_warping = false;
			bool is_adjusting = false;

			const warp::PointArray *warp_mat = nullptr;
			const warp::PointArray *adjust_mat = nullptr;

		public:
			void draw(cv::Mat &frame, const std::vector<cv::Point> &gmm_points, const vision::Vision::Ball &ball,
					  const std::map<unsigned int, vision::Vision::RecognizedTag> &our_tags,
					  const std::vector<cv::Point> &adv_tags,
					  const std::array<Robot2 *, 3> &our_robots, bool is_game_on);

			void init(const warp::PointArray &warp, const warp::PointArray &adjust);

			void set_is_warping(bool warping = true) { is_warping = warping; };

			void set_is_adjusting(bool adjusting = true) { is_adjusting = adjusting; };
	};
}

#endif //VSSS_IMAGEART_HPP
