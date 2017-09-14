#ifndef STRATEGY_HPP_
#define STRATEGY_HPP_
#define PI 3.14159265453
#include "opencv2/opencv.hpp"
#include "LS.cpp"
#include "../pack-capture-gui/capture-gui/Robot.hpp"
#include "SerialW.hpp"
#include <math.h>
#include "Ann.hpp"

#define PREDICAO_NUM_SAMPLES 15
// Parametros para atacante sem bola

#define ENTRADA_GOL_INVERSO	1

//Parametros para atacante com bola
#define AVOID_GOLEIRO_APOLO		0

// Parametros do Defensor com bola no ataque
#define DEF_RATIO			1.0f

// Parametros do Defensor na defesa
#define ZAGA_CENTRALIZADA			1
//role
#define GOALKEEPER 0
#define DEFENDER 1
#define ATTACKER 2
#define OPPONENT 3

//cmdType
#define POSITION 0
#define SPEED 1
#define ORIENTATION 2

//state
#define NORMAL_STATE 0
#define CORNER_STATE 1
#define	STEP_BACK 2
#define	ADVANCING_STATE 3
#define	TRANSITION_STATE 4





class Strategy
{
public:
	Ann * ann;
	bool useAnn_flag = false;

	// Robot Goalkeeper;
	// Robot Attack;
	// Robot Defense;
	// Robot Opponent;
	cv::Point Ball;
	cv::Point FutureBall;
	int distBall;
	LS LS_ball_x;
	LS LS_ball_y;
	cv::Point Ball_Est;
	cv::Point target = cv::Point(-1,-1);
	cv::Point targetlock = cv::Point(-1,-1);
	int height, width;
	int COMPRIMENTO_CAMPO_TOTAL;
	int COMPRIMENTO_PISTA;
	int LARGURA_CAMPO 	;
	int TAMANHO_GOL  	;
	int TAMANHO_AREA  	;
	bool GOAL_DANGER_ZONE = false;
	bool ADV_NA_AREA = false;
	int BANHEIRA		;
	int DIVISAO_AREAS	;
	int OFFSET_BANHEIRA	;
	int MEIO_GOL_X		;
	int MEIO_GOL_Y		;
	int MAX_GOL_Y		;
	int MIN_GOL_Y		;
	int LINHA_ZAGA		;
	int LIMITE_AREA_X	;
	// Parametros para atacante sem bola
	int OFFSET_RATIO	;
	int CONE_RATIO		;
	int COUNT_DECISION  ;
	int MAX_DECISION_COUNT;
	int TARGET_LOCK_COUNT;
	int MAX_TARGET_LOCK_COUNT;
	// Parametros do Defensor na defesa
	int DESLOCAMENTO_ZAGA_ATAQUE	;
	int BALL_RADIUS;


	vector<Robot> robots;
	int collision_count[3];
	cv::Point past_position[3];
	cv::Point Goalkeeper;
	cv::Point Defender;
	cv::Point Attacker;
	int atk, def, gk, opp;


	bool half_transition = false;
	bool full_transition = false;
	bool danger_zone_1 = false;
	bool danger_zone_2 = false;
	bool transition_enabled = true;
	bool atk_ball_possession = false;

	int ABS_PLAYING_FIELD_WIDTH,
		ABS_GOAL_TO_GOAL_WIDTH,
		COORD_MID_FIELD_X,
		ABS_FIELD_HEIGHT,
		ABS_GOAL_SIZE_Y,
		ABS_GOAL_SIZE_X,
		COORD_GOAL_DEF_FRONT_X,
		COORD_GOAL_ATK_FRONT_X,
		COORD_GOAL_MID_Y,
		COORD_GOAL_UP_Y,
		COORD_GOAL_DWN_Y,
		ABS_BOX_SIZE_Y,
		ABS_BOX_SIZE_X,
		COORD_BOX_DEF_X,
		COORD_BOX_ATK_X,
		COORD_BOX_UP_Y,
		COORD_BOX_DWN_Y,
		ABS_ROBOT_SIZE;

	int corner_atk_limit,
		def_line,
		possession_distance,
		max_approach,
		fixed_pos_distance,
		collision_radius,
		max_collision_count;




	Strategy()
	{
		LS_ball_x.init(PREDICAO_NUM_SAMPLES,1);
		LS_ball_y.init(PREDICAO_NUM_SAMPLES,1);
	}

	void set_constants(int w, int h) {
		width = w;
		height = h;

		ABS_PLAYING_FIELD_WIDTH = round(1.50*float(width)/1.70);
		ABS_GOAL_TO_GOAL_WIDTH = width;
		ABS_ROBOT_SIZE = round(0.08*float(width)/1.70);
		COORD_MID_FIELD_X = ABS_GOAL_TO_GOAL_WIDTH/2;

		ABS_FIELD_HEIGHT = height;
		ABS_GOAL_SIZE_Y = round(0.40*float(height)/1.30);
		ABS_GOAL_SIZE_X = round(0.10*float(width)/1.70);
		COORD_GOAL_DEF_FRONT_X = ABS_GOAL_SIZE_X;
		COORD_GOAL_ATK_FRONT_X = ABS_GOAL_TO_GOAL_WIDTH - ABS_GOAL_SIZE_X;
		COORD_GOAL_MID_Y = ABS_FIELD_HEIGHT/2;
		COORD_GOAL_UP_Y = ABS_FIELD_HEIGHT/2 - ABS_GOAL_SIZE_Y/2;
		COORD_GOAL_DWN_Y = ABS_FIELD_HEIGHT/2 + ABS_GOAL_SIZE_Y/2;

		ABS_BOX_SIZE_Y = round(0.70*float(height)/1.30);
		ABS_BOX_SIZE_X = round(0.15*float(width)/1.70);
		COORD_BOX_DEF_X = ABS_GOAL_SIZE_X + ABS_BOX_SIZE_X;
		COORD_BOX_ATK_X = ABS_GOAL_TO_GOAL_WIDTH - (ABS_GOAL_SIZE_X + ABS_BOX_SIZE_X);
		COORD_BOX_UP_Y = round((ABS_FIELD_HEIGHT - ABS_BOX_SIZE_Y)/2);
		COORD_BOX_DWN_Y = round(ABS_BOX_SIZE_Y + (ABS_FIELD_HEIGHT - ABS_BOX_SIZE_Y)/2);

		//variáveis ajustáveis
		corner_atk_limit = COORD_BOX_ATK_X - ABS_ROBOT_SIZE;
		def_line = ABS_GOAL_TO_GOAL_WIDTH/3;
		possession_distance = ABS_ROBOT_SIZE;
		collision_radius = ABS_ROBOT_SIZE/2;
		fixed_pos_distance = ABS_ROBOT_SIZE/2;
		max_approach = ABS_ROBOT_SIZE*5;
		max_collision_count = 15;


		//não usando
		BANHEIRA		=	round((0.50*COMPRIMENTO_CAMPO_TOTAL))+round(0.16*float(width)/1.70);
		DIVISAO_AREAS	=	round((0.50*COMPRIMENTO_CAMPO_TOTAL) - 10); // O valor negativo é um offset para não ficar exatamente no meio.
		OFFSET_BANHEIRA	=	round(0.20*float(width)/1.70);
		MEIO_GOL_X		=	round(COMPRIMENTO_CAMPO_TOTAL-round(0.05*float(width)/1.70));
		MEIO_GOL_Y		=	round(LARGURA_CAMPO/2);
		MAX_GOL_Y		=	round(MEIO_GOL_Y + TAMANHO_GOL/2);
		MIN_GOL_Y		=	round(MEIO_GOL_Y - TAMANHO_GOL/2);
		LINHA_ZAGA		=	round(0.30*COMPRIMENTO_CAMPO_TOTAL);

		// Parametros para atacante sem bola
		OFFSET_RATIO	=	round(0.12*float(width)/1.70);
		CONE_RATIO		=	round(0.8*float(width)/1.70);
		MAX_DECISION_COUNT = 10;
		COUNT_DECISION = MAX_DECISION_COUNT;
		TARGET_LOCK_COUNT = 10;
		MAX_TARGET_LOCK_COUNT = 10;
		// Parametros do Defensor na defesa
		DESLOCAMENTO_ZAGA_ATAQUE	=	round(1.3*float(width)/1.70);
		BALL_RADIUS = 100;
		//não usando
	}

	double transformOrientation(double robotOrientation, double targetOrientation) {
		return (robotOrientation - targetOrientation);
	}

	double distance(cv::Point A, cv::Point B) {
		double dist = sqrt(pow(double(A.y - B.y), 2) + pow(double(A.x - B.x), 2));
		return dist;
	}

	void get_targets(vector<Robot> * pRobots) {

			robots = * pRobots;
			// cout << "vector copied" << endl;

			for(int i =0; i<3; i++) { //pegar posições e índices
				robots[i].cmdType = POSITION;
				robots[i].fixedPos = false;

				switch (robots[i].role)	{
					case GOALKEEPER:
					Goalkeeper = robots[i].position;
					gk = i;
		      break;

					case DEFENDER:
					Defender = robots[i].position;
					def = i;
		      break;

		      case ATTACKER:
					Attacker = robots[i].position;
					atk = i;
		      break;
				}
			}
			// cout << "indexes set" << endl;

			set_flags(); // analisar situação atual e setar flags
			// cout << "flags set" << endl;

			danger_zone_1 = false; //só para testes com um robô
			danger_zone_2 = false; //só para testes com um robô
			transition_enabled = false; //só para testes com um robô

			gk_routine(gk);
			def_routine(def);
			atk_routine(atk);
			// test_run(atk);

			// cout << "routines end" << endl;

			if(half_transition && transition_enabled) {
				half_transition = false;
				transition_enabled = false;
				for(int i =0; i<3; i++) {
					robots[i].status = NORMAL_STATE;
					switch (robots[i].role) {
						case GOALKEEPER:
						break;
						case DEFENDER:
						robots[i].role = ATTACKER;
						break;
						case ATTACKER:
						robots[i].role = DEFENDER;
						robots[i].status = TRANSITION_STATE;
						break;
					}
				}
			}
			if(full_transition && transition_enabled) {
				full_transition = false;
				transition_enabled = false;
				for(int i =0; i<3; i++) {
					robots[i].status = NORMAL_STATE;
					switch (robots[i].role) {
						case GOALKEEPER:
						robots[i].role = ATTACKER;
						break;
						case DEFENDER:
						robots[i].role = GOALKEEPER;
						break;
						case ATTACKER:
						robots[i].role = DEFENDER;
						break;
					}
				}
			}
			// cout << "transitions" << endl;
			for(int i =0; i<3; i++) {
				fixed_position_check(i);
				// cout << "fixed position checked" << endl;
				collision_check(i);
			}
		// devolve o vetor de robots com as alterações
		*pRobots = robots;
		// cout << "passou ponteiro" << endl;
	} // get_targets

	void set_flags () {
		danger_zone_1 = false;
		danger_zone_2 = false;
		atk_ball_possession = false;
		if(Ball.x > COORD_MID_FIELD_X) {
			transition_enabled = true;
		}
		// cout << "transition check" << endl;

		if(Ball.x < Attacker.x && Ball.x > Defender.x) { // entre defensor e atacante
			danger_zone_1 = true;
		} else if(Ball.x < Attacker.x && Ball.x < Defender.x) { // entre defensor e goleiro
			danger_zone_2 = true;
		}
		// cout << "danger zones check" << endl;

		double ball_angle = double(atan(double(Ball.y - Attacker.y)/double(Ball.x - Attacker.x))); // ângulo da bola em relação ao robô
		// cout << "calculo ball angle" << endl;

		double diff_angle = double(transformOrientation(robots[atk].orientation, ball_angle)); // deslocamento angular necessário
		// cout << "calculo diff angle " << endl;

		if(Ball.x > Attacker.x && distance(Ball, Attacker) <= possession_distance &&
		((diff_angle < PI/4 && diff_angle > -PI/4) || (diff_angle > 5*PI/4 && diff_angle < -5*PI/4)) ) { // ângulos toleráveis do robô, considerando ir de costas
			atk_ball_possession = true;
		}
		// cout << "ball possession check" << endl;
	}

	void fixed_position_check(int i) { // Para posição fixa
		if(robots[i].fixedPos) {
			if (distance(robots[i].target, robots[i].position) <= fixed_pos_distance) {
				robots[i].target = robots[i].position;
			}
		}
	}

	void collision_check(int i) {
		if(!robots[i].fixedPos) {
			if(distance(robots[i].position, past_position[i]) <= collision_radius) {
				collision_count[i]++;
			} else {
				past_position[i] = robots[i].position;
				collision_count[i] = 0;
			}
			if(collision_count[i] >= max_collision_count) {
				robots[i].target.x = COORD_MID_FIELD_X;
				robots[i].target.y = COORD_GOAL_MID_Y;
			}
		} else {
			collision_count[i] = 0;
		}
		// cout << " past " << past_position[i] << " count "<< collision_count[i] << endl;
	}

	bool set_ann(const char * annName) {
		// bool pra fazer alguma verificação se deu certo (no futuro)
		ann = new Ann(annName);
		return true;
	}

	cv::Point go_to_the_ball (cv::Point agent) {

		double dist_ball_goal = sqrt(pow(double(COORD_GOAL_MID_Y - Ball.y), 2) + pow(double(COORD_GOAL_ATK_FRONT_X - Ball.x), 2));
		double dist_agent_goal = sqrt(pow(double(COORD_GOAL_MID_Y - agent.y), 2) + pow(double(COORD_GOAL_ATK_FRONT_X - agent.x), 2));
		double dist_agent_ball = sqrt(pow(double(agent.y - Ball.y), 2) + pow(double(agent.x - Ball.x), 2));

		double angle = acos(( double(pow(dist_agent_goal, 2) + pow(dist_ball_goal, 2) - pow(dist_agent_ball, 2)) )/
											double((2 * dist_agent_goal * dist_ball_goal)) );
		// lei dos cossenos para encontrar o ângulo entre as seguintes retas: - do meio do gol à bola
		// 																																	  - do meio do gol ao robô
		// cout << "calculos de distancia e angulos" << endl;

		double approach = double(max_approach) * angle/(PI/2); // O alvo muda de acordo com o ângulo, quanto menor, mais próximo da bola
		// cout << "angle "<< angle << " approach "<< approach << endl;

		target.x = round(Ball.x - approach * cos(atan(abs(double(COORD_GOAL_MID_Y - Ball.y))/abs(double(COORD_GOAL_ATK_FRONT_X - Ball.x)))));

		// cout << "calculos aproximação e alvo" << endl;

		if(Ball.y > COORD_GOAL_MID_Y) {
			target.y = round(Ball.y + approach * sin(atan(abs(double(COORD_GOAL_MID_Y - Ball.y))/abs(double(COORD_GOAL_ATK_FRONT_X - Ball.x)))));
		} else {
			target.y = round(Ball.y - approach * sin(atan(abs(double(COORD_GOAL_MID_Y - Ball.y))/abs(double(COORD_GOAL_ATK_FRONT_X - Ball.x)))));
		}
		// o alvo é posicionado na mesma reta, do meio do gol à bola
		// por isso a utilização de seno e cosseno para encontrar as componentes X e Y
		// os sinais em Y mudam de acordo com a metade do campo em que a bola se encontra
		// uma vez que a reta alvo estará para cima ou para baixo

		if(target.y < 0) target.y = 0;
		if(target.y > ABS_FIELD_HEIGHT) target.y = ABS_FIELD_HEIGHT;

		//  cout << "Tx "<< target.x << " Ty "<< target.y << endl;

		return target;
	}

	cv::Point around_the_ball (cv::Point agent) {
		// serve como um estado anterior ao go_to_the_ball, para quando o robô se encontra a frente da bola
		target.x = Ball.x;

		if (agent.y > Ball.y) target.y = Ball.y + max_approach;
		else target.y = Ball.y - max_approach;

		return target;
	}

	double look_at_ball(int i) {
		robots[i].cmdType = ORIENTATION;
		double target_angle = atan(double(Ball.y - robots[i].position.y)/double(Ball.x - robots[i].position.x)); // ângulo da bola em relação ao robô
		double turn_angle = transformOrientation(double(robots[i].orientation), target_angle); // deslocamento angular necessário
		// cout << "alvo " << target_angle << " orientação " << robots[i].orientation << " turn_angle "<< turn_angle << endl;
		return turn_angle;
	}

	// void reach_point(int i, cv::Point target_point) {
	// 	if(distance(target_point, robots[i].position) > ABS_ROBOT_SIZE) {
	// 		robots[i].target = target_point;
	// 	}
	// }

	void spin_left(int i){
		robots[i].cmdType = SPEED;
		robots[i].Vr = robots[i].vmax;
		robots[i].Vl = - robots[i].vmax;
	}

	void spin_right(int i){
		robots[i].cmdType = SPEED;
		robots[i].Vr = - robots[i].vmax;
		robots[i].Vl = robots[i].vmax;
	}

	void def_wait(int i) {
		if (Ball.y > COORD_GOAL_MID_Y) {
			if (sqrt(pow(double(COORD_GOAL_DWN_Y - robots[i].position.y), 2) + pow(double(def_line - robots[i].position.x), 2)) < ABS_ROBOT_SIZE) {
				robots[i].transOrientation = look_at_ball(i);
			} else {
				robots[i].target.x = def_line;
				robots[i].target.y = COORD_GOAL_DWN_Y;
			}
		} else {
			if (sqrt(pow(double(COORD_GOAL_UP_Y - robots[i].position.y), 2) + pow(double(def_line - robots[i].position.x), 2)) < ABS_ROBOT_SIZE) {
				robots[i].transOrientation = look_at_ball(i);
			} else {
				robots[i].target.x = def_line;
				robots[i].target.y = COORD_GOAL_UP_Y;
			}
		}
		// cout << robots[i].target.x << " x " << robots[i].target.y << " y "<< endl;
	}

	void test_run(int i) {
		robots[i].target = Ball;
	}

	void atk_routine(int i) {

		switch (robots[i].status) {

			case NORMAL_STATE:
				if(Ball.x > corner_atk_limit && (Ball.y > COORD_GOAL_DWN_Y || Ball.y < COORD_GOAL_UP_Y)) {
					robots[i].status = CORNER_STATE; // bola no canto
					// cout << "CORNER_STATE " << endl;
				}
 				if(atk_ball_possession) {
					robots[i].status = ADVANCING_STATE; // ir para o gol
					// cout << "ADVANCING_STATE " << endl;
				} else {
					robots[i].target = go_to_the_ball(robots[i].position);
					// cout << "go_to_the_ball " << endl;
				}
			break;

			case CORNER_STATE:
				if(Ball.x > corner_atk_limit) {
					if(Ball.y < COORD_GOAL_MID_Y) { // acima ou abaixo do gol, para saber para qual lado girar
						if(Ball.y < COORD_GOAL_UP_Y) { // acima ou abaixo da trave, escolher o giro para levar a bola para o gol ou para faze-la entrar
							if(Ball.x > robots[i].position.x && distance(Ball, robots[i].position) < ABS_ROBOT_SIZE) spin_right(i); // giro que leva a bola ao gol
							else robots[i].target = Ball; //!!! testar pode dar ruim com a troca
						} else {
							if(Ball.y > robots[i].position.y && distance(Ball, robots[i].position) < ABS_ROBOT_SIZE) spin_left(i); // giro para faze-la entrar
							else robots[i].target = Ball;
						}
					} else {
						if(Ball.y > COORD_GOAL_DWN_Y) {
							if(Ball.x > robots[i].position.x && distance(Ball, robots[i].position) < ABS_ROBOT_SIZE) spin_left(i);
							else robots[i].target = Ball;
						} else {
							if(Ball.y > robots[i].position.y && distance(Ball, robots[i].position) < ABS_ROBOT_SIZE) spin_right(i); // giro para faze-la entrar
							else robots[i].target = Ball;
						}
					}
				} else {
					robots[i].status = NORMAL_STATE; // voltar ao estado normal
				}
			break;

			case ADVANCING_STATE:
				robots[i].target.x = ABS_GOAL_TO_GOAL_WIDTH;
				robots[i].target.y = COORD_GOAL_MID_Y;
				if(!atk_ball_possession) robots[i].status = NORMAL_STATE;
			break;
		}

	}

	void def_routine(int i) {

		switch (robots[i].status) {

			case NORMAL_STATE:
				if(danger_zone_1) half_transition = true;
				else if(danger_zone_2) robots[i].status = STEP_BACK;
				else def_wait(i);
			break;

			case STEP_BACK:
				if(Ball.y > COORD_GOAL_MID_Y) {
					robots[i].target.y = COORD_BOX_UP_Y;
					robots[i].target.x = COORD_BOX_DEF_X;
				} else {
					robots[i].target.y = COORD_BOX_DWN_Y;
					robots[i].target.x = COORD_BOX_DEF_X;
				}
			break;

			case TRANSITION_STATE:
				robots[i].target.x = COORD_MID_FIELD_X;
				if(Ball.y < robots[i].position.y) {
					robots[i].target.y = ABS_FIELD_HEIGHT;
				} else {
					robots[i].target.y = 0;
				}
				if(distance(Ball, robots[i].position) > ABS_ROBOT_SIZE*4) {
					robots[i].status = NORMAL_STATE;
				}
			break;
		}
	}

	void gk_routine(int i) {

		switch (robots[i].status) {

			case NORMAL_STATE:
			robots[i].fixedPos = true;
			robots[i].target.x = COORD_GOAL_DEF_FRONT_X + ABS_ROBOT_SIZE;
			robots[i].target.y = Ball.y;
			if(Ball.y > COORD_GOAL_DWN_Y) robots[i].target.y = COORD_GOAL_DWN_Y;
			else if (Ball.y < COORD_GOAL_UP_Y) robots[i].target.y = COORD_GOAL_UP_Y;
			if(danger_zone_2) robots[i].status = ADVANCING_STATE;
			break;

			case ADVANCING_STATE:
			robots[i].target = Ball;
			if(robots[i].position.x > COORD_BOX_DEF_X || robots[i].position.y < COORD_BOX_UP_Y || robots[i].position.y > COORD_BOX_DWN_Y) {
				full_transition = true;
			}
			break;
		}
	}



cv::Point get_Ball_Est() {
	return Ball_Est;
}
void set_Ball_Est(cv::Point b) {
	Ball_Est =  b;
}
void set_Ball(cv::Point b) {
	Ball = b;
	LS_ball_x.addValue(Ball.x);
	LS_ball_y.addValue(Ball.y);

	Ball_Est.x =  LS_ball_x.estimate(8);
	Ball_Est.y =  LS_ball_y.estimate(8);

}

};
#endif /* STRATEGY_HPP_ */
