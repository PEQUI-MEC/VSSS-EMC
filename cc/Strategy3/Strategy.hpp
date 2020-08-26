#ifndef VSSS_STRATEGY_HPP
#define VSSS_STRATEGY_HPP

#include <vector>
#include <Geometry/Geometry.h>
#include "Robot3.hpp"
#include "Types.hpp"
#include "Ball.hpp"

class Strategy {
	public:
	virtual void run_strategy(std::vector<Robot3> &team,
							  std::vector<Geometry::Point> &adversaries,
							  Ball ball,
							  bool first_iteration) = 0;
	virtual Strategy * clone() = 0;
};

#endif //VSSS_STRATEGY_HPP
