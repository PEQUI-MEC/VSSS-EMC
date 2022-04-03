#ifndef VSSS_STRATEGY_HPP
#define VSSS_STRATEGY_HPP

#include <vector>
#include <Geometry/Geometry.h>
#include "Robot3.hpp"
#include "Types.hpp"
#include "Ball.hpp"
#include "vssref_command.pb.h"

class Strategy {
	public:
	virtual void run_strategy(std::vector<Robot3> &team,
							  std::vector<Geometry::Point> &adversaries,
							  Ball ball,
							  bool first_iteration,
							  time_point now) = 0;
	virtual Strategy * clone() = 0;
	virtual void set_foul(VSSRef::ref_to_team::VSSRef_Command foul, bool is_defending, time_point foul_time) = 0;
};

#endif //VSSS_STRATEGY_HPP
