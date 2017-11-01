#ifndef STRATEGY_HPP_
#define STRATEGY_HPP_
#define PI 3.14159265453
#include "Fuzzy/FuzzyController.hpp"
#include "opencv2/opencv.hpp"
#include "LS.cpp"
#include "../pack-capture-gui/capture-gui/Robot.hpp"
#include "SerialW.hpp"
#include <math.h>
#include "TestFrame.hpp"

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
#define VECTOR 3
#define VECTORRAW 4

//state
#define NORMAL_STATE 0
#define CORNER_STATE 1
#define	STEP_BACK 2
#define	ADVANCING_STATE 3
#define	TRANSITION_STATE 4
#define	SIDEWAYS 5


//potField
#define BALL_IS_OBS 0
#define BALL_ONLY_OBS 1
#define BALL_NOT_OBS 2
#define NO_OBS 3





class Strategy
{
public:
	TestFrame testFrame;

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

	double pot_thetaX = 0;
	double pot_thetaY = 0;
	double pot_theta = 0;
	double pot_goalTheta = 0;
	double pot_goalMag;
	double pot_magnitude[5];
	double pot_angle[5];
	float ball_angle = 0;
	float ball_mag = 0;

	vector<Robot> robots;
	cv::Point* adv;
	int collision_count[3];
	double past_transangle[3];
	cv::Point past_position[3];
	cv::Point Goalkeeper;
	cv::Point Defender;
	cv::Point Attacker;
	cv::Point Opponent;
	int atk, def, gk, opp;
	double lock_angle;

	FuzzyController controller;

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
		ABS_BALL_SIZE,
		ABS_ROBOT_SIZE;

	int corner_atk_limit,
		def_line,
		possession_distance,
		max_approach,
		fixed_pos_distance,
		collision_radius,
		max_collision_count,
		goalie_line,
		goalie_offset,
		transition_back_radius,
		transition_y_distance,
		acceleration;

	int t = 0;
	int timeout = 0;
	int transition_timeout = 0;
	bool action1 = false;
	bool action2 = false;
	bool action3 = false;
	bool kick = false;
	bool atk_mindcontrol = false;


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
		ABS_BALL_SIZE = round(0.04*float(width)/1.70);
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
		fixed_pos_distance = ABS_ROBOT_SIZE;
		max_approach = ABS_ROBOT_SIZE*2;
		max_collision_count = 30;
		acceleration = 0.8;
		goalie_line = COORD_GOAL_DEF_FRONT_X + ABS_ROBOT_SIZE/2;
		goalie_offset = ABS_ROBOT_SIZE;
		transition_back_radius = ABS_ROBOT_SIZE*3;
		transition_y_distance = ABS_ROBOT_SIZE*4;

		fuzzy_init();

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

	double distance(cv::Point A, cv::Point B) {
		double dist = sqrt(pow(double(A.y - B.y), 2) + pow(double(A.x - B.x), 2));
		return dist;
	}

	double distance_meters(cv::Point A, cv::Point B) {
		double dist = sqrt(pow(double(A.y - B.y), 2) + pow(double(A.x - B.x), 2));
		dist = 1.7*dist/ABS_GOAL_TO_GOAL_WIDTH;
		return dist;
	}

	void get_variables() {
		goalie_line = testFrame.getValue(0); // index da barrinha
		goalie_offset = testFrame.getValue(1); // index da barrinha
	}

	void get_past(int i) {
		past_position[i] = robots[i].position;
		past_transangle[i] = robots[i].transAngle;
	}

	void get_targets(vector<Robot> * pRobots, cv::Point * advRobots) {

			robots = * pRobots;
			adv = advRobots;
			// cout << "vector copied" << endl;

			for(int i =0; i<3; i++) { //pegar posições e índices
				robots[i].cmdType = VECTOR;
				robots[i].fixedPos = false;
				robots[i].using_pot_field = false;
				robots[i].vmax = robots[i].vdefault;

				get_variables();

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

				case OPPONENT:
					Opponent = robots[i].position;
					opp = i;
		      	break;
				}
			}
			// cout << "indexes set" << endl;

			set_flags(); // analisar situação atual e setar flags
			// cout << "flags set" << endl;

			// danger_zone_1 = false; //só para testes com um robô
			// danger_zone_2 = false; //só para testes com um robô
			transition_enabled = false; //só para testes com um robô

			// cout << "transitions" << endl;

			gk_routine(gk);
			def_routine(def);
			atk_routine(atk);
			// test_run(gk);

			// opp_gk_routine(opp);

			overmind();

			transition_timeout--;
			if(transition_timeout < 0) transition_timeout = 0;

			if(half_transition && transition_enabled && transition_timeout == 0) {
				half_transition = false;
				transition_enabled = false;
				transition_timeout = 90; //3 segundos
				for(int i =0; i<3; i++) {
					robots[i].status = NORMAL_STATE;
					switch (robots[i].role) {
						case GOALKEEPER:
						break;
						case DEFENDER:
						robots[i].role = ATTACKER;
						robots[i].vmax = robots[i].vdefault;
						atk = i;
						break;
						case ATTACKER:
						robots[i].role = DEFENDER;
						robots[i].vmax = robots[i].vdefault;
						def = i;
						break;
					}
				}
			}
			if(full_transition && transition_enabled && transition_timeout == 0) {
				full_transition = false;
				transition_enabled = false;
				transition_timeout = 90; //3 segundos
				for(int i =0; i<3; i++) {
					robots[i].status = NORMAL_STATE;
					switch (robots[i].role) {
						case GOALKEEPER:
						robots[i].role = ATTACKER;
						robots[i].vmax = robots[i].vdefault;
						atk = i;
						break;
						case DEFENDER:
						robots[i].role = GOALKEEPER;
						robots[i].vmax = robots[i].vdefault;
						gk = i;
						break;
						case ATTACKER:
						robots[i].role = DEFENDER;
						robots[i].vmax = robots[i].vdefault;
						def = i;
						break;
					}
				}
			}

			// cout << "Ball Speed -> " << distance_meters(Ball, Ball_Est)/0.3333333333  << " m/s" << endl;
			// timeout++;
			// if(timeout >= 15) {
			// 	timeout = 0;
			// 	// cout << "Ball Est -> " <<  Ball_Est.x << " " << Ball_Est.y << " Ball -> " << Ball.x << " " << Ball.y << endl;
			//
			// }

			for(int i =0; i<3; i++) {
				if(!robots[i].using_pot_field) position_to_vector(i);
				fixed_position_check(i);
				collision_check(i);
			}

		// devolve o vetor de robots com as alterações
		*pRobots = robots;
		// cout << "passou ponteiro" << endl;
	} // get_targets

	void overmind() {
		cv::Point goal = cv::Point(COORD_GOAL_ATK_FRONT_X, COORD_GOAL_MID_Y);
		double m1 = double(goal.y - Ball.y)/double(goal.x - Ball.x);
		double m2 = double(robots[atk].position.y - Ball.y)/double(robots[atk].position.x - Ball.x);
		double ball_goal = distance(Ball, goal);
		double r1 = ball_goal + max_approach;

		double mup = double(COORD_GOAL_UP_Y - Ball.y)/double(COORD_GOAL_ATK_FRONT_X - Ball.x);
		double mdwn = double(COORD_GOAL_DWN_Y - Ball.y)/double(COORD_GOAL_ATK_FRONT_X - Ball.x);
		double theta = atan((mup-mdwn)/(1+mup*mdwn));

		double phi = atan((m2-m1)/(1+m2*m1));

		// cout << " Robo1 " << robots[0].role << " Robo2 " << robots[1].role << " Robo3 " << robots[2].role << endl;
		//
		// int trocaIndex = Fuzzy_Troca();
		// switch (trocaIndex) {
		// 	case 0:
		// 	// DO NOTHING
		// 	cout << "não vai troca ninguem!" << endl;
		// 	break;
		// 	case 1: // TROCA TUDO
		// 	full_transition = true;
		// 	cout << "troca tudo nessa porra" << endl;
		// 	break;
		// 	case 2: // TROCA ATKDEF
		// 	half_transition = true;
		// 	cout << "troca atk/def" << endl;
		// 	break;
		// }
		// cout << endl;

		if(robots[atk].cmdType == SPEED || robots[atk].status == CORNER_STATE) atk_mindcontrol = false;

		if(atk_mindcontrol) {
			robots[atk].cmdType = VECTOR;
			// robots[atk].target = goal;
			// if(abs(robots[atk].orientation) > 90) robots[atk].transAngle = 180;
			// else robots[atk].transAngle = 0;
			robots[atk].transAngle = lock_angle;
			robots[atk].vmax = 1.2;
			timeout++;
			if(timeout >= 30) {
				timeout = 0;
				atk_mindcontrol = false;
				robots[atk].vmax = robots[atk].vdefault;
			}
		}

		if((distance(robots[atk].position, Ball) < ABS_ROBOT_SIZE*2) && (phi*180/PI < 20) && (Ball.x > robots[atk].position.x ) &&
		((((robots[atk].orientation - atan(m1))*180/PI) < 20 && ((robots[atk].orientation - atan(m1))*180/PI) > -20) ||
		(((robots[atk].orientation - atan(m1))*180/PI) < -165 || ((robots[atk].orientation - atan(m1))*180/PI) > 165) )
		// 	(robots[atk].orientation > atan(mdwn) && robots[atk].orientation < atan(mup))
	 	) {
			if(!atk_mindcontrol) {
				lock_angle = atan2(double(robots[atk].position.y - Ball.y), - double(robots[atk].position.x - Ball.x));
			}

			atk_mindcontrol = true;
			// cout << "mindcontrol" << endl;
		}
		//  cout << " robot-ball " << distance(robots[atk].position, Ball) <<
		//   " robot size " << ABS_ROBOT_SIZE <<
		//   " angulo m1 " << atan(m1)*180/PI  << " robot.or " << robots[atk].orientation*180/PI  <<
		//   " robot.or - m1 angle " << (robots[atk].orientation - atan(m1))*180/PI <<
		//   " phi " << phi*180/PI << endl;

		//defender mindcontrol
		if(Ball.y > COORD_GOAL_UP_Y && Ball.y < COORD_GOAL_DWN_Y &&
		Ball.x > corner_atk_limit && distance(robots[atk].position, Ball) > ABS_ROBOT_SIZE*2) {
			half_transition = true;
		}



		if(danger_zone_1 && (Ball.x < robots[atk].position.x - ABS_ROBOT_SIZE)) {
			half_transition = true;
		} else if(danger_zone_2) {
			if(Ball.x < COORD_BOX_DEF_X && (Ball.y < COORD_GOAL_UP_Y || Ball.y > COORD_GOAL_DWN_Y) ) full_transition = true;
		} else if(abs(Ball.y - robots[atk].position.y) > transition_y_distance) {
			half_transition = true;
		}

	}

	void set_flags() {
		danger_zone_1 = false;
		danger_zone_2 = false;
		atk_ball_possession = false;
		half_transition = false;
		full_transition = false;
		if(Ball.x > COORD_MID_FIELD_X) {
			transition_enabled = true;
		}
		if(Ball.x < robots[atk].position.x && Ball.x > robots[def].position.x) {
			danger_zone_1 = true;
		} else if (Ball.x < robots[atk].position.x && Ball.x < robots[def].position.x) {
			danger_zone_2 = true;
		}
	}

	void fixed_position_check(int i) { // Para posição fixa
		if(robots[i].fixedPos && (robots[i].cmdType == VECTOR || robots[i].cmdType == POSITION)) {
			robots[i].vmax = robots[i].vdefault * pow(distance(robots[i].position, robots[i].target)/(ABS_GOAL_TO_GOAL_WIDTH/4), 2);
			// cout << "vmax " << robots[i].vmax << " distancia "<< distance(robots[i].position, robots[i].target) << " distancia max " << ABS_GOAL_TO_GOAL_WIDTH/4<<endl;
			if(robots[i].vmax > robots[i].vdefault) robots[i].vmax = robots[i].vdefault;
			if(robots[i].vmax < 0.3) robots[i].vmax = 0.3;

			if (distance(robots[i].target, robots[i].position) <= fixed_pos_distance) {
				robots[i].target = robots[i].position;
				robots[i].vmax = 0;
			}
		}
	}

	void collision_check(int i) {
		if(!robots[i].fixedPos && robots[i].role != GOALKEEPER && robots[i].status != CORNER_STATE) {
			if(distance(robots[i].position, past_position[i]) <= collision_radius) {
				collision_count[i]++;
			} else {
				get_past(i);
				collision_count[i] = 0;
			}
			if(collision_count[i] >= max_collision_count) {
				robots[i].transAngle = atan2(sin(past_transangle[i]+PI),cos(past_transangle[i]+PI));
			}
		} else {
			collision_count[i] = 0;
		}
		// cout << " past " << past_position[i] << " count "<< collision_count[i] << endl;
	}

	bool is_near(int i, cv::Point point) {
		return distance(robots[i].position, point) < ABS_ROBOT_SIZE;
	}

	void position_to_vector(int i) {
		robots[i].transAngle = atan2(double(robots[i].position.y - robots[i].target.y), - double(robots[i].position.x - robots[i].target.x));
	}

	void go_to_the_ball(int i) {

		cv::Point targets_temp;
		cv::Point goal = cv::Point(COORD_GOAL_ATK_FRONT_X, COORD_GOAL_MID_Y);
		double m1 = double(goal.y - Ball.y)/double(goal.x - Ball.x);
		double m2 = double(robots[i].position.y - Ball.y)/double(robots[i].position.x - Ball.x);
		double ball_goal = distance(Ball, goal);
		double r1 = ball_goal + max_approach;

		cv::Point v = cv::Point(goal.x - Ball.x, goal.y - Ball.y);
		double module = sqrt(pow(v.x,2) + pow(v.y,2));

		double phi = atan((m2-m1)/(1+m2*m1));

		targets_temp.x = double(Ball.x - double(v.x/module) * max_approach * abs(phi*180/PI) / 100);
		targets_temp.y = double(Ball.y - double(v.y/module) * max_approach * abs(phi*180/PI) / 100);
		//  cout << "Tx "<< targets_temp.x << " Ty "<< targets_temp.y << " phi*180/PI /100 " << (phi*180/PI) / 100 << endl;

		if(targets_temp.y < 0) targets_temp.y = 0;
		if(targets_temp.y > ABS_FIELD_HEIGHT) targets_temp.y = ABS_FIELD_HEIGHT;

		robots[i].target = targets_temp;
		robots[i].vmax = 0.5 * 4*distance(robots[i].position, targets_temp)/ABS_GOAL_TO_GOAL_WIDTH;
		if (robots[i].vmax > 0.5) robots[i].vmax = 0.5;
	}

	void go_to_the_ball_direct(int i) {

		cv::Point targets_temp;
		cv::Point goal = cv::Point(COORD_GOAL_ATK_FRONT_X, COORD_GOAL_MID_Y);
		double m1 = double(goal.y - Ball.y)/double(goal.x - Ball.x);
		double m2 = double(robots[i].position.y - Ball.y)/double(robots[i].position.x - Ball.x);
		double ball_goal = distance(Ball, goal);
		double r1 = ball_goal + max_approach;

		cv::Point v = cv::Point(goal.x - Ball.x, goal.y - Ball.y);
		double module = sqrt(pow(v.x,2) + pow(v.y,2));

		double phi = atan((m2-m1)/(1+m2*m1));
		// cout << "phi -> " << phi*180/PI << " 180 - phi -> " << 180 - phi*180/PI << endl;

		targets_temp.x = double(Ball.x - double(v.x/module) * max_approach);
		targets_temp.y = double(Ball.y - double(v.y/module) * max_approach);

		if(targets_temp.x > ABS_GOAL_TO_GOAL_WIDTH || targets_temp.x < 0 || targets_temp.y > ABS_FIELD_HEIGHT || targets_temp.y < 0) {
			targets_temp = Ball;
		}
		if (is_near(i, targets_temp)) {
			action1 = true;
		}
		if (look_at_ball(i)*180/PI > robots[i].targetOrientation*180/PI - 10 && look_at_ball(i)*180/PI < robots[i].targetOrientation*180/PI + 10) {
			action2 = true;
		}
		if(phi*180/PI > 45 || phi*180/PI < -45 || distance(robots[i].position, Ball) > ABS_ROBOT_SIZE*3) {
			action1 = false;
			action2 = false;
		}
		// cout << action1 << " actions " << action2 << endl;
		// if(!action1 && !action2) {
		if(!is_near(i, targets_temp)) {
			robots[i].target = targets_temp;
			robots[i].vmax = robots[i].vdefault * 4*distance(robots[i].position, targets_temp)/ABS_GOAL_TO_GOAL_WIDTH;
			if (robots[i].vmax > robots[i].vdefault) robots[i].vmax = robots[i].vdefault;
			// cout << robots[i].target.x << " " << robots[i].target.y << endl;
		}
		// if(action1 && !action2) {
		// 	robots[i].cmdType = ORIENTATION;
		// 	robots[i].targetOrientation = look_at_ball(i);
		// 	// cout << robots[i].targetOrientation*180/PI << " target Or"<< endl;
		// }
		// if(action1 && action2) {
		else {
			robots[i].target = Ball;
		}


	} // incompleto!!! //incompleto!!

	void around_the_ball(int i) {
		// serve como um estado anterior ao go_to_the_ball, para quando o robô se encontra a frente da bola
		robots[i].target.x = Ball.x - max_approach/2;
		if (robots[i].position.y > Ball.y) robots[i].target.y = Ball.y + max_approach/2;
		else robots[i].target.y = Ball.y - max_approach/2;
	}

	double look_at_ball(int i) {
		double target_angle = atan2(double(Ball.y - robots[i].position.y),- double(Ball.x - robots[i].position.x)); // ângulo da bola em relação ao robô
		return target_angle;
	}

	double potField(int robot_index, cv::Point goal, int behavior=BALL_NOT_OBS) {
		robots[robot_index].using_pot_field = true;
		float gain_rep=10;
		float gain_att=1;
		float rep_radius=0.2;
		float ball_rep_radius = 0.3;
		float rot_angle = 0;
		ball_angle = 0;
		ball_mag = 0;
		pot_thetaX = 0;
		pot_thetaY = 0;
		pot_theta = 0;
		pot_goalTheta = 0;
		pot_goalMag = 0;
		for (int i = 0; i < 3; i++) {
			if(distance_meters(robots[robot_index].position, adv[i]) > 0 && distance_meters(robots[robot_index].position, adv[i]) < rep_radius) {
				pot_magnitude[i] = gain_rep*pow(1/(distance_meters(robots[robot_index].position, adv[i]) - 1/rep_radius), 2)/2;
				pot_angle[i] = atan2(double(robots[robot_index].position.y - adv[i].y),- double(robots[robot_index].position.x - adv[i].x));

				if(pot_rotation_decision(robot_index,goal,adv[i])==0)
					pot_magnitude[i] = 0;
				else if(pot_rotation_decision(robot_index,goal,adv[i])>0){
						pot_angle[i] = atan2(sin(pot_angle[i]+PI/2),cos(pot_angle[i]+PI/2));
				//	printf("HORARIO ");
				}else{
					pot_angle[i] = atan2(sin(pot_angle[i]+3*PI/2),cos(pot_angle[i]+3*PI/2));

						//printf("ANTI-HORARIO ");
					}
				//printf("adv[%d] x %d, y %d| magnitude [%d] %f | angle[%d] %f\n",i,adv[i].x,adv[i].y,i,pot_magnitude[i],i,pot_angle[i]*180/PI);
			} else {
				pot_magnitude[i] = 0;
				pot_angle[i] = 0;
			}


		}
		int j = 3;
		for (int i = 0; i < 3; i++) {
			if(i != robot_index) {
				if(distance_meters(robots[robot_index].position, robots[i].position) < rep_radius) {
					pot_magnitude[j] = gain_rep*pow(1/(distance_meters(robots[robot_index].position, robots[i].position) - 1/rep_radius), 2)/2;
					pot_angle[j] = atan2(double(robots[robot_index].position.y - robots[i].position.y),- double(robots[robot_index].position.x - robots[i].position.x));
					if(pot_rotation_decision(robot_index,goal,robots[i].position)==0)
						pot_magnitude[j] = 0;
					else if(pot_rotation_decision(robot_index,goal,robots[i].position)>0){
						pot_angle[j] = atan2(sin(pot_angle[j]+PI/2),cos(pot_angle[j]+PI/2));
					//	printf("HORARIO ");

					}else{
							pot_angle[j] = atan2(sin(pot_angle[j]+3*PI/2),cos(pot_angle[j]+3*PI/2));
						//	printf("ANTI-HORARIO ");
						}
			//	printf("robots[%d] x %d, y %d|magnitude [%d] %f | angle[%d] %f\n",i,robots[i].position.x,robots[i].position.y,j,pot_magnitude[j],j,pot_angle[j]*180/PI);
				}else {
					pot_magnitude[j] = 0;
					pot_angle[j] = 0;
				}

				j++;

			}
		}

		switch(behavior) {
			case BALL_IS_OBS:
			if(distance_meters(robots[robot_index].position, Ball) > 0 && distance_meters(robots[robot_index].position, Ball) < ball_rep_radius) {

				ball_mag = (gain_rep*pow(1/(distance_meters(robots[robot_index].position, Ball) - 1/(ball_rep_radius)), 2)/2 );

				ball_angle = atan2(double(robots[robot_index].position.y - Ball.y),- double(robots[robot_index].position.x - Ball.x));
				if(pot_rotation_decision(robot_index,goal,Ball)==0)
					ball_mag = 0;
				else if(pot_rotation_decision(robot_index,goal,Ball)>0){
					ball_angle = atan2(sin(ball_angle+PI/2),cos(ball_angle+PI/2));
				//	printf("HORARIO ");

				}else{
					ball_angle = atan2(sin(ball_angle+3*PI/2),cos(ball_angle+3*PI/2));
					//	printf("ANTI-HORARIO ");
				}
				pot_thetaY += ball_mag*sin(ball_angle);
				pot_thetaX += ball_mag*cos(ball_angle);

			}else {
				ball_mag = 0;
				ball_angle = 0;
			}
			break;

			case BALL_ONLY_OBS:
				for (int i = 0; i < 5; i++) {
				pot_magnitude[i] = 0;
				}
				if(distance_meters(robots[robot_index].position, Ball) > 0 && distance_meters(robots[robot_index].position, Ball) < ball_rep_radius) {

				ball_mag = (gain_rep*pow(1/(distance_meters(robots[robot_index].position, Ball) - 1/ball_rep_radius), 2)/2 );

				ball_angle = atan2(double(robots[robot_index].position.y - Ball.y),- double(robots[robot_index].position.x - Ball.x));
				if(pot_rotation_decision(robot_index,goal,Ball)==0)
					ball_mag = 0;
				else if(pot_rotation_decision(robot_index,goal,Ball)>0){
					ball_angle = atan2(sin(ball_angle+PI/2),cos(ball_angle+PI/2));
						//	printf("HORARIO ");

				}else{
					ball_angle = atan2(sin(ball_angle+3*PI/2),cos(ball_angle+3*PI/2));
							//	printf("ANTI-HORARIO ");
				}
				pot_thetaY = ball_mag*sin(ball_angle);
				pot_thetaX = ball_mag*cos(ball_angle);
				}else {
					ball_mag = 0;
					ball_angle = 0;
				}
			break;

			case NO_OBS:
			for (int i = 0; i < 5; i++) {
			pot_magnitude[i] = 0;
			}
			break;

			case BALL_NOT_OBS:
			break;
		}

		for (int i = 0; i < 5; i++) {
			pot_thetaY += pot_magnitude[i]*sin(pot_angle[i]);
			pot_thetaX += pot_magnitude[i]*cos(pot_angle[i]);
		}

		pot_goalTheta = atan2(double(robots[robot_index].position.y -goal.y ),- double( robots[robot_index].position.x - goal.x ));
		pot_goalMag = gain_att*pow(distance_meters(robots[robot_index].position, goal), 2)/2;
		//	printf("goalMag %f | goalTheta %f\n",pot_goalMag,pot_goalTheta*180/PI);

		pot_thetaY += pot_goalMag*sin(pot_goalTheta);
		pot_thetaX += pot_goalMag*cos(pot_goalTheta);
		pot_theta = atan2(pot_thetaY, pot_thetaX);
		//pot_theta = robots[robot_index].orientation - pot_theta;
		// pot_theta = atan2(-sin(robots[robot_index].orientation - pot_theta),cos(robots[robot_index].orientation - pot_theta));
		//printf("THETA %f \n",pot_theta*180/PI);
		return pot_theta;
	}

	int pot_rotation_decision(int robot_index,cv::Point goal, cv::Point obst) {
		int HORARIO = 1;
		int ANTI_HORARIO = -1;
		bool goal_ahead = robots[robot_index].position.x > obst.x && obst.x > goal.x;
		bool goal_behind = robots[robot_index].position.x < obst.x && obst.x < goal.x;

		if(obst.y < ABS_ROBOT_SIZE*2) {
			if(goal_ahead) return HORARIO;
			if(goal_behind) return ANTI_HORARIO;
		} else if(obst.y > (ABS_FIELD_HEIGHT - ABS_ROBOT_SIZE*2)) {
			if(goal_ahead) return ANTI_HORARIO;
			if(goal_behind) return HORARIO;
		}
		float angle_Obst_Robot = atan2(double(obst.y - robots[robot_index].position.y),
					- double(obst.x - robots[robot_index].position.x));
		float angle_Obst_Goal = atan2(double(obst.y - goal.y),
					- double(obst.x - goal.x));
		//printf("%f + %f = %f \n",fabs(angle_Obst_Robot)*180/PI,fabs(angle_Obst_Goal)*180/PI,(fabs(angle_Obst_Robot)+fabs(angle_Obst_Goal))*180/PI);
		if(angle_Obst_Robot > 0 && angle_Obst_Robot <= PI/2){ // ROBO NO PRIMEIRO QUAD

			if(angle_Obst_Goal > 0 && angle_Obst_Goal <= PI/2){ // OBJETIVO NO PRIMEIRO QUAD
				return 0;

			} else if(angle_Obst_Goal > PI/2 && angle_Obst_Goal <= PI){ // OBJETIVO NO SEGUNDO QUAD
				return ANTI_HORARIO;

			} else if(angle_Obst_Goal > -PI && angle_Obst_Goal <= -PI/2){// OBJETIVO NO TERCEIRO QUAD
				return HORARIO;

			} else { // OBJETIVO NO QUARTO QUAD
				//printf("DECIDE" );
				if(fabs(angle_Obst_Robot)+fabs(angle_Obst_Goal)<PI)
					return HORARIO;
				else
					return ANTI_HORARIO;
			}

		} else if(angle_Obst_Robot > PI/2 && angle_Obst_Robot <= PI){ // ROBO NO SEGUNDO QUAD

			if(angle_Obst_Goal > 0 && angle_Obst_Goal <= PI/2){ // OBJETIVO NO PRIMEIRO QUAD
				return HORARIO;

			} else if(angle_Obst_Goal > PI/2 && angle_Obst_Goal <= PI){ // OBJETIVO NO SEGUNDO QUAD
				return 0;

			} else if(angle_Obst_Goal > -PI && angle_Obst_Goal <= -PI/2){// OBJETIVO NO TERCEIRO QUAD
				return ANTI_HORARIO;

			} else { // OBJETIVO NO QUARTO QUAD
				//printf("DECIDE" );
				if(fabs(angle_Obst_Robot)+fabs(angle_Obst_Goal)<PI)
					return HORARIO;
				else
					return ANTI_HORARIO;
			}



		} else if(angle_Obst_Robot > -PI && angle_Obst_Robot <= -PI/2){// ROBO NO TERCEIRO QUAD

			if(angle_Obst_Goal > 0 && angle_Obst_Goal <= PI/2){ // OBJETIVO NO PRIMEIRO QUAD
			//	printf("DECIDE" );
				if(fabs(angle_Obst_Robot)+fabs(angle_Obst_Goal)<PI)
					return ANTI_HORARIO;
				else
					return HORARIO;

			} else if(angle_Obst_Goal > PI/2 && angle_Obst_Goal <= PI){ // OBJETIVO NO SEGUNDO QUAD
				return HORARIO;

			} else if(angle_Obst_Goal > -PI && angle_Obst_Goal <= -PI/2){// OBJETIVO NO TERCEIRO QUAD
				return 0;

			} else { // OBJETIVO NO QUARTO QUAD

				return ANTI_HORARIO;
			}



		} else { // ROBO NO QUARTO QUAD

			if(angle_Obst_Goal > 0 && angle_Obst_Goal <= PI/2){ // OBJETIVO NO PRIMEIRO QUAD
				return ANTI_HORARIO;

			} else if(angle_Obst_Goal > PI/2 && angle_Obst_Goal <= PI){ // OBJETIVO NO SEGUNDO QUAD
			//	printf("DECIDE" );
				if(fabs(angle_Obst_Robot)+fabs(angle_Obst_Goal)<PI)
					return ANTI_HORARIO;
				else
					return HORARIO;

			} else if(angle_Obst_Goal > -PI && angle_Obst_Goal <= -PI/2){// OBJETIVO NO TERCEIRO QUAD
				return HORARIO;

			} else { // OBJETIVO NO QUARTO QUAD

				return 0;
			}

		}


	}

	double reach_on_time(double dist, double time_limit = 1) {
		double tmp0 = time_limit + sqrt(pow(time_limit, 2) - 2 * dist/acceleration) * acceleration;
		double tmp1 = time_limit - sqrt(pow(time_limit, 2) - 2 * dist/acceleration) * acceleration;

		if(tmp0 <= 1.1 && tmp0 >= 0.3) return tmp0;
		if(tmp1 <= 1.1 && tmp1 >= 0.3) return tmp1;
		else return 1.0;
	}

	void spin_left(int i) {
		robots[i].cmdType = SPEED;
		robots[i].Vr = robots[i].vmax;
		robots[i].Vl = - robots[i].vmax;
	}

	void spin_right(int i) {
		robots[i].cmdType = SPEED;
		robots[i].Vr = - robots[i].vmax;
		robots[i].Vl = robots[i].vmax;
	}

	void def_wait(int i) {
		robots[i].fixedPos = true;
		if(distance(robots[i].position, robots[i].target) <= fixed_pos_distance) {
			robots[i].cmdType = ORIENTATION;
			robots[i].targetOrientation = look_at_ball(i);
		}
	}

	void fuzzy_init() {
		cout << "init " << endl;

		controller.importRules("cc/Fuzzy/RULES_VSS2017.txt");
		cout << "rules " << endl;

		controller.defineVariables("cc/Fuzzy/Membership_VSS2017.txt");
		cout << "variables " << endl;

	}

	int Fuzzy_Troca() {

				vector<float> input;
				// cout << "vector " << endl;
				if(Ball.x < 0) Ball.x = 0;
				if(Ball.y < 0) Ball.y = 0;
				if(robots[atk].position.x < 0) robots[atk].position.x = 0;
				if(robots[atk].position.y < 0) robots[atk].position.y = 0;
				if(robots[def].position.x < 0) robots[def].position.x = 0;
				if(robots[def].position.y < 0) robots[def].position.y = 0;
				//
				// input.push_back(float(Ball.x * 1.7/(ABS_GOAL_TO_GOAL_WIDTH)));
				// input.push_back(float(Ball.y * 1.3/ABS_FIELD_HEIGHT));
				// input.push_back(float(robots[atk].position.x * 1.7/(ABS_GOAL_TO_GOAL_WIDTH)));
				// input.push_back(float(distance(robots[atk].position, Ball) * 1.7/(ABS_GOAL_TO_GOAL_WIDTH)));
				// input.push_back(float(robots[def].position.x * 1.7/(ABS_GOAL_TO_GOAL_WIDTH)));
				// input.push_back(float(distance(robots[def].position, Ball) * 1.7/(ABS_GOAL_TO_GOAL_WIDTH)));

				// cout << "var " << endl;
				// cout << "XB " << input[0]<<endl;
				// cout << "YB " << input[1]<<endl;
				// cout << "XA " << input[2]<<endl;
				// cout << "DAB " << input[3]<<endl;
				// cout << "XD " << input[4]<<endl;
				// cout << "DDB " << input[5]<<endl;

				input.push_back(float(distance(robots[gk].position, Ball) * 1.7/(ABS_GOAL_TO_GOAL_WIDTH)));
				input.push_back(float(distance(robots[atk].position, Ball) * 1.7/(ABS_GOAL_TO_GOAL_WIDTH)));
				input.push_back(float(distance(robots[def].position, Ball) * 1.7/(ABS_GOAL_TO_GOAL_WIDTH)));


				vector<float>  out = controller.ControladorFuzzy(input);
				std::cout << "resultado: " << out[0] << std::endl;

				if (out.size()>0) {
					switch(int(out[0])){
						case 0:
					//	std::cout << "NÃO TROCA NADA, PORRA: " << out[0] << std::endl;
						break;
						case 1:
					//	std::cout << "TROCA A PORRA TODA: " << out[0] << std::endl;
						break;
						case 2:
					//	std::cout << "TROCA ATK DEF: " << out[0] << std::endl;
						break;

					}


				}
				return out[0];

	}

	double map_range(double actual, double minactual, double maxactual) {
		double maxtarget = 1;
		double mintarget = 0;
		return (actual - minactual) * ((maxtarget-mintarget)/(maxactual-minactual));
	}

	void pot_field_around(int i) {
		robots[i].using_pot_field = true;
		cv::Point targets_temp;
		cv::Point goal = cv::Point(COORD_GOAL_ATK_FRONT_X, COORD_GOAL_MID_Y);
		double m1 = double(goal.y - Ball_Est.y)/double(goal.x - Ball_Est.x);
		double m2 = double(robots[i].position.y - Ball_Est.y)/double(robots[i].position.x - Ball_Est.x);
		double ball_goal = distance(Ball_Est, goal);
		double r1 = ball_goal + max_approach;

		double dist_up = distance(Ball_Est, cv::Point(COORD_GOAL_ATK_FRONT_X, COORD_GOAL_UP_Y));
		double dist_dwn = distance(Ball_Est, cv::Point(COORD_GOAL_ATK_FRONT_X, COORD_GOAL_DWN_Y));

		double beta = acos( (pow(dist_up,2)+pow(dist_dwn,2)-pow(ABS_GOAL_SIZE_Y,2))/(2*dist_up*dist_dwn) );

		cv::Point v = cv::Point(goal.x - Ball_Est.x, goal.y - Ball_Est.y);
		double module = sqrt(pow(v.x,2) + pow(v.y,2));

		double phi = atan((m2-m1)/(1+m2*m1));
		// cout << "phi -> " << phi*180/PI << " 180 - phi -> " << 180 - phi*180/PI << endl;

		targets_temp.x = double(Ball_Est.x - double(v.x/module) * max_approach);
		targets_temp.y = double(Ball_Est.y - double(v.y/module) * max_approach);

		//crop
		if(targets_temp.y < 0) targets_temp.y = 0;
		if(targets_temp.y > ABS_FIELD_HEIGHT) targets_temp.y = ABS_FIELD_HEIGHT;
		if(targets_temp.x < 0) targets_temp.x = 0;
		if(targets_temp.x > ABS_GOAL_TO_GOAL_WIDTH) targets_temp.x = ABS_GOAL_TO_GOAL_WIDTH;

		robots[i].target = targets_temp;
		robots[i].transAngle = potField(i, targets_temp, BALL_ONLY_OBS);
		// cout << "go_to_the_ball " << endl;

		if(robots[i].position.x > Ball.x + ABS_ROBOT_SIZE){
			action1 = false;
		}


		if(is_near(i, targets_temp) || abs(phi) < beta/2 || action1) {

			v = cv::Point(robots[i].position.x - Ball.x, robots[i].position.y - Ball.y);
			module = sqrt(pow(v.x,2) + pow(v.y,2));

			targets_temp.x = double(Ball.x - double(v.x/module) * max_approach);
			targets_temp.y = double(Ball.y - double(v.y/module) * max_approach);

			robots[i].transAngle = atan2(double(robots[i].position.y - Ball.y), - double(robots[i].position.x - Ball.x));
			action1 = true;
			// cout << "action1" << endl;
		}
	}

	void gotta_catch_the_ball(int i) { //(UMA BOSTA)usando área de possibilidade para gol

		double dist_up = distance(Ball_Est, cv::Point(COORD_GOAL_ATK_FRONT_X, COORD_GOAL_UP_Y));
		double dist_dwn = distance(Ball_Est, cv::Point(COORD_GOAL_ATK_FRONT_X, COORD_GOAL_DWN_Y));

		double beta = acos( (pow(dist_up,2)+pow(dist_dwn,2)-pow(ABS_GOAL_SIZE_Y,2))/(2*dist_up*dist_dwn) );

		double m1 = double(COORD_GOAL_MID_Y - Ball_Est.y)/double(COORD_GOAL_ATK_FRONT_X - Ball_Est.x);
		double m2 = double(robots[i].position.y - Ball_Est.y)/double(robots[i].position.x - Ball_Est.x);

		double phi = atan((m2-m1)/(1+m2*m1));
		cout << "phi ="<<phi*180/PI << " beta ="<< beta*180/PI<<endl;

		cv::Point v = cv::Point(COORD_GOAL_ATK_FRONT_X - Ball_Est.x, COORD_GOAL_MID_Y - Ball_Est.y);
		double module = sqrt(pow(v.x,2) + pow(v.y,2));



		robots[i].target.x = double(Ball_Est.x - double(v.x/module) * max_approach);
		robots[i].target.y = double(Ball_Est.y - double(v.y/module) * max_approach);

		//crop
		if(robots[i].target.y < 0) robots[i].target.y = 0;
		if(robots[i].target.y > ABS_FIELD_HEIGHT) robots[i].target.y = ABS_FIELD_HEIGHT;
		if(robots[i].target.x < COORD_BOX_DEF_X) robots[i].target.x = COORD_BOX_DEF_X;
		if(robots[i].target.x > COORD_GOAL_ATK_FRONT_X) robots[i].target.x = COORD_GOAL_ATK_FRONT_X;



	}

	// void fixed_lookup(int i) {
	// 	if(distance(robots[i].position, robots[i].target) <= fixed_pos_distance/2) {
	// 		robots[i].cmdType = ORIENTATION;
	// 		robots[i].targetOrientation = PI/2;
	// 		// cout << "lookup" << '\n';
	// 	}
	// }

	void test_run(int i) {
		// robots[i].target = robots[i].position;
		// fixed_lookup(i);
	}

	void atk_routine(int i) {

		switch (robots[i].status) {

			case NORMAL_STATE:
				if(Ball.x > corner_atk_limit && (Ball.y > COORD_GOAL_DWN_Y || Ball.y < COORD_GOAL_UP_Y)) {
					robots[i].status = CORNER_STATE; // bola no canto
					// cout << "CORNER_STATE " << endl;
				} else if(Ball.y > ABS_FIELD_HEIGHT - ABS_ROBOT_SIZE*1.5 || Ball.y < ABS_ROBOT_SIZE*1.5) {
					robots[i].status = SIDEWAYS;
				}

				pot_field_around(i);
			break;

			case SIDEWAYS:
				if(Ball.y > ABS_FIELD_HEIGHT - ABS_ROBOT_SIZE*1.5) {
					if(distance(robots[i].position, Ball) > ABS_ROBOT_SIZE) {
						robots[i].target = Ball;
					} else if(Ball.x >= robots[i].position.x + ABS_ROBOT_SIZE/2){
						robots[i].target = Ball;
					} else{
						spin_left(i);
					}
					if(Ball.x < robots[i].position.x) {
						robots[i].target.x = Ball.x - max_approach;
						robots[i].target.y = Ball.y - max_approach/2;
					}
				} else if(Ball.y < ABS_ROBOT_SIZE*1.5) {
					if(distance(robots[i].position, Ball) > ABS_ROBOT_SIZE) {
						robots[i].target = Ball;
					} else if(Ball.x >= robots[i].position.x + ABS_ROBOT_SIZE/2){
						robots[i].target = Ball;
					} else{
						spin_right(i);
					}
					if(Ball.x < robots[i].position.x) {
						robots[i].target.x = Ball.x - max_approach;
						robots[i].target.y = Ball.y + max_approach/2;
					}
				} else {
					robots[i].status = NORMAL_STATE; // voltar ao estado normal
				}
			break;

			case CORNER_STATE:
				if(Ball.x > corner_atk_limit) {
					if(Ball.y < COORD_GOAL_MID_Y) { // acima ou abaixo do gol, para saber para qual lado girar
						if(Ball.y < COORD_GOAL_UP_Y) { // acima ou abaixo da trave, escolher o giro para levar a bola para o gol ou para faze-la entrar
							if((Ball.x > robots[i].position.x || Ball.y < robots[i].position.y) && distance(Ball, robots[i].position) < ABS_ROBOT_SIZE) spin_right(i); // giro que leva a bola ao gol
							else robots[i].target = Ball; //!!! testar pode dar ruim com a troca
						} else {
							if(Ball.y > robots[i].position.y && distance(Ball, robots[i].position) < ABS_ROBOT_SIZE) spin_left(i); // giro para faze-la entrar
							else robots[i].target = Ball;
						}
					} else {
						if(Ball.y > COORD_GOAL_DWN_Y) {
							if((Ball.x > robots[i].position.x || Ball.y > robots[i].position.y) && distance(Ball, robots[i].position) < ABS_ROBOT_SIZE) spin_left(i);
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
				robots[i].target.x = COORD_GOAL_ATK_FRONT_X;
				if(robots[i].position.y > COORD_GOAL_MID_Y + (COORD_GOAL_UP_Y - COORD_GOAL_MID_Y)/2) {
					robots[i].target.y = COORD_GOAL_UP_Y;
				}
				else if(robots[i].position.y > COORD_GOAL_MID_Y - (COORD_GOAL_MID_Y - COORD_GOAL_DWN_Y)/2) {
					robots[i].target.y = COORD_GOAL_DWN_Y;
				}
				else {
					robots[i].target.y = COORD_GOAL_MID_Y;
				}
				if(!atk_ball_possession) robots[i].status = NORMAL_STATE;
			break;
		}

	}

	void def_routine(int i) {

		switch (robots[i].status) {

			case NORMAL_STATE:
				if(Ball.x > corner_atk_limit) {
					robots[i].target = cv::Point(COORD_MID_FIELD_X, COORD_GOAL_MID_Y);
					robots[i].transAngle = potField(i, cv::Point(COORD_MID_FIELD_X, COORD_GOAL_MID_Y), BALL_IS_OBS);
				} else {
					robots[i].target = cv::Point(COORD_MID_FIELD_X/2, COORD_GOAL_MID_Y);
					robots[i].transAngle = potField(i, cv::Point(COORD_MID_FIELD_X/2, COORD_GOAL_MID_Y), BALL_IS_OBS);
				}
				def_wait(i);
			break;

			case STEP_BACK:

			break;

			case TRANSITION_STATE:

			break;
		}
	}

	void gk_routine(int i) {

		switch (robots[i].status) {

			case NORMAL_STATE:
			// robots[i].fixedPos = true;
			robots[i].target.x = goalie_line;
			if (Ball.x > ABS_GOAL_TO_GOAL_WIDTH/2) robots[i].target.y = Ball_Est.y;
			else robots[i].target.y = Ball.y;

			if(distance(Ball, Ball_Est) > ABS_ROBOT_SIZE*2 && Ball.x > Ball_Est.x && (Ball.x > COORD_BOX_DEF_X) ) {
				double m = double(Ball.y - Ball_Est.y)/double(Ball.x - Ball_Est.x);
				robots[i].target.y = Ball.y - m * (Ball.x) - COORD_GOAL_DEF_FRONT_X;
			}

			if(robots[i].target.y > COORD_GOAL_DWN_Y) robots[i].target.y = COORD_GOAL_DWN_Y;
			if(robots[i].target.y < COORD_GOAL_UP_Y) robots[i].target.y = COORD_GOAL_UP_Y;

			// if(Ball.y > COORD_GOAL_DWN_Y) robots[i].target.y = COORD_GOAL_DWN_Y;
			// if(Ball.y < COORD_GOAL_UP_Y) robots[i].target.y = COORD_GOAL_UP_Y;

			if(Ball.x < COORD_BOX_DEF_X && Ball.y > COORD_GOAL_DWN_Y) {
				robots[i].target.y = COORD_GOAL_DWN_Y + goalie_offset;
			} else if (Ball.x < COORD_BOX_DEF_X && Ball.y < COORD_GOAL_UP_Y){
				robots[i].target.y = COORD_GOAL_UP_Y - goalie_offset;
			}

			robots[i].vmax = robots[i].vdefault * (distance(robots[i].position, robots[i].target))/ABS_GOAL_SIZE_Y;
			// cout << "vmax " << robots[i].vmax << " distancia "<< distance(robots[i].position, robots[i].target) << " distancia max " << ABS_GOAL_TO_GOAL_WIDTH/4<<endl;
			if(robots[i].vmax > robots[i].vdefault) robots[i].vmax = robots[i].vdefault;
			if(robots[i].vmax < 0.3) robots[i].vmax = 0.3;
			// fixed_lookup(i);

			if(distance(robots[i].position, robots[i].target) < fixed_pos_distance/2) {
				robots[i].cmdType = ORIENTATION;
				robots[i].targetOrientation = PI/2;
				if( tan(robots[i].orientation) > tan(robots[i].targetOrientation - 5*PI/180) && tan(robots[i].orientation) < tan(robots[i].targetOrientation + 5*PI/180) )
				{
					robots[i].cmdType = POSITION;
					robots[i].vmax = 0;
				}
			}

			break;

			case ADVANCING_STATE:
			robots[i].target = Ball;
			if(robots[i].position.x > COORD_BOX_DEF_X || robots[i].position.y < COORD_BOX_UP_Y || robots[i].position.y > COORD_BOX_DWN_Y) {
				full_transition = true;
			}
			break;
		}
	}

	void opp_gk_routine(int i) {

		switch (robots[i].status) {

			case NORMAL_STATE:
			robots[i].fixedPos = true;
			robots[i].target.x = COORD_GOAL_ATK_FRONT_X - ABS_ROBOT_SIZE/2;
			robots[i].target.y = Ball_Est.y;

			if(Ball_Est.y > COORD_GOAL_DWN_Y) robots[i].target.y = COORD_GOAL_DWN_Y;
			if(Ball_Est.y < COORD_GOAL_UP_Y) robots[i].target.y = COORD_GOAL_UP_Y;
			// if(danger_zone_2) robots[i].status = ADVANCING_STATE;
			break;

			// case ADVANCING_STATE:
			// robots[i].target = Ball;
			// if(robots[i].position.x > COORD_BOX_DEF_X || robots[i].position.y < COORD_BOX_UP_Y || robots[i].position.y > COORD_BOX_DWN_Y) {
			// 	full_transition = true;
			// }
			// break;
		}
	}

	//Estimativa da Bola
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

		Ball_Est.x =  LS_ball_x.estimate(10);
		Ball_Est.y =  LS_ball_y.estimate(10);

	}

};
#endif /* STRATEGY_HPP_ */
