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

	if(distance(ball,get_pose().position)>0.03){
		go_to_pose(ball, ball_to_goal);
	}else{
	go_in_direction(ball_to_goal);
	}
	//	Inserir corrida para o gol ao chegar na bola
}

void Attacker::spin_shot(bool clockwise){
	if (clockwise){
		spin(-1.4);// Robô gira no sentido horário
	}else{
		spin(1.4);//Robô gira no sentido anti-horário
	}
}

void Attacker::crossing(Point ball, bool clockwise){

	if(ball.x > get_pose().position.x) {
		Vector robot_to_ball = ball - get_pose().position;
		go_to_pose(ball, robot_to_ball);

		if (ball.y > get_pose().position.y && distance(ball, get_pose().position) < size) {
			if(clockwise)
			   spin(-1.4);
            else
				spin(1.4);
		}else {
			go_to_pose(ball, robot_to_ball);
		}
	}else{

		uvf_to_goal(ball);
		//Se ele tiver a frente da bola, faz apenas o UVF,
		//porém a condição tem que ser verificada pela estratégia antes de chamar o crossing,
		//ele vai só entrar nesse else em caso de ser chamado em hora indevida.
	}
}

void Attacker::atk_mindcontrol(Vector ball_to_goal){
		go_in_direction(ball_to_goal,1.4);

}
