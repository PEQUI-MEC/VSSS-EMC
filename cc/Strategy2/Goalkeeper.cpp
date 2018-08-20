#include "Goalkeeper.hpp"
#include "Field.h"

using namespace field;
using namespace Geometry;

void Goalkeeper::protect_goal(const Point ball, const Point ball_est) {
	const Line est_line(ball, ball_est);
	const Point goal_est = intersection(our::goal::front::line, est_line);

	if (ball_est.x < ball.x) {
		// a bola está vindo para o nosso campo

		// Se goal_est estiver na pequena área, fica na projeção da estimativa da bola
		// se não fica no limite da pequena área
		if (at_location(goal_est, Location::OurBox))
			go_to_and_stop(intersection(est_line, our::area::goalkeeper::line));
		else {
			if (at_location(goal_est, Location::UpperField))
				go_to_and_stop(our::area::goalkeeper::upper_limit);
			else
				go_to_and_stop(our::area::goalkeeper::lower_limit);
		}
	} else {
		// fica na projeção da bola, sempre em frente ao gol
		if (ball.y <= our::area::goalkeeper::lower_limit.y)
			go_to_and_stop(our::area::goalkeeper::lower_limit);
		else if (ball.y >= our::area::goalkeeper::upper_limit.y)
			go_to_and_stop(our::area::goalkeeper::upper_limit);
		else
			go_to_and_stop({our::area::goalkeeper::lower_limit.x, ball.y});
	}
}

void Goalkeeper::spin_shot(const Point ball) {
	Pose pose = get_pose();

	if (ball.x > pose.position.x) { // bola na frente/lateral do goleiro
		if (ball.y > pose.position.y) // bola acima do goleiro
			spin(-35.0); // horário
		else
			spin(35.0); // anti-horário
	} else {
		// FIXME: Caso contrário o goleiro deve ficar parado para evitar fazer gol contra. Poderia fazer outra coisa?
		go_to_and_stop(get_pose().position);
	}
}
