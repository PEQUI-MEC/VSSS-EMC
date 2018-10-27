#include "Attacker.h"

using namespace Geometry;
using namespace field;

void Attacker::decide_spin_shot(const Geometry::Point &ball) {
	double upper_y = get_position().y + size/2;
	double upper_x = get_position().x + size/2;
	double lower_x = get_position().x - size/2;

	if ((at_location(get_position(), Location::UpperField) && (ball.y > upper_y || ball.x > upper_x))
		|| (at_location(get_position(), Location::LowerField) && (ball.y > upper_y || ball.x < lower_x))) {
		spin(-35.0); // Robô gira no sentido horário
	} else {
		spin(35);//Robô gira no sentido anti-horário
	}
}

void Attacker::uvf_to_goal(const Geometry::Point &ball) {
	Vector ball_to_goal = their::goal::back::center - ball;

	if(distance(ball, get_position()) > 0.03){
		go_to_pose(ball, ball_to_goal);
	}else{
	    go_in_direction(ball_to_goal);
	}
}

void Attacker::spin_shot(const Geometry::Point &ball) {
	decide_spin_shot(ball);
}

void Attacker::crossing(const Geometry::Point &ball){
	if (distance(ball, get_position()) >= size) {
		// longe da bola: vai até a bola
		go_to(ball);
	} else {
		decide_spin_shot(ball);
	}
}

void Attacker::protect_goal(const Geometry::Point &ball) {
	if (distance(get_position(), ball) < 0.1) {
		 // Se a bola chegar perto, gira para jogar a bola longe
		if (at_location(ball, Location::UpperField))
			spin(-35); // horário
		else
			spin(35); // anti-horário
	} else if (at_location(ball, Location::UpperField)) {
		// bloquear area (cima)
		go_to_and_stop(our::corner::upper::attacker::point);
	} else {
		// bloquear area (baixo)
		go_to_and_stop(our::corner::lower::attacker::point);
	}
}

void Attacker::charged_shot(const Geometry::Point &ball) {
	go_in_direction(ball - get_position(), 1.2);
}
