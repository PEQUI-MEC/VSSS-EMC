//
// Created by daniel on 25/11/18.
//

#include "ImageArt.hpp"

using namespace vision;

void art::draw(cv::Mat &frame, const std::vector<cv::Point> &gmm_points, const vision::Vision::Ball &ball,
			   const std::map<unsigned int, Vision::RecognizedTag> &our_tags, const std::vector<cv::Point> &adv_tags,
			   const std::array<Robot2 *, 3> &our_robots, const bool is_game_on) {

	// GMM draw points
	for (unsigned long i = 0; i < gmm_points.size(); i = i + 2) {
		cv::rectangle(frame, gmm_points.at(i), gmm_points.at(i + 1), cv::Scalar(0, 255, 255));
	}

	// Ball
	if (ball.isFound)
		circle(frame, ball.position, 7, cv::Scalar(255, 255, 255), 2);

	// Our Robots
	for (const auto &tag : our_tags) {
		circle(frame, tag.second.position, 15, cv::Scalar(255, 255, 0), 2);
		putText(frame, std::to_string(tag.first+1),
				cv::Point(tag.second.position.x + 13, tag.second.position.y - 15),
				cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 0), 2);
		// linha da orientação do robô
		line(frame, tag.second.rear_point, tag.second.front_point, cv::Scalar(255, 0, 0), 2);
	}

	// Adversary Robots
	for (const auto &robot : adv_tags) {
		circle(frame, robot, 15, cv::Scalar(0, 0, 255), 2);
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
					auto x2 = static_cast<int>(position.x + 16*cos(angle));
					auto y2 = static_cast<int>(position.y - 16*sin(angle));
					line(frame, position, cv::Point(x2, y2),
						 cv::Scalar(127, 255, 127), 3);
					break;
				}
				case Robot2::Command::None:
					putText(frame, "x", robot->get_position().to_cv_point(), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(127, 255, 127), 2);
					break;
				case Robot2::Command::UVF: {
					double angle = robot->get_target().orientation;
					cv::Point target = robot->get_target().position.to_cv_point();
					auto x2 = static_cast<int>(target.x + 16*cos(angle));
					auto y2 = static_cast<int>(target.y - 16*sin(angle));
					circle(frame, target, 7, cv::Scalar(127, 255, 127), 2);
					line(frame, target, cv::Point(x2, y2),
						 cv::Scalar(127, 255, 127), 3);
					putText(frame, std::to_string(robot->tag + 1),
							cv::Point(target.x - 5, target.y - 17),
							cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(127, 255, 127), 2);
					break;
				}
				default:
					cv::Point robot_target = robot->get_target().position.to_cv_point();
					circle(frame, robot_target, 7, cv::Scalar(127, 255, 127), 2);
					putText(frame, std::to_string(robot->tag + 1),
							cv::Point(robot_target.x - 5, robot_target.y - 17),
							cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(127, 255, 127), 2);
			} // switch
		} // for
	}
}
