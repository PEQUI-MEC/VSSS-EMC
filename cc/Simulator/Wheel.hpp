#ifndef VSSS_WHEEL_HPP
#define VSSS_WHEEL_HPP

class Wheel {
	struct PID {
		double kp = 2.5;
		double ki = 2.5;
		double kd = 0.001;
	};

	PID pid{};
	double prev_error = 0;
	double error_acc = 0;

	public:
	double target_vel = 0;

	double pid_step(double vel, double time) {
		if (target_vel == 0) {
			prev_error = 0;
			error_acc = 0;
			return 0;
		}
		double error = target_vel - vel;
//		if (error > 0.4) error = 0.4;
//		else if (error < -0.4) error = -0.4;
		double deriv_error = (error - prev_error) / time;
		error_acc += error * time;
		if (error_acc > 0.5) error_acc = 0.5f;
		if (error_acc < -0.5) error_acc = -0.5f;
		prev_error = error;
		auto output = pid.kd * deriv_error + pid.ki * error_acc + pid.kp * error;
		if (output > 2) { return 2; }
		if (output < -2) { return -2; }
		else return output;
	}
};

#endif //VSSS_WHEEL_HPP
