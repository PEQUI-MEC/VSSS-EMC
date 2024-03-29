#ifndef VSSS_AISTRATEGY_HPP
#define VSSS_AISTRATEGY_HPP

#include <Python.h>
#include <Geometry/Geometry.h>
#include <Robot2.h>
#include <Strategy3/Strategy.hpp>

class AIStrategy : public Strategy {
	public:
		AIStrategy();
		~AIStrategy();
		void run_strategy(std::vector<Robot3> &team, std::vector<Adversary> &adversaries, Ball ball,
						  bool first_iteration, bool is_simulation, bool is_inverted, time_point now);

	AIStrategy * clone() {
		return new AIStrategy(*this);
	}

	void set_foul(VSSRef::ref_to_team::VSSRef_Command foul, bool is_defending, time_point foul_time) {};

	private:
		PyObject * module;
		PyObject * module_dict;
		PyObject * ai_class;
		PyObject * ai_object;
		PyObject * strategy_method_name;
};

#endif //VSSS_AISTRATEGY_HPP
