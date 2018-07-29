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
	go_to(ball, ball_to_goal.with_size(0.1));
//	Inserir corrida para o gol ao chegar na bola
}
