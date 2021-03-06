#include "Goalkeeper.hpp"
#include "Field.h"

using namespace field;
using namespace Geometry;

void Goalkeeper::protect_goal(const Point& ball, const Point& ball_est) {
	if (at_location(ball, Location::OurUpperCorner)) {
		go_to_and_stop(our::area::upper::center);
	} else if (at_location(ball, Location::OurLowerCorner)) {
		go_to_and_stop(our::area::lower::center);
	} else if (ball_est.x < ball.x && distance(ball, ball_est) > 0.03) {
		// a bola está vindo para o nosso campo

		// Se goal_est estiver na pequena área, fica na projeção da estimativa da bola
		// se não fica no limite da pequena área
		if (match_y(ball_est, Location::OurBox))
			go_to_and_stop({our::area::box::upper_limit.x, ball_est.y});
		else if (at_location(ball_est, Location::UpperField))
			go_to_and_stop(our::area::box::upper_limit);
		else
			go_to_and_stop(our::area::box::lower_limit);
	} else {
		// fica na projeção da bola, sempre em frente ao gol
		if (match_y(ball, Location::OurBox))
			go_to_and_stop({our::area::box::upper_limit.x, ball.y});
		else if (at_location(ball, Location::UpperField))
			go_to_and_stop(our::area::box::upper_limit);
		else
			go_to_and_stop(our::area::box::lower_limit);
	}
}

void Goalkeeper::spin_shot(const Point& ball) {
	double lower_robot_x = get_position().x - SIZE/4; // é size/4 para que evite fazer gol contra
	double upper_robot_x = get_position().x + SIZE/2;

	if (ball.x > upper_robot_x) { // bola na frente do goleiro
		if (at_location(get_position(), Location::UpperField))
			spin(35.0);
		else
			spin(-35.0);
	} else if (ball.x > lower_robot_x && ball.x <= upper_robot_x) { // bola na lateral do goleiro
		if (ball.y > get_position().y) // bola acima do goleiro
			spin(-35.0); // horário
		else
			spin(35.0); // anti-horário
	} else {
		// FIXME: Caso contrário o goleiro deve ficar parado para evitar fazer gol contra. Poderia fazer outra coisa?
		go_to_and_stop(get_position());
	}
}

void Goalkeeper::exit_goal() {
		go_to_and_stop(our::area::box::center_line_point);
}
