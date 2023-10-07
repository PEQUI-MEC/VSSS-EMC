#ifndef VSSS_ROBOT3_HPP
#define VSSS_ROBOT3_HPP

#include "Types.hpp"
#include <RobotControl/Control.h>
#include <vision/Tag.hpp>

class Robot3 {
	public:
	Pose pose;
	Target target;

	unsigned int TAG = 0;
	char ID = 'A';
	Role role = Role::None;

	double SIZE = 0.08;
	static constexpr double WHEEL_RADIUS = 0.03;
	double default_target_velocity = 0.8;
	const double TARGET_OFFSET = 0.03; // tolerância para saber se o robô chegou no ponto
	const double BALL_OFFSET = 0.078; // tolerância para saber se a bola está próxima ao robô

	Control control;

	Robot3(unsigned int TAG, char ID) : TAG(TAG), ID(ID), control(pose, target, SIZE){}

	/**	Robô vai para um ponto e continua se movendo com mesma velocidade"
	 *	@param point Ponto em que o robô deve passar
	 *	@param velocity	Velocidade do movimento. Caso não seja definida,
	 *	robô se move com velocidade "default_target_velocity" */
	void go_to(Geometry::Point point, std::optional<double> velocity = std::nullopt);
	/**	Robô vai para um ponto com a orientacao final desejada,
	 *	e continua se movendo com mesma velocidade"
	 *	@param point Ponto em que o robô deve passar
	 *	@param direction Vetor na direcao da orientacão desejada
	 *	@param velocity Velocidade do movimento. Caso não seja definida,
	 *	robô se move com velocidade "default_target_velocity" */
	void go_to_pose(Geometry::Point point,
					Geometry::Vector direction, std::optional<double> velocity = std::nullopt);
	/**	Robô vai para um ponto e fica parado ao chegar
	 *	@param point Ponto em que o robô deve chegar e parar
	 *	@param velocity Velocidade do movimento. Caso não seja definida,
	 *	robô se move com velocidade "default_target_velocity" */
	void go_to_and_stop(Geometry::Point point, std::optional<double> velocity = std::nullopt);
	void go_to_and_stop_orientation(Geometry::Point point, double orientation, std::optional<double> velocity = std::nullopt);
	/**	Robô se move na direcao do vetor
	 *	@param vector Vetor que define a orientacao
	 *	em que o robô deve se mover
	 *	@param velocity Velocidade do movimento. Caso não seja definida,
	 *	robô se move com velocidade "default_target_velocity" */
	void go_in_direction(Geometry::Vector vector, std::optional<double> velocity = std::nullopt);
	/**	Robô continua parado no mesmo lugar, com orientacao igual
	 *	a do vetor definido
	 *	@param orientation Vetor que define a orentacao desejada do robô */
	void set_target_orientation(Geometry::Vector orientation);
	/**	Robô gira com a velocidade angular desejada
	 *	@param angular_velocity Velocidade angular do giro do robô
	 *	Valores positivos significam giro anti-horário,
	 *	valores negativos significam giro horário */
	void spin(double angular_velocity);
	/**	Robô se movimenta com as velocidades de rodas desejadas
	 *	@param left Velocidade da roda esquerda
	 *	@param right Velocidade da roda direita */
	void set_target_wheel_velocity(double left, double right);
	/** Para o robô **/
	void stop() { spin(0); };
	/** Retorna a direção do movimento do robô **/
	Geometry::Vector get_direction();

    void spin_kick_to_target(Geometry::Point ball, Geometry::Point target);

	void send_no_command() {
		target.command = Command::None;
	}

	void set_pose(cv::Point position, double orientation, bool invert) {
		pose.position = Geometry::from_cv_point(position).inverted_coordinates(invert);
		if (invert) {
			pose.orientation = Geometry::wrap(-orientation + M_PI);
		} else {
			pose.orientation = -orientation;
		}
	}

	Geometry::Point get_position() const {
		return pose.position;
	}

	PyObject * python_pose() const {
		auto position = pose.position;
		auto pyrobot = Py_BuildValue("[ddd]", position.x, position.y, pose.orientation);
		return pyrobot;
	}

	std::string tag_name() {
		return "Robot " + std::to_string(TAG);
	}

	Tag to_tag(bool invert) const;
	Geometry::Point position_in_seconds(double time) const;
};

#endif //VSSS_ROBOT3_HPP
