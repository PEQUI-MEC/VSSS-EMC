#ifndef VSSS_NOSTRATEGY_HPP
#define VSSS_NOSTRATEGY_HPP

#include "Strategy.hpp"

class NoStrategy : public Strategy {
	void run_strategy(std::vector<Robot3> &team, std::vector<Adversary> &adversaries, Ball ball,
					  bool first_iteration) {
		for (auto& robot : team) {
			robot.send_no_command();
		}
	}

	virtual NoStrategy * clone() {
		return new NoStrategy(*this);
	};

	void set_foul(VSSRef::ref_to_team::VSSRef_Command foul, bool is_defending) {};
};

#endif //VSSS_NOSTRATEGY_HPP
