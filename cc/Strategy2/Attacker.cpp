#include "Attacker.h"

using namespace Geometry;

//	Placeholders, devem ser substituidos pelas constantes do Field
const static Line goal_line({10,10}, {10, 0});
const static Point goal_center({5, 10});

void Attacker::uvf_to_goal(Point ball) {
	Vector ball_to_goal = goal_center - ball;
	go_to_pose(ball, ball_to_goal);
//	Inserir corrida para o gol ao chegar na bola
}
