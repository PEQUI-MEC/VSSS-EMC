#ifndef VSSS_WHEEL_HPP
#define VSSS_WHEEL_HPP

class Wheel {
	struct PID {
		double kp = -0.21;
		double ki = 0.0;
		double kd = 0.0;
	};
	double ka = 0.1;

	PID pid{};
	double prev_error = 0;
	double error_acc = 0;
	
	const double MAX_ACC_ERROR = 0.3f;

	double prev_vel = 0;
	double max_wheel_accel = 1.18; // m/s^2

	public:
	double target_vel = 0;

	double pid_step(double vel, double time) {
		if (target_vel == 0) {
			prev_error = 0;
			error_acc = 0;
			return 0;
		}
		double max_vel = prev_vel + 25 * time;

		// double target = target_vel;
		// if (target > max_vel) {
		// 	target = max_vel;
		// }

		double error = target_vel - vel;
		// std::cout << vel << ", " << max_vel << std::endl;
		// double max_error = (max_vel - vel);
		// if (error > max_error) {
		// 	error = max_error;
		// }
//		if (error > 0.4) error = 0.4;
//		else if (error < -0.4) error = -0.4;
		double deriv_error = (vel - prev_vel) / time;
		error_acc += error * time;
		// if (error_acc > MAX_ACC_ERROR) error_acc = MAX_ACC_ERROR;
		// if (error_acc < -MAX_ACC_ERROR) error_acc = -MAX_ACC_ERROR;
		prev_error = error;
		prev_vel = vel;
		auto output = pid.kd * deriv_error + pid.ki * error_acc + pid.kp * error + target_vel;
		if (output > max_vel) {
			double error_before = error_acc;
			error_acc = error_acc - ka * (output - max_vel);
			int sign_before = error_before > 0 ? 1 : -1;
			int sign_after = error_acc > 0 ? 1 : -1;
			if (sign_before != sign_after) {
				error_acc = 0;
			}
		}
		if (output > 2) { return 2; }
		if (output < -2) { return -2; }
		else return output;
	}
};

#endif //VSSS_WHEEL_HPP
