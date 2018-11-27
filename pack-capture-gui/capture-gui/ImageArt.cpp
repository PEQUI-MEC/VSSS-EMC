//
// Created by daniel on 25/11/18.
//

#include "ImageArt.hpp"

using namespace vision;
using namespace art;

void ImageArt::draw(cv::Mat &frame, const std::vector<cv::Point> &gmm_points, const vision::Vision::Ball &ball,
					const std::map<unsigned int, Vision::RecognizedTag> &our_tags,
					const std::vector<cv::Point> &adv_tags,
					const std::array<Robot2 *, 3> &our_robots, bool is_game_on) {

	// Adversary Robots
	for (const auto &robot : adv_tags) {
		circle(frame, robot, 15, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
	}

	// Ball
	if (ball.isFound)
		circle(frame, ball.position, 7, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);

	// Our Robots
	for (const auto &tag : our_tags) {
		circle(frame, tag.second.position, 15, cv::Scalar(255, 255, 0), 2, cv::LINE_AA);
		putText(frame, std::to_string(tag.first + 1),
				cv::Point(tag.second.position.x + 13, tag.second.position.y - 15),
				cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 0), 2, cv::LINE_AA);
		// linha da orientação do robô
		line(frame, tag.second.rear_point, tag.second.front_point, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
	}

	// Strategy targets
	if (is_game_on) {
		for (auto robot : our_robots) {
			switch (robot->get_command()) {
				case Robot2::Command::Vector:
					BOOST_FALLTHROUGH;
				case Robot2::Command::Orientation: {
					double angle = robot->get_target().orientation;
					cv::Point position = robot->get_position().to_cv_point();
					auto x2 = static_cast<int>(position.x + 16 * cos(angle));
					auto y2 = static_cast<int>(position.y - 16 * sin(angle));
					line(frame, position, cv::Point(x2, y2),
						 cv::Scalar(127, 255, 127), 3, cv::LINE_AA);
					break;
				}
				case Robot2::Command::None:
					putText(frame, "x", robot->get_position().to_cv_point(), cv::FONT_HERSHEY_PLAIN, 1,
							cv::Scalar(127, 255, 127), 2, cv::LINE_AA);
					break;
				case Robot2::Command::UVF: {
					double angle = robot->get_target().orientation;
					cv::Point target = robot->get_target().position.to_cv_point();
					auto x2 = static_cast<int>(target.x + 16 * cos(angle));
					auto y2 = static_cast<int>(target.y - 16 * sin(angle));
					circle(frame, target, 7, cv::Scalar(127, 255, 127), 2);
					line(frame, target, cv::Point(x2, y2),
						 cv::Scalar(127, 255, 127), 3, cv::LINE_AA);
					putText(frame, std::to_string(robot->tag + 1),
							cv::Point(target.x - 5, target.y - 17),
							cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(127, 255, 127), 2, cv::LINE_AA);
					break;
				}
				default:
					cv::Point robot_target = robot->get_target().position.to_cv_point();
					circle(frame, robot_target, 7, cv::Scalar(127, 255, 127), 2, cv::LINE_AA);
					putText(frame, std::to_string(robot->tag + 1),
							cv::Point(robot_target.x - 5, robot_target.y - 17),
							cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(127, 255, 127), 2, cv::LINE_AA);
			} // switch
		} // for
	}

	// GMM draw points
	for (unsigned long i = 0; i < gmm_points.size(); i = i + 2) {
		cv::rectangle(frame, gmm_points.at(i), gmm_points.at(i + 1), cv::Scalar(0, 255, 255), cv::LINE_AA);
	}

	// Warp
	if (is_warping) {
		auto color = cv::Scalar(52, 168, 3);
		auto scale = 1;
		auto thickness = 2;
		auto font = cv::FONT_HERSHEY_TRIPLEX;
		if (warp_mat->is_full()) {
			cv::putText(frame, "Press \'Apply\' to finish", {frame.cols / 5, frame.rows / 2},
						font, scale, color, thickness, cv::LINE_AA);
			cv::line(frame, warp_mat->at(0), warp_mat->at(1), color, thickness, cv::LINE_AA);
			cv::line(frame, warp_mat->at(1), warp_mat->at(3), color, thickness, cv::LINE_AA);
			cv::line(frame, warp_mat->at(3), warp_mat->at(2), color, thickness, cv::LINE_AA);
			cv::line(frame, warp_mat->at(2), warp_mat->at(0), color, thickness, cv::LINE_AA);
		} else {
			std::string message[2] = {"Click on the four field edges", "to collect warp points"};
			cv::Point first_line = {frame.cols / 10, frame.rows / 2};
			cv::Point second_line = {frame.cols / 10 + frame.cols / 12, frame.rows / 2 + frame.rows / 12};
			cv::putText(frame, message[0], first_line,
						font, scale, color, thickness, cv::LINE_AA);
			cv::putText(frame, message[1], second_line,
						font, scale, color, thickness, cv::LINE_AA);
		}

		for (unsigned short index = 0; index < warp_mat->get_size(); index++) {
			cv::Point point = warp_mat->unordered_at(index);
			cv::circle(frame, point, 2, color, 2);
			cv::putText(frame, std::to_string(index + 1), {point.x + 5, point.y - 5},
						cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, color, 1, cv::LINE_AA);
		}
	}

	// Adjust
	if (is_adjusting) {
		auto font = cv::FONT_HERSHEY_TRIPLEX;
		auto scale = 0.9;
		auto color = cv::Scalar(191, 5, 30);
		auto thickness = 2;
		if (adjust_mat->is_full()) {
			cv::putText(frame, "Press \'Apply\' to finish", {frame.cols / 5, frame.rows / 2},
						font, scale, color, thickness, cv::LINE_AA);
		} else {
			std::string message[2] = {"Click on the four goal corners", "points to collect adjust points"};
			cv::Point first_line = {frame.cols / 8, frame.rows / 2};
			cv::Point second_line = {frame.cols / 8, frame.rows / 2 + frame.rows / 12};
			cv::putText(frame, message[0], first_line,
						font, scale, color, thickness, cv::LINE_AA);
			cv::putText(frame, message[1], second_line,
						font, scale, color, thickness, cv::LINE_AA);
		}

		for (unsigned short index = 0; index < adjust_mat->get_size(); index++) {
			cv::Point point = adjust_mat->unordered_at(index);
			cv::circle(frame, point, 2, color, thickness);
			cv::putText(frame, std::to_string(index + 1), {point.x + 5, point.y - 5},
						cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, color, 1, cv::LINE_AA);
		}
	}
}

void ImageArt::init(const warp::PointArray &warp, const warp::PointArray &adjust) {
	warp_mat = &warp;
	adjust_mat = &adjust;
}
