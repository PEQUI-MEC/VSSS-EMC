#ifndef VSSS_ATTACKER_H
#define VSSS_ATTACKER_H

#include "Robot2.h"
#include "Geometry/Geometry.h"

class Attacker : Robot2 {
	private:
		enum Behavior {
			UVF_To_Goal, Spin_Shot, Crossing, Atk_Mindcontrol
		};

		void execute_behavior(Behavior behavior);

		/**	Robô vai para a bola usando o UVF
		 *	@param ball posição da bola
		 *	robô se move com velocidade "default_target_velocity" */
		void uvf_to_goal(Geometry::Point ball);

		/** Robô gira com sentido horário ou anti-horário dependendo da posição da bola
		 *	@param ball posição da bola
		 */
		void spin_shot(Geometry:: Point ball);

		/**	Robô trava a bola no canto e faz um cruzamento
		 * @param ball posição da bola
		*/
		void crossing(Geometry::Point ball);

		/**	Chuta em velocidade máxima carregando a bola ("torar o pau")
		 *	@param ball_to_goal vetor travado na direção e sentido que o robô terá que correr
		 */
        void atk_mindcontrol(Geometry::Vector ball_to_goal);
};

#endif //VSSS_ATTACKER_H
