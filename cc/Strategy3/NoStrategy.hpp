#ifndef VSSS_NOSTRATEGY_HPP
#define VSSS_NOSTRATEGY_HPP

#include "Strategy.hpp"

class NoStrategy : public Strategy {
	void run_strategy(Robots team,
							  Robots adversaries,
							  Ball ball) {
			}

	virtual NoStrategy * clone() {
		return new NoStrategy(*this);
	};
};

#endif //VSSS_NOSTRATEGY_HPP
