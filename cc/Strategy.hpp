#ifndef STRATEGY_HPP_
#define STRATEGY_HPP_
#define PI 3.14159265453
#include "opencv2/opencv.hpp"
#include "LS.cpp"
#include "Robot.hpp"
#include "SerialW.hpp"
#include <math.h>
#define PREDICAO_NUM_SAMPLES 15
#define COMPRIMENTO_CAMPO 1.50f //m
#define LARGURA_CAMPO 1.3f //m
#define TAMANHO_GOL 0.40f //m
#define TAMANHO_AREA 0.70f //m

#define LINHA_ZAGA			0.30*COMPRIMENTO_CAMPO//m
#define BANHEIRA			0.50*COMPRIMENTO_CAMPO+0.16//m
#define DIVISAO_AREAS		0.50*COMPRIMENTO_CAMPO//m
#define OFFSET_BANHEIRA		0.20//m
#define MEIO_GOL_X			COMPRIMENTO_CAMPO+0.2f//m
#define MEIO_GOL_Y			LARGURA_CAMPO/2//m
#define MAX_GOL_Y			MEIO_GOL_Y + TAMANHO_GOL/2//m
#define MIN_GOL_Y			MEIO_GOL_Y - TAMANHO_GOL/2//m

// Parametros para atacante sem bola
#define OFFSET_RATIO		0.12
#define CONE_RATIO			0.8
#define ENTRADA_GOL_INVERSO	1

//Parametros para atacante com bola
#define AVOID_GOLEIRO_APOLO		0

// Parametros do Defensor com bola no ataque
#define DEF_RATIO			1.0f

// Parametros do Defensor na defesa
#define ZAGA_CENTRALIZADA			1
#define DESLOCAMENTO_ZAGA_ATAQUE	1.3




class Strategy
{
public:
		Robot Goalkeeper;
		Robot Attack;
		Robot Defense;
		cv::Point Ball;
		cv::Point FutureBall;
		double width;
		double height;
		double distBall;
		cv::Point robot;
		LS LS_ball_x;
		LS LS_ball_y;
		cv::Point Ball_Est;
		cv::Point target;	
		
Strategy()									// TUDO EM METROS
	{
		LS_ball_x.init(PREDICAO_NUM_SAMPLES,1);
		LS_ball_y.init(PREDICAO_NUM_SAMPLES,1);
	}
void Pixel_to_Meters(cv::Point position) { // Método para passar a posição de pixels para metros
	robot.x = position.x*1.70/width;			// posição x de pixels para metros
	robot.y = position.y*1.30/height;		// posição y de pixels para metros
	}
		
cv::Point get_Attack_Classic(cv::Point Ball, cv::Point position) { // Estratégia de ataque clássico (Antigo Ojuara)
	
	Pixel_to_Meters(position);
	distBall = sqrt(pow(robot.x - Ball.x, 2) + pow(robot.y - Ball.y, 2));
	if (Ball.x > DIVISAO_AREAS) { //Bola no ataque?
		if (distBall < 0.08 && robot.x < Ball.x) { //Posse de bola? && Orientação robô + bola voltada para o lado dversário?
			if (robot.x > COMPRIMENTO_CAMPO - 0.2 && ( robot.y < MIN_GOL_Y || robot.y > MAX_GOL_Y)) {
				// robo esta nos retangulos laterais(com aprox 20cm a partir do fundo do gol) do gol adversario?
				target.x = Ball_Est.x; //Vai na estimativa da bola
				target.y = Ball_Est.y;	
			}
			else {
				target.x = MEIO_GOL_X; //Vai na projeçao da bola na linha vertical do meio do gol
				target.y = Ball.y;
				if ( target.y > MAX_GOL_Y) target.y = MAX_GOL_Y;
				if ( target.y < MIN_GOL_Y) target.y = MIN_GOL_Y;	
			}	
		}
		else { //Sem a bola
			if(robot.x > Ball_Est.x+0.04f){ // Atacante a frente da bola?
				target.x = Ball_Est.x-0.16f;	//Coisas fuzzy que ninguem entende a partir daqui----------------------------|
				// Ataque adiantado, da a volta na bola
				float diff = (Ball_Est.y - robot.y)/LARGURA_CAMPO;
				float offset = pow(cos(PI*diff/2),6)*OFFSET_RATIO*cos(2*PI*diff);
				float mixrb = abs(diff)*Ball_Est.y + (1-abs(diff))*robot.y;

				if(diff>0)
					target.y = mixrb - offset;
				else
					target.y = mixrb + offset;

				// Não permite que o target seja colocado além dos limites do campo
				if(target.y<0)			
					target.y = mixrb + offset;	
				else if(target.y>LARGURA_CAMPO)
					target.y = mixrb - offset;
			}
			else{
				// Ataque recuado
				if(robot.x < COMPRIMENTO_CAMPO-0.2f){
					float phi = atan((MEIO_GOL_Y - Ball.y)/(MEIO_GOL_X - Ball.x));					// Angulo entre o gol e a bola
					float theta = atan((MEIO_GOL_Y - robot.y)/(MEIO_GOL_X - robot.x));	// Angulo entre o gol e o atacante
					target.x = Ball_Est.x - CONE_RATIO*cos(phi)*2*(abs(phi-theta))/PI;
					target.y = Ball_Est.y - CONE_RATIO*sin(phi)*2*(abs(phi-theta))/PI;
					if(target.y<0 || target.y > LARGURA_CAMPO){
						theta = atan((robot.y - Ball.y)/(robot.x - Ball.x));
						target.x = Ball_Est.x;
						target.y = Ball_Est.y;
					}
				}else{
					target.x = Ball_Est.x;
					target.y = Ball_Est.y;				// Fim das coisas fuzzy queninguem entende----------------------------|
				}
			}
		}
	}
	else {
		//Bola na defesa
		target.x = BANHEIRA;
		if(Ball.y < LARGURA_CAMPO/2)
			target.y = Ball.y + OFFSET_BANHEIRA;
		else
			target.y = Ball.y - OFFSET_BANHEIRA;
		
	}
	
	return target;
	
}

cv::Point get_Defense_Classic(cv::Point Ball, cv::Point position) { // Estratégia de defesa clássica (Antigo Lenhador)
	
	Pixel_to_Meters(position);
	distBall = sqrt(pow(robot.x - Ball.x, 2) + pow(robot.y - Ball.y, 2));
	if (Ball.x < DIVISAO_AREAS) { //Bola na defesa?
		if (distBall < 0.08 && robot.x < Ball.x) { //Posse de bola? && Orientação robô + bola voltada para o lado dversário?
			if (robot.x > COMPRIMENTO_CAMPO - 0.2 && ( robot.y < MIN_GOL_Y || robot.y > MAX_GOL_Y)) { //-------------------?
				// robo esta nos retangulos laterais(com aprox 20cm a partir do fundo do gol) do gol adversario?
				target.x = Ball_Est.x; //Vai na estimativa da bola
				target.y = Ball_Est.y;	
			}
			else {
				target.x = MEIO_GOL_X; //Vai na projeçao da bola na linha vertical do meio do gol
				target.y = Ball.y;
				if ( target.y > MAX_GOL_Y) target.y = MAX_GOL_Y;
				if ( target.y < MIN_GOL_Y) target.y = MIN_GOL_Y;	
			} //-----------------------------------------------------------------------------------------------------------?
		}
		else { // Sem a bola
			if (robot.x > Ball_Est.x + 0.04) {
				target.x = Ball_Est.x-0.16f;	//Coisas fuzzy que ninguem entende a partir daqui----------------------------|
				// Ataque adiantado, da a volta na bola
				float diff = (Ball_Est.y - robot.y)/LARGURA_CAMPO;
				float offset = pow(cos(PI*diff/2),6)*OFFSET_RATIO*cos(2*PI*diff);
				float mixrb = abs(diff)*Ball_Est.y + (1-abs(diff))*robot.y;

				if(diff>0)
					target.y = mixrb - offset;
				else
					target.y = mixrb + offset;

				// Não permite que o target seja colocado além dos limites do campo
				if(target.y<0)			
					target.y = mixrb + offset;	
				else if(target.y>LARGURA_CAMPO)
					target.y = mixrb - offset;
			}
			else{
				// Ataque recuado
				if(robot.x < COMPRIMENTO_CAMPO-0.2f){
					float phi = atan((MEIO_GOL_Y - Ball.y)/(MEIO_GOL_X - Ball.x));		// Angulo entre o gol e a bola
					float theta = atan((MEIO_GOL_Y - robot.y)/(MEIO_GOL_X - robot.x));	// Angulo entre o gol e o atacante
					target.x = Ball_Est.x - CONE_RATIO*cos(phi)*2*(abs(phi-theta))/PI;
					target.y = Ball_Est.y - CONE_RATIO*sin(phi)*2*(abs(phi-theta))/PI;
					if(target.y<0 || target.y > LARGURA_CAMPO){
						theta = atan((robot.y - Ball.y)/(robot.x - Ball.x));
						target.x = Ball_Est.x;
						target.y = Ball_Est.y;
					}
				}else{
					target.x = Ball_Est.x;
					target.y = Ball_Est.y;				// Fim das coisas fuzzy que ninguem entende----------------------------|
				}
			}
		}
	}
	else {
		// Bola no ataque
		target.x = LINHA_ZAGA;
		if (ZAGA_CENTRALIZADA) {
			float alpha = (Ball_Est.x - DIVISAO_AREAS)/(DESLOCAMENTO_ZAGA_ATAQUE*COMPRIMENTO_CAMPO - DIVISAO_AREAS);
			target.y = alpha*LARGURA_CAMPO/2 + (1+alpha)*Ball_Est.y;
		}
		else {
			target.y = Ball_Est.y;
		}
	}
	return target;
}


		
		
	
void set_Attack(Robot r){
	Attack=r;
	}
void set_Defense(Robot r){
	Defense=r;
	}
void set_Goalkeeper(Robot r){
	Goalkeeper=r;
	}
	
cv::Point get_Ball_Est(){
	return Ball_Est;
	}
	
void set_Ball(cv::Point b){
	Ball = b;
	LS_ball_x.addValue(Ball.x);
	LS_ball_y.addValue(Ball.y);
	
	Ball_Est.x =  LS_ball_x.estimate(7);
	Ball_Est.y =  LS_ball_y.estimate(7);

	}
	

cv::Point get_gk_target(){

		Goalkeeper.target.x = 70;
		Goalkeeper.target.y = Ball.y;
	
		if (Goalkeeper.target.y > 354)
		Goalkeeper.target.y = 354;
	else if (Goalkeeper.target.y < 166)
		Goalkeeper.target.y = 166;	
		
		return Goalkeeper.target;

	}
	
	}; 
#endif /* STRATEGY_HPP_ */
