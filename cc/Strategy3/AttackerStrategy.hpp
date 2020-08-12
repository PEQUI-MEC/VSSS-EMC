#ifndef VSSS_ATTACKERSTRATEGY_H
#define VSSS_ATTACKERSTRATEGY_H

#include "boost/variant.hpp"
#include "Robot2.h"
#include "Geometry/Geometry.h"
#include "Field.h"
#include "Robot3.hpp"
#include "RoleStrategy.hpp"
#include "Ball.hpp"

class AttackerStrategy : public RoleStrategy {
	public:
	enum class UvfState {
		seek_ball,
		close_to_ball,
		has_ball,
	};
	UvfState uvf_state = UvfState::seek_ball;

	void run_strategy(Ball& ball);

	/**	Robô vai para a bola usando o UVF
	 *	@param ball posição da bola
	 *	robô se move com velocidade "default_target_velocity" */
	void uvf_to_goal(const Geometry::Point &ball, const Geometry::Point &ball_est);

	/** Robô gira com sentido horário ou anti-horário dependendo da posição da bola
	 *	@param ball posição da bola
	 */
	void spin_shot(const Geometry::Point &ball);

	/**	Robô trava a bola no canto e faz um cruzamento
	 * @param ball posição da bola
	*/
	void crossing(const Geometry::Point &ball);

	/**	Robô vai para a bola na lateral e gira no sentido do gol
	 * @param ball posição da bola
	*/
	void side_spin_shot(Geometry:: Point ball);


	void protect_goal(const Geometry::Point& ball);
	void charged_shot(const Geometry::Point& ball);
	void exit_goal(const Geometry::Point &ball);

	Role get_role() {
		return Role::Attacker;
	}

	private:
	void decide_spin_shot(const Geometry::Point &ball);
};

#endif //VSSS_ATTACKER_H
