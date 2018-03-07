#ifndef STRATEGY_HPP_
#define STRATEGY_HPP_
#define PI 3.14159265453

#include "Fuzzy/FuzzyController.hpp"
#include "opencv2/opencv.hpp"
#include "LS.h"
#include "../pack-capture-gui/capture-gui/Robot.hpp"
#include <math.h>
#include "TestFrame.hpp"
#include "Constants.hpp"

#define PREDICAO_NUM_SAMPLES 15
// Parametros para atacante sem bola

#define ENTRADA_GOL_INVERSO    1

//Parametros para atacante com bola
#define AVOID_GOLEIRO_APOLO        0

// Parametros do Defensor com bola no ataque
#define DEF_RATIO            1.0f

// Parametros do Defensor na defesa
#define ZAGA_CENTRALIZADA            1
//role
#define GOALKEEPER 0
#define DEFENDER 1
#define ATTACKER 2
#define OPPONENT 3

//cmdType
#define POSITION 0
#define SPEED 1
#define ORIENTATION 2
#define VECTOR 3
#define VECTORRAW 4

//state
#define NORMAL_STATE 0
#define CORNER_STATE 1
#define    STEP_BACK 2
#define    ADVANCING_STATE 3
#define    TRANSITION_STATE 4
#define    SIDEWAYS 5
#define DEF_CORNER_STATE 6


//potField
#define BALL_IS_OBS 0
#define BALL_ONLY_OBS 1
#define BALL_NOT_OBS 2
#define NO_OBS 3

#define THETA_TOLERATION 3

using namespace CONST;

class Strategy {
	public:
		TestFrame testFrame;

		// Robot Goalkeeper;
		// Robot Attack;
		// Robot Defense;
		// Robot Opponent;
		cv::Point Ball;
		cv::Point FutureBall;
		int distBall;
		LS LS_ball_x;
		LS LS_ball_y;
		cv::Point Ball_Est;
		cv::Point target = cv::Point(-1, -1);
		cv::Point targetlock = cv::Point(-1, -1);

		double pot_thetaX = 0;
		double pot_thetaY = 0;
		double pot_theta = 0;
		double pot_goalTheta = 0;
		double pot_goalMag;
		double pot_magnitude[5];
		double pot_angle[5];
		float ball_angle = 0;
		float ball_mag = 0;

		vector<Robot> robots;
		cv::Point *adv;
		int collision_count[3];
		double past_transangle[3];
		cv::Point past_position[3];
		cv::Point Goalkeeper;
		cv::Point Defender;
		cv::Point Attacker;
		cv::Point Opponent;
		int atk, def, gk, opp;
		double lock_angle;

		FuzzyController controller;

		int frames_blocked;

		bool half_transition = false;
		bool full_transition = false;
		bool danger_zone_1 = false;
		bool danger_zone_2 = false;
		bool half_transition_enabled = false;
		bool full_transition_enabled = false;
		bool transition_mindcontrol_enabled = true;
		bool atk_ball_possession = false;

		int t = 0;
		int timeout = 0;
		int transition_timeout = 0;
		int transition_overmind_timeout = 0;
		bool action1 = false;
		bool action2 = false;
		bool action3 = false;
		bool kick = false;
		bool transition_mindcontrol = false;
		bool atk_mindcontrol = false;
		bool def_mindcontrol = false;

		Strategy();
		void set_constants(int w, int h);
		double distance(cv::Point A, cv::Point B);
		double distance_meters(cv::Point A, cv::Point B);
		void get_variables();
		void get_past(int i);
		void get_targets(vector<Robot> *pRobots, cv::Point *advRobots);
		void overmind();
		void set_flags();
		bool offensive_adv();
		void fixed_position_check(int i);
		void collision_check(int i);
		bool cock_blocked();
		void set_role(int i, int role);
		bool is_near(int i, cv::Point point);
		void position_to_vector(int i);
		void go_to_the_ball(int i);
		void go_to_the_ball_direct(int i);
		void around_the_ball(int i);
		double look_at_ball(int i);
		double potField(int robot_index, cv::Point goal, int behavior);
		int pot_rotation_decision(int robot_index, cv::Point goal, cv::Point obst);
		double reach_on_time(double dist, double time_limit);
		void spin_anti_clockwise(int i, double speed);
		void spin_clockwise(int i, double speed);
		void def_wait(int i);
		void fuzzy_init();
		int Fuzzy_Troca();
		double map_range(double actual, double minactual, double maxactual);
		void pot_field_around(int i);
		void gotta_catch_the_ball(int i);
		void crop_targets(int i);
		void smart_ball(int i, int max_distance);
		void test_run(int i);
		void atk_routine(int i);
		void def_routine(int i);
		void gk_routine(int i);
		void def_past_routine(int i);
		void opp_gk_routine(int i);
		cv::Point get_Ball_Est();
		void set_Ball_Est(cv::Point b);
		void set_Ball(cv::Point b);
};

#endif /* STRATEGY_HPP_ */
