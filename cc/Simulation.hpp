#ifndef VSSS_SIMULATION_HPP
#define VSSS_SIMULATION_HPP

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/PointStamped.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <vsss_msgs/Control.h>
#include <thread>
#include <optional>
#include <Strategy2/Robot2.h>
#include <Strategy2/Strategy2.hpp>
#include "Ball.hpp"
#include <V4LInterface.hpp>
#include <Strategy2/Team.h>

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
		ros::NodeHandle nh;
		std::array<RosRobot, 3> ros_robots;
		bool run_ros_thread = false;
		std::optional<std::thread> ros_thread;
		message_filters::Subscriber<PointStamped> ball_position_sub;
		message_filters::TimeSynchronizer<PoseStamped,
		PoseStamped, PoseStamped, PointStamped> sync;

//		References from other classes
    std::shared_ptr<Team> team;
		bool is_right_team;

		void ros_callback(const PoseStampedPtr &robot1_msg, const PoseStampedPtr &robot2_msg,
						  const PoseStampedPtr &robot3_msg, const PointStampedPtr &ball_msg);

	public:
		capture::V4LInterface& interface;

		Simulation(const std::string &name1, const std::string &name2, const std::string &name3, bool is_right_team,
					capture::V4LInterface &interface_ref);
		void start_ros_thread();
		void stop_ros_thread();
		~Simulation() {
			stop_ros_thread();
		}
};

#endif //VSSS_SIMULATION_HPP
