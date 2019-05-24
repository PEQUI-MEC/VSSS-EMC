#ifndef VSSS_TEAM_H
#define VSSS_TEAM_H

#include "Strategy2.hpp"
#include "Ball.hpp"

struct Team {
	Attacker attacker;
	Defender defender;
	Goalkeeper goalkeeper;

	std::array<Robot2 *, 3> robots;

	Strategy2 strategy;

	explicit Team(Ball &ball) : robots{&attacker, &defender, &goalkeeper},
								strategy(attacker, defender, goalkeeper, ball.position, ball.estimate) {
		attacker.tag = 0;
		defender.tag = 1;
		goalkeeper.tag = 2;
	}
};

#endif //VSSS_TEAM_H
