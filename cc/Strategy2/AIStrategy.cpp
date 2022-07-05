#include "AIStrategy.hpp"
#include "Field.h"

AIStrategy::AIStrategy() {
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

//	Py_DecRef(sys_path);
//	Py_DecRef(py_dir);
}

void AIStrategy::run_strategy(std::vector<Robot3> &team,
							  std::vector<Adversary> &adversaries,
							  Ball ball,
							  bool first_iteration, bool is_simulation, bool is_inverted, time_point now) {
	if (module == nullptr) return;
//	Robot positions
	auto robot1 = team[0].python_pose();
	auto robot2 = team[1].python_pose();
	auto robot3 = team[2].python_pose();
//	Adversary positions
	auto adv0 = adversaries[0].position.to_python();
	auto adv1 = adversaries[1].position.to_python();
	auto adv2 = adversaries[2].position.to_python();
//	Ball position
	auto ball_pos = ball.position.to_python();
//	First iteration
	PyObject * first_iter;
	if (first_iteration) {
		Py_IncRef(Py_True);
		first_iter = Py_True;
	}
	else {
		Py_IncRef(Py_False);
		first_iter = Py_False;
	}
//	Call ai strategy
	auto velocities = PyObject_CallMethodObjArgs(ai_object, strategy_method_name,
												 robot1, robot2, robot3,
												 ball_pos, adv0, adv1, adv2,
												 first_iter,
												 nullptr);

	for (int i = 0; i < 3; i++) {
//		Set robot commands
		auto wheel_vel = PyTuple_GetItem(velocities, i);
		auto py_vel_left = PyTuple_GetItem(wheel_vel, 0);
		auto py_vel_right = PyTuple_GetItem(wheel_vel, 1);
		double left = PyFloat_AsDouble(py_vel_left);
		double right = PyFloat_AsDouble(py_vel_right);
		auto linear_vel = WheelAngularVelocity{left, right}.to_linear(Robot3::WHEEL_RADIUS);
		team[i].set_target_wheel_velocity(linear_vel.left, linear_vel.right);
//		printf("%f, %f\n", linear_vel.left, linear_vel.right);
	}
//	std::cout << std::endl;

//	Delete python objects
//	Py_DecRef(velocities);
//	Py_DecRef(ball_pos);
//	Py_DecRef(robot3);
//	Py_DecRef(robot2);
//	Py_DecRef(robot1);
}

AIStrategy::~AIStrategy() {
	Py_DecRef(strategy_method_name);
	Py_DecRef(ai_object);
	Py_DecRef(ai_class);
	Py_DecRef(module_dict);
	Py_DecRef(module);
}

