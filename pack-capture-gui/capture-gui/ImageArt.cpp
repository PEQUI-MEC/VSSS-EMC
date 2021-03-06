//
// Created by daniel on 25/11/18.
//

#include "ImageArt.hpp"

using namespace vision;
using namespace art;
using namespace onClick;

void ImageArt::draw(cv::Mat &frame, const vision::Vision::Ball &ball,
					const std::map<unsigned int, Vision::RecognizedTag> &our_tags,
					const std::vector<cv::Point> &adv_tags,
					const std::array<Robot2 *, 3> &our_robots, bool is_game_on) {

	// Adversary Robots
	for (const auto &robot : adv_tags) {
		circle(frame, robot, 15, adv_robots_color, 2, cv::LINE_AA);
	}

	// Ball
	if (ball.isFound)
		circle(frame, ball.position, 7, ball_color, 2, cv::LINE_AA);

	// Our Robots
	for (const auto &tag : our_tags) {
		circle(frame, tag.second.position, 15, our_robots_color[0], 2, cv::LINE_AA);
		putText(frame, std::to_string(tag.first + 1),
				cv::Point(tag.second.position.x + 13, tag.second.position.y - 15),
				cv::FONT_HERSHEY_PLAIN, 1, our_robots_color[0], 2, cv::LINE_AA);
		// linha da orientação do robô
		line(frame, tag.second.rear_point, tag.second.front_point, our_robots_color[1], 2, cv::LINE_AA);
	}

	// Strategy targets
	if (is_game_on) {
		for (auto robot : our_robots) {
			draw_targets(robot, frame);
		}
	}

	// Test On Click
	if (test_on_click.is_active()) {
		Robot2* robot = test_on_click.get_selected_robot();
		draw_targets(robot, frame);

		if (robot != nullptr)
			cv::circle(frame, robot->get_position().to_cv_point(), 13, test_color, 2, cv::LINE_AA);
	}

	// Warp
	if (is_warping) {
		auto scale = 1;
		auto thickness = 2;
		auto font = cv::FONT_HERSHEY_TRIPLEX;
		if (warp_mat.is_full()) {
			cv::putText(frame, "Press \'Apply\' to finish", {frame.cols / 5, frame.rows / 2},
						font, scale, warp_color, thickness, cv::LINE_AA);
			cv::line(frame, warp_mat.at(0), warp_mat.at(1), warp_color, thickness, cv::LINE_AA);
			cv::line(frame, warp_mat.at(1), warp_mat.at(3), warp_color, thickness, cv::LINE_AA);
			cv::line(frame, warp_mat.at(3), warp_mat.at(2), warp_color, thickness, cv::LINE_AA);
			cv::line(frame, warp_mat.at(2), warp_mat.at(0), warp_color, thickness, cv::LINE_AA);
		} else {
			std::string message[2] = {"Click on the four field edges", "to collect warp points"};
			cv::Point first_line = {frame.cols / 10, frame.rows / 2};
			cv::Point second_line = {frame.cols / 10 + frame.cols / 12, frame.rows / 2 + frame.rows / 12};
			cv::putText(frame, message[0], first_line,
						font, scale, warp_color, thickness, cv::LINE_AA);
			cv::putText(frame, message[1], second_line,
						font, scale, warp_color, thickness, cv::LINE_AA);
		}

		auto points = warp_mat.get_ordered_points();
		int index = 0;
		for (const auto& point : points) {
			cv::circle(frame, point, 2, warp_color, 2);
			cv::putText(frame, std::to_string(index++ + 1), {point.x + 5, point.y - 5},
						cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, warp_color, 1, cv::LINE_AA);
		}
	}

	// Adjust
	if (is_adjusting) {
		auto font = cv::FONT_HERSHEY_TRIPLEX;
		auto scale = 0.9;
		auto thickness = 2;
		if (adjust_mat.is_full()) {
			cv::putText(frame, "Press \'Apply\' to finish", {frame.cols / 5, frame.rows / 2},
						font, scale, adjust_color, thickness, cv::LINE_AA);
		} else {
			std::string message[2] = {"Click on the four goal corners", "points to collect adjust points"};
			cv::Point first_line = {frame.cols / 8, frame.rows / 2};
			cv::Point second_line = {frame.cols / 8, frame.rows / 2 + frame.rows / 12};
			cv::putText(frame, message[0], first_line,
						font, scale, adjust_color, thickness, cv::LINE_AA);
			cv::putText(frame, message[1], second_line,
						font, scale, adjust_color, thickness, cv::LINE_AA);
		}


		auto points = adjust_mat.get_ordered_points();
		int index = 0;
		for (const auto& point : points) {
			// desenhar retầngulos
			if (point.x <= width / 2 && point.y <= height / 2) {
				cv::line(frame, {point.x, 0}, point, adjust_color, 1, cv::LINE_AA);
				cv::line(frame, {0, point.y}, point, adjust_color, 1, cv::LINE_AA);
			} else if (point.x <= width / 2 && point.y > height / 2) {
				cv::line(frame, {point.x, height}, point, adjust_color, 1, cv::LINE_AA);
				cv::line(frame, {0, point.y}, point, adjust_color, 1, cv::LINE_AA);
			} else if (point.x > width / 2 && point.y <= height / 2) {
				cv::line(frame, {point.x, 0}, point, adjust_color, 1, cv::LINE_AA);
				cv::line(frame, {width, point.y}, point, adjust_color, 1, cv::LINE_AA);
			} else {
				cv::line(frame, {point.x, height}, point, adjust_color, 1, cv::LINE_AA);
				cv::line(frame, {width, point.y}, point, adjust_color, 1, cv::LINE_AA);
			}

			cv::circle(frame, point, 2, adjust_color, thickness);

			cv::putText(frame, std::to_string(index++ + 1), {point.x + 5, point.y - 5},
						cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, adjust_color, 1, cv::LINE_AA);
		}
	}
}

void ImageArt::draw_targets(const Robot2 *robot, cv::Mat &frame) {
	if (robot == nullptr)
		return;

	cv::Scalar color;
	double angle;
	cv::Point target;
	cv::Point position = robot->get_position().to_cv_point();
	bool is_test_on_click = test_on_click.is_active();

	if (is_test_on_click) {
		color = test_color;
		angle = test_on_click.get_orientation_value();
		target = test_on_click.get_target().to_cv_point();
	} else {
		color = strategy_color;
		angle = robot->get_target().orientation;
		robot->get_target().position.to_cv_point();
	}

	switch (is_test_on_click? test_on_click.get_command() : robot->get_command()) {
		case Robot2::Command::Vector:
			[[fallthrough]];
		case Robot2::Command::Orientation: {
			auto x2 = static_cast<int>(position.x + 16 * cos(angle));
			auto y2 = static_cast<int>(position.y - 16 * sin(angle));
			line(frame, position, cv::Point(x2, y2),
				 color, 3, cv::LINE_AA);
			break;
		}
		case Robot2::Command::None:
			putText(frame, "x", position, cv::FONT_HERSHEY_PLAIN, 1,
					color, 2, cv::LINE_AA);
			break;
		case Robot2::Command::UVF: {
			auto x2 = static_cast<int>(target.x + 16 * cos(angle));
			auto y2 = static_cast<int>(target.y - 16 * sin(angle));
			circle(frame, target, 7, color, 2);
			line(frame, target, cv::Point(x2, y2),
				 color, 3, cv::LINE_AA);
			putText(frame, std::to_string(robot->tag + 1),
					cv::Point(target.x - 5, target.y - 17),
					cv::FONT_HERSHEY_PLAIN, 1, color, 2, cv::LINE_AA);
			break;
		}
		default:
			circle(frame, target, 7, color, 2, cv::LINE_AA);
			putText(frame, std::to_string(robot->tag + 1),
					cv::Point(target.x - 5, target.y - 17),
					cv::FONT_HERSHEY_PLAIN, 1, color, 2, cv::LINE_AA);
	} // switch
}

ImageArt::ImageArt(warp::PointArray &warp, warp::PointArray &adjust, onClick::TestOnClick &test, int &width, int &height)
	: warp_mat(warp),
	adjust_mat(adjust),
	test_on_click(test),
	width(width),
	height(height){

}