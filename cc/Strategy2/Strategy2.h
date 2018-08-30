#ifndef VSSS_STRATEGY2_H
#define VSSS_STRATEGY2_H

#include "Attacker.h"

class Strategy2 {
	private:
		Attacker &attacker;

		Geometry::Point &ball;

	public:
		Strategy2(Attacker &attacker, Geometry::Point &ball);
		void run();
};

#endif //VSSS_STRATEGY2_H
