#include "Strategy2.h"

Strategy2::Strategy2(Attacker &attacker, Geometry::Point &ball) :
		attacker(attacker), ball(ball) {
}

void Strategy2::run() {
	attacker.execute_behavior(Attacker::Behavior::UVF_To_Goal);
}
