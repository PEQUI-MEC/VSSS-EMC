#include <filesystem>
#include "AIStrategy.hpp"
#include "Field.h"

AIStrategy::AIStrategy() {
	Py_Initialize();

	char path[PATH_MAX];
	readlink("/proc/self/exe", path, PATH_MAX);

//	auto current_path = std::filesystem::current_path();
//	auto dir = current_path.string() + "/python";
	auto dir = "/home/thiago/Workspace/VSSS-EMC/python";
	auto py_dir = PyUnicode_FromString(dir);
	auto sys_path = PySys_GetObject("path");
	PyList_Append(sys_path, py_dir);

	std::cout << "importing module" << std::endl;
	module = PyImport_ImportModule("ai");
	if (module == nullptr) {
		std::cout << "Could not import AI module" << std::endl;
		return;
	} else {
		std::cout << "AI module imported" << std::endl;
	}
	module_dict = PyModule_GetDict(module);
	ai_class = PyDict_GetItemString(module_dict, "AI");
	auto our_goal = field::our::goal::front::center.to_python();
	auto adv_goal = field::their::goal::front::center.to_python();
	auto constructor_args = PyTuple_Pack(2, our_goal, adv_goal);
	ai_object = PyObject_CallObject(ai_class, constructor_args);
	strategy_method_name = PyUnicode_FromString("run_strategy");

	std::cout << "Constructed AI Strategy" << std::endl;

//	Py_DecRef(sys_path);
//	Py_DecRef(py_dir);
}

void AIStrategy::run_strategy(Robots team, std::vector<Geometry::Point> &adversaries, Ball ball) {
	if (module == nullptr) return;
//	Robot positions
	auto robot1 = team[0].python_pose();
	auto robot2 = team[1].python_pose();
	auto robot3 = team[2].python_pose();
//	Adversary positions
	auto adv0 = adversaries[0].to_python();
	auto adv1 = adversaries[1].to_python();
	auto adv2 = adversaries[2].to_python();
//	Ball position
	auto ball_pos = ball.position.to_python();
//	Call ai strategy
	auto velocities = PyObject_CallMethodObjArgs(ai_object, strategy_method_name,
												 robot1, robot2, robot3,
												 ball_pos, adv0, adv1, adv2,
												 nullptr);

	for (int i = 0; i < 3; i++) {
//		Set robot commands
		auto wheel_vel = PyTuple_GetItem(velocities, i);
		auto py_vel_left = PyTuple_GetItem(wheel_vel, 0);
		auto py_vel_right = PyTuple_GetItem(wheel_vel, 1);
		double left = PyFloat_AsDouble(py_vel_left);
		double right = PyFloat_AsDouble(py_vel_right);
		team[i].set_target_wheel_velocity(left, right);
	}

//	Delete python objects
//	Py_DecRef(velocities);
//	Py_DecRef(ball_pos);
//	Py_DecRef(robot3);
//	Py_DecRef(robot2);
//	Py_DecRef(robot1);
}

AIStrategy::~AIStrategy() {
	Py_Finalize();

	Py_DecRef(strategy_method_name);
	Py_DecRef(ai_object);
	Py_DecRef(ai_class);
	Py_DecRef(module_dict);
	Py_DecRef(module);
}

