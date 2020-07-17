#ifndef VSSS_STRATEGY_HPP
#define VSSS_STRATEGY_HPP

#include <vector>
#include <Geometry/Geometry.h>
#include "Robot3.hpp"
#include "Types.hpp"
#include "Robots.hpp"

class Strategy {
	public:
	virtual void run_strategy(Robots team,
								Robots adversaries,
								Ball ball) = 0;
};

#endif //VSSS_STRATEGY_HPP
