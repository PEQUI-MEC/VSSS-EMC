#ifndef VSSS_STRATEGY_HPP
#define VSSS_STRATEGY_HPP

#include <vector>
#include <Geometry/Geometry.h>
#include "Robot3.hpp"
#include "Types.hpp"
#include "Ball.hpp"

class Strategy {
	public:
	using Robots = std::vector<Robot3>&;
	virtual void run_strategy(Robots team,
							  std::vector<Geometry::Point> &adversaries,
							  Ball ball) = 0;
	virtual Strategy * clone() = 0;
};

#endif //VSSS_STRATEGY_HPP
