#ifndef ROBOT_HPP_
#define ROBOT_HPP_
#include "opencv2/opencv.hpp"
#include "../../cc/vision/tag.hpp"

class Robot
{
    public:
        cv::Point position, secundary, target, transTarget, ternary;
        cv::Point rearPoint;
        char ID;
        bool isOdd;
        double orientation, targetOrientation;
        double orientation2;
        double transAngle;
        int cmdType;
        double vdefault;
        double vmax; // m/s
        float Vl,Vr;  // RPS
        bool fixedPos;
        bool using_pot_field;
        bool ignore_obstacles = false;
        int role;
        int status;

        Robot() : isOdd(false), orientation(0), orientation2(0),
        cmdType(0), vdefault(0.8), vmax(1.4), fixedPos(false),
        using_pot_field(false), role(0), status(0)
        {
            ternary = cv::Point(-1,-1);
            secundary= cv::Point(-1,-1);
            position= cv::Point(-1,-1);
            target = cv::Point(-1,-1);
            rearPoint = cv::Point(-1,-1);
        }

        ~Robot() {

        }
};
#endif /* ROBOT_HPP_ */
