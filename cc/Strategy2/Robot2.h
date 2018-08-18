#ifndef VSSS_ROBOT_H
#define VSSS_ROBOT_H

#include "Geometry/Geometry.h"

class Robot2 {
	public:
		struct Pose {	// Define as variáveis de estado do robô.
			Geometry::Point position;
			double orientation;
			double velocity;
			double angular_velocity;
		};
	public: const double size=0.08;
	private:
		struct UVF_params {	// Parametros utilizados no UVF
			double n;
			double reference_distance;
		};

		enum class Command { // Comandos que o Messenger pode enviar para o robô
			Position, Vector, UVF, Orientation, Angular_Vel, None
		};

		Pose pose;	// Pose atual do robô
		Pose target; //	Objetivo do robô. Uso de variáveis depende do comando utilizado
		Command command = Command::None; // Tipo de comando que será enviado pelo Messenger
		double default_target_velocity = 0.8; // Velocidade padrão do robô
		UVF_params uvf_data = {1, 0.1}; // Parâmetros utilizados no UVF

	public:
		/**	Robô vai para um ponto e continua se movendo com mesma velocidade"
		 *	@param point Ponto em que o robô deve passar
		 *	@param velocity	Velocidade do movimento. Caso não seja definida,
		 *	robô se move com velocidade "default_target_velocity" */
		void go_to(Geometry::Point point, double velocity);
		/**	Robô vai para um ponto com a orientacao final desejada,
		 *	e continua se movendo com mesma velocidade"
		 *	@param point Ponto em que o robô deve passar
		 *	@param direction Vetor na direcao da orientacão desejada
		 *	@param velocity Velocidade do movimento. Caso não seja definida,
		 *	robô se move com velocidade "default_target_velocity" */
		void go_to_pose(Geometry::Point point,
						Geometry::Vector direction, double velocity);
		/**	Robô vai para um ponto e fica parado ao chegar
		 *	@param point Ponto em que o robô deve chegar e parar
		 *	@param velocity Velocidade do movimento. Caso não seja definida,
		 *	robô se move com velocidade "default_target_velocity" */
		void go_to_and_stop(Geometry::Point point, double velocity);
		/**	Robô se move na direcao do vetor
		 *	@param vector Vetor que define a orientacao
		 *	em que o robô deve se mover
		 *	@param velocity Velocidade do movimento. Caso não seja definida,
		 *	robô se move com velocidade "default_target_velocity" */
		void go_in_direction(Geometry::Vector vector, double velocity);
		/**	Robô continua parado no mesmo lugar, com orientacao igual
		 *	a do vetor definido
		 *	@param orientation Vetor que define a orentacao desejada do robô */
		void set_target_orientation(Geometry::Vector orientation);
		/**	Robô gira com a velocidade angular desejada
		 *	@param angular_velocity Velocidade angular do giro do robô
		 *	Valores positivos significam giro anti-horário,
		 *	valores negativos significam giro horário */
		void spin(double angular_velocity);
		/**	@return Pose atual do robô	*/
		Pose get_pose() const;

		/**	Fazem o mesmo que foi descrito nos métodos acima,
		 *	mas utilzam a velocidade padrão (definida na variável default_target_velocity) **/
		void go_to(Geometry::Point point);
		void go_to_pose(Geometry::Point point, Geometry::Vector direction);
		void go_to_and_stop(Geometry::Point point);
		void go_in_direction(Geometry::Vector vector);
};

#endif //VSSS_ROBOT_H
