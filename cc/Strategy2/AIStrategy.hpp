#ifndef VSSS_AISTRATEGY_HPP
#define VSSS_AISTRATEGY_HPP

#include <Python.h>
#include <Geometry/Geometry.h>
#include <Robot2.h>

class AIStrategy {
	public:
		AIStrategy();
		~AIStrategy();
		void run_strategy(std::array<Robot2 *, 3> &robots, Geometry::Point ball,
				std::array<Geometry::Point, 3> advs);

	private:
		PyObject * module;
		PyObject * module_dict;
		PyObject * ai_class;
		PyObject * ai_object;
		PyObject * strategy_method_name;
};

#endif //VSSS_AISTRATEGY_HPP
