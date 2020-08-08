#ifndef VSSS_TEAM_HPP
#define VSSS_TEAM_HPP

#include "Robot3.hpp"
#include "Strategy3.hpp"
#include <vector>

class Team {
	public:
	std::vector<Robot3> robots;
	std::unique_ptr<Strategy> strategy; // IA ou manual
	bool controlled = false;
	bool inverted_field = false;
	Team(int robot_number, int starting_tag, bool controlled, bool inverted_field) : controlled(controlled), inverted_field(inverted_field) {
		for(int i = starting_tag; i < starting_tag + robot_number; i++) {
			robots.push_back(Robot3(i, 'A' + i));
		}
	};
};

#endif //VSSS_TEAM_HPP
