#ifndef VSSS_TEAM_HPP
#define VSSS_TEAM_HPP

#include "Robot3.hpp"
#include "Strategy3.hpp"
#include "Robots.hpp"
#include <vector>

class Team {
	public:
	Robots robots;
	std::unique_ptr<Strategy> strategy; // IA ou manual
	bool controlled = false;
	bool invert_camp = false;
	Team(bool controlled, bool invert_camp) : controlled(controlled), invert_camp(invert_camp) {};
};

#endif //VSSS_TEAM_HPP
