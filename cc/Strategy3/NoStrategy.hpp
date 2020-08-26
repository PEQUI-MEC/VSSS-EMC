#ifndef VSSS_NOSTRATEGY_HPP
#define VSSS_NOSTRATEGY_HPP

#include "Strategy.hpp"

class NoStrategy : public Strategy {
	void run_strategy(std::vector<Robot3> &team, std::vector<Geometry::Point> &adversaries, Ball ball,
					  bool first_iteration) {
		for (auto& robot : team) {
			robot.send_no_command();
		}
	}

	virtual NoStrategy * clone() {
		return new NoStrategy(*this);
	};
};

#endif //VSSS_NOSTRATEGY_HPP
