//
// Created by thiago on 01/02/2020.
//

#include <LS.h>
#include <RobotControl/SimuRobotControl.h>
#include "Attacker.h"
#include "Defender.hpp"
#include "Goalkeeper.hpp"
#include "Strategy2.hpp"
#include "Geometry.h"

Attacker attacker;
Defender defender;
Goalkeeper goalkeeper;
LS ls_x, ls_y;
Geometry::Point ball;
Geometry::Point ball_est;
Strategy2 strategy(attacker, defender, goalkeeper, ball, ball_est);
std::array<Robot2 *, 3> robots{&attacker, &defender, &goalkeeper};

void init() {
	ls_x.init(15, 1);
	ls_y.init(15, 1);
	attacker.tag = 1;
	defender.tag = 2;
	goalkeeper.tag = 3;
}

Robot2 * get_robot(int tag) {
	for (auto robot : robots) {
		if (robot->tag == tag) {
			return robot;
		}
	}
	std::cout << "ERRO do thiago!!" << std::endl;
}

void update_ball_est() {
	ls_x.addValue(ball.x);
	ls_y.addValue(ball.y);
	ball_est.x = ls_x.estimate(10);
	ball_est.y = ls_y.estimate(10);
}

struct Velocities {
		Control::WheelVelocity vel1;
		Control::WheelVelocity vel2;
		Control::WheelVelocity vel3;
};

// data[0] e data[1]: x e y em metros, origem no canto inferior esquerdo
// data[2]: orientacao de -pi a pi
// data[3] e data[4]: vel da roda esquerda e direita
// time: tempo em segundos entre cada iteracao do controle
Velocities run(float robot1data[5], float robot2data[5], float robot3data[5], float ballpos[2], float time) {
	auto * robot1 = get_robot(1);
	robot1->set_pose_m({robot1data[0], robot1data[1]}, robot1data[2]);
	auto * robot2 = get_robot(2);
	robot2->set_pose_m({robot2data[0], robot2data[1]}, robot2data[2]);
	auto * robot3 = get_robot(3);
	robot3->set_pose_m({robot3data[0], robot3data[1]}, robot3data[2]);

	ball = {ballpos[0], ballpos[1]};
	update_ball_est();

	strategy.run();

	robot1 = get_robot(1);
	robot2 = get_robot(1);
	robot3 = get_robot(1);
	auto vel1 = robot1->run_control(robot1data[3], robot1data[4], time);
	auto vel2 = robot2->run_control(robot2data[4], robot2data[4], time);
	auto vel3 = robot3->run_control(robot3data[4], robot3data[4], time);
	return Velocities{vel1, vel2, vel3};
}
