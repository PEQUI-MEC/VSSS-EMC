#include "Attacker.h"

using namespace Geometry;

//	Placeholders, devem ser substituidos pelas constantes do Field
const static Line goal_line({10,10}, {10, 0});
const static Point goal_center({5, 10});

void Attacker::execute_behavior(Attacker::Behavior behavior) {
	switch (behavior) {
		case Behavior::UVF_To_Goal:
			uvf_to_goal({0,0});
			break;
	}
}

void Attacker::uvf_to_goal(Point ball) {
	Vector ball_to_goal = goal_center - ball;
	go_to_pose(ball, ball_to_goal);
	go_in_direction(ball_to_goal);
//	Inserir corrida para o gol ao chegar na bola
}

void Attacker::spin_shot(bool clockwise, double speed){
	if (clockwise){
		// Robô gira no sentido horário
	}else{
		//Robô gira no sentido anti-horário
	}
}

void Attacker::hang_in_the_corner(Point ball){
	Vector robot_to_ball = ball - get_pose().position;
	go_to_pose(ball, robot_to_ball);


}

