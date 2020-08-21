//
// Created by daniel on 25/11/18.
//

#include "ImageArt.hpp"

using namespace vision;
using namespace art;
using namespace onClick;

void ImageArt::draw_with_orientation(cv::Mat &frame, const std::vector<Tag> &team_tags, RobotColor color) {
	for (unsigned i = 0; i < team_tags.size(); i++) {
		auto& tag = team_tags[i];
		auto draw_color = color == RobotColor::Yellow ? yellow_color : blue_color;
		circle(frame, tag.position, 15, draw_color, 2, cv::LINE_AA);
		putText(frame, std::to_string(i + 1),
				cv::Point(tag.position.x + 13, tag.position.y - 15),
				cv::FONT_HERSHEY_PLAIN, 1, draw_color, 2, cv::LINE_AA);
		// linha da orientação do robô
		line(frame, tag.rear_point, tag.front_point, orientation_line_color, 2, cv::LINE_AA);
	}
}

void ImageArt::draw_robots_and_ball(cv::Mat &frame, const Tags &tags) {
	if (tags.yellow_has_orientation) {
		draw_with_orientation(frame, tags.yellow, RobotColor::Yellow);
	} else {
		for (auto &robot : tags.yellow) {
			circle(frame, robot.position, 15, yellow_color, 2, cv::LINE_AA);
		}
	}

	if (tags.blue_has_orientation) {
		draw_with_orientation(frame, tags.blue, RobotColor::Blue);
	} else {
		for (auto &robot : tags.blue) {
			circle(frame, robot.position, 15, blue_color, 2, cv::LINE_AA);
		}
	}

	// Ball
	if (tags.found_ball)
		circle(frame, tags.ball.position, 7, ball_color, 2, cv::LINE_AA);
}

void ImageArt::draw(cv::Mat &frame, const Tags &tags, Team &team, bool is_game_on) {
	draw_robots_and_ball(frame, tags);

	// Strategy targets
	if (is_game_on) {
		for (auto robot : team.robots) {
			draw_targets(robot, frame, false, team.robot_color);
		}
	}

	// Test On Click
	if (test_on_click.is_active()) {
		Robot3* robot = test_on_click.get_selected_robot();
		draw_targets(*robot, frame, false, team.robot_color);

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

void ImageArt::draw_targets(const Robot3 &robot, cv::Mat &frame, bool inverted, RobotColor robot_color) {
	cv::Scalar color;
	double angle;
	cv::Point target;
	cv::Point position = robot.pose.position.inverted_coordinates(inverted).to_cv_point();
	bool is_test_on_click = test_on_click.is_active();

	if (is_test_on_click) {
		color = test_color;
		angle = test_on_click.get_orientation_value();
		target = test_on_click.get_target().to_cv_point();
	} else {
		if (robot_color == RobotColor::Yellow) color = yellow_color;
		else color = blue_color;
		Target robot_target = robot.target.inverted(inverted);
		angle = robot_target.pose.orientation;
		target = robot_target.pose.position.to_cv_point();
	}

	switch (is_test_on_click? test_on_click.get_command() : robot.target.command) {
		case Command::Vector:
			[[fallthrough]];
		case Command::Orientation: {
			auto x2 = static_cast<int>(position.x + 16 * cos(angle));
			auto y2 = static_cast<int>(position.y - 16 * sin(angle));
			line(frame, position, cv::Point(x2, y2),
				 color, 3, cv::LINE_AA);
			break;
		}
		case Command::None:
			putText(frame, "x", position, cv::FONT_HERSHEY_PLAIN, 1,
					color, 2, cv::LINE_AA);
			break;
		case Command::UVF: {
			auto x2 = static_cast<int>(target.x + 16 * cos(angle));
			auto y2 = static_cast<int>(target.y - 16 * sin(angle));
			circle(frame, target, 7, color, 2);
			line(frame, target, cv::Point(x2, y2),
				 color, 3, cv::LINE_AA);
			putText(frame, std::to_string(robot.TAG + 1),
					cv::Point(target.x - 5, target.y - 17),
					cv::FONT_HERSHEY_PLAIN, 1, color, 2, cv::LINE_AA);
			break;
		}
		default:
			circle(frame, target, 7, color, 2, cv::LINE_AA);
			putText(frame, std::to_string(robot.TAG + 1),
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
