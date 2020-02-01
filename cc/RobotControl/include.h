#ifndef VSSS_SIMU_INCLUDE_H
#define VSSS_SIMU_INCLUDE_H

#include <cmath>
#include <ostream>

#define PI 3.1415926f

constexpr float field_size_x = 1.7f;
constexpr float field_size_y = 1.3f;

namespace Control {

	struct PID {
		float kp = 2.5;
		float ki = 2.5;
		float kd = 0.001;
	};

	class Wheel {
			PID pid{};
			float prev_error = 0;
			float error_acc = 0;

		public:
			float target_vel = 0;

			float pid_step(float vel, float time) {
				if (target_vel == 0) {
					prev_error = 0;
					error_acc = 0;
					return 0;
				}
				float error = target_vel - vel;
				float deriv_error = (error - prev_error) / time;
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

	struct WheelVelocity {
		float left = -1;
		float right = -1;

		friend std::ostream &operator<<(std::ostream &os,
										const WheelVelocity &vel) {
			os << "left: " << vel.left << ", right: " << vel.right;
			return os;
		}
	};

	struct Vector {
		float size;
		float orientation;
	};

	struct Point {
		float x;
		float y;

		Point operator+(const Vector &vector) const {
			return Point{x + vector.size * std::cos(vector.orientation),
						 y + vector.size * std::sin(vector.orientation)};
		}

		std::pair<double, double> to_simulator_point(double x, double y) {
			return {(float) x - field_size_x / 2,
					(float) y - field_size_y / 2};
		}

		static Point from_simulator_point(double x, double y) {
			return {(float) x + field_size_x / 2,
					(float) y + field_size_y / 2};
		}

		friend std::ostream &operator<<(std::ostream &os,
										const Point &p) {
			os << "x: " << p.x << ", y: " << p.y;
			return os;
		}
	};

#define ROBOT_SIZE 0.08f

	struct TargetVelocity {
		float v;
		float w;

		WheelVelocity to_wheel_vel() {
			return {v - w * ROBOT_SIZE / 2.0f,
					v + w * ROBOT_SIZE / 2.0f};
		}
	};

	enum class Command {
			None = 0, Position = 1, Vector = 2, UVF = 3, Orientation = 4, Velocity = 5
	};

	struct Target {
		friend std::ostream &operator<<(std::ostream &os, const Target &target) {
			os << "position: " << target.position << " theta: " << target.theta << " velocity: " << target.velocity
			   << " angular_vel: " << target.angular_vel;
			return os;
		}

		Point position = {0, 0};
		float theta = 0;
		float velocity = 0;
		float angular_vel = 0;
	};

	constexpr float wrap(float angle) {
		float theta = std::fmod(angle, 2 * PI);
		if (theta > PI) theta = theta - 2 * PI;
		else if (theta < -PI) theta = theta + 2 * PI;
		return theta;
	}
}

#endif //VSSS_SIMU_INCLUDE_H
