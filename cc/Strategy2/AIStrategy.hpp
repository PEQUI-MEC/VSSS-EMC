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
		void run_strategy(Robots team, Robots adversaries, Ball ball);

	AIStrategy * clone() {
		return new AIStrategy(*this);
	}

	private:
		PyObject * module;
		PyObject * module_dict;
		PyObject * ai_class;
		PyObject * ai_object;
		PyObject * strategy_method_name;
};

#endif //VSSS_AISTRATEGY_HPP
