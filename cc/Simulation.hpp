#ifndef VSSS_SIMULATION_HPP
#define VSSS_SIMULATION_HPP

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/PointStamped.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <vsss_msgs/Control.h>
#include <thread>
#include <Strategy2/Robot2.h>
#include "Ball.hpp"

using PoseStamped = geometry_msgs::PoseStamped;
using PoseStampedPtr = geometry_msgs::PoseStampedPtr;
using PointStampedPtr = geometry_msgs::PointStampedPtr;
using PointStamped = geometry_msgs::PointStamped;

struct RosRobot {
	using PoseSub = message_filters::Subscriber<PoseStamped>;
	std::unique_ptr<PoseSub> pose_sub;
	ros::Publisher control_pub;
	RosRobot() = default;
	RosRobot(ros::NodeHandle& nh, const std::string& id):
			pose_sub() {
		pose_sub = std::make_unique<PoseSub>(nh, "robot" + id + "/pose", 1);
		control_pub = nh.advertise<vsss_msgs::Control>("robot" + id + "/control", 1);
	}
};

class Simulation {
		bool use_simulator = false;

		ros::NodeHandle nh;
		std::array<RosRobot, 3> ros_robots;
		std::thread * simulator_thread;
		message_filters::Subscriber<PointStamped> ball_position_sub;
		message_filters::TimeSynchronizer<PoseStamped,
		PoseStamped, PoseStamped, PointStamped> sync;

//		References from other classes
		std::array<Robot2 *, 3> &robots;
		Ball& ball;

		void ros_callback(const PoseStampedPtr &robot1_msg, const PoseStampedPtr &robot2_msg,
						  const PoseStampedPtr &robot3_msg, const PointStampedPtr &ball_msg);

	public:
		Simulation(std::array<Robot2 *, 3>& robots, Ball& ball);
};

#endif //VSSS_SIMULATION_HPP
