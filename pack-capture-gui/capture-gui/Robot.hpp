#ifndef ROBOT_HPP_
#define ROBOT_HPP_

#include "opencv2/opencv.hpp"
#include "../../cc/vision/tag.hpp"

class Robot {
	public:
		cv::Point position, secundary, target;
		cv::Point rearPoint, uvf_ref;
		cv::Point proj_to_ball;
		char ID;
		bool isOdd;
//		Orientation: positivo no sentido horário
		double orientation, targetOrientation;
		double orientation2;
//		TransAngle: positivo no sentido anti-horário
		double transAngle;
		int cmdType;
		double vdefault;
		double vmax; // m/s
		double Vl;
		double Vr;  // RPS
		double uvf_n;
		bool fixedPos;
		bool using_pot_field;
		int role;
		int status;

		Robot() : isOdd(false), orientation(0), orientation2(0),
				  cmdType(0), vdefault(0.8), vmax(1.4), fixedPos(false),
				  using_pot_field(false), role(0), status(0) {
			secundary = cv::Point(-1, -1);
			position = cv::Point(-1, -1);
			target = cv::Point(-1, -1);
			rearPoint = cv::Point(-1, -1);
		}

		~Robot() {
		}
};

#endif /* ROBOT_HPP_ */
