//
// Created by daniel on 25/11/18.
//

#ifndef VSSS_IMAGEART_HPP
#define VSSS_IMAGEART_HPP

#include "opencv2/opencv.hpp"
#include "vision.hpp"
#include "ImageWarp.hpp"
#include "Robot2.h"
#include "TestOnClick.hpp"

namespace art {
	class ImageArt {
		private:
			bool is_warping = false;
			bool is_adjusting = false;

			int& width;
			int& height;

			const warp::PointArray& warp_mat;
			const warp::PointArray& adjust_mat;
			const onClick::TestOnClick& test_on_click;

			const cv::Scalar strategy_color = {127, 255, 127}; // green
			const cv::Scalar test_color = {250, 28, 250}; // pink
			const cv::Scalar adjust_color = {191, 5, 30};
			const cv::Scalar warp_color = {52, 168, 3};
			const cv::Scalar our_robots_color[2] = {{255, 255, 0}, {255, 0, 0}};
			const cv::Scalar ball_color = {255, 255, 255}; // white
			const cv::Scalar adv_robots_color = {0, 0, 255}; // blue
			const cv::Scalar gmm_color = {0, 255, 255};


			void draw_targets(const Robot2 *robot, cv::Mat &frame);

		public:
			ImageArt(warp::PointArray &warp, warp::PointArray &adjust, onClick::TestOnClick &test, int &width, int &height);
			void draw(cv::Mat &frame, const std::vector<cv::Point> &gmm_points, const vision::Vision::Ball &ball,
					  const std::map<unsigned int, vision::Vision::RecognizedTag> &our_tags,
					  const std::vector<cv::Point> &adv_tags,
					  const std::array<Robot2 *, 3> &our_robots, bool is_game_on);

			void set_is_warping(bool warping = true) { is_warping = warping; };

			void set_is_adjusting(bool adjusting = true) { is_adjusting = adjusting; };
	};
}

#endif //VSSS_IMAGEART_HPP
