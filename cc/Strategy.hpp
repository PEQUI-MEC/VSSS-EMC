#ifndef STRATEGY_HPP_
#define STRATEGY_HPP_
#define PI 3.14159265453
#include "opencv2/opencv.hpp"
#include "LS.cpp"
#include "Robot.hpp"
#include "SerialW.hpp"
#include <math.h>
#define PREDICAO_NUM_SAMPLES 15
// Parametros para atacante sem bola

#define ENTRADA_GOL_INVERSO	1

//Parametros para atacante com bola
#define AVOID_GOLEIRO_APOLO		0

// Parametros do Defensor com bola no ataque
#define DEF_RATIO			1.0f

// Parametros do Defensor na defesa
#define ZAGA_CENTRALIZADA			1





class Strategy
{
public:
		Robot Goalkeeper;
		Robot Attack;
		Robot Defense;
		cv::Point Ball;
		cv::Point FutureBall;
		int distBall;
		LS LS_ball_x;
		LS LS_ball_y;
		cv::Point Ball_Est;
		cv::Point target = cv::Point(-1,-1);	
		int height, width;

		int COMPRIMENTO_CAMPO	;
		int LARGURA_CAMPO 	; 	
		int TAMANHO_GOL  	;		
		int TAMANHO_AREA  	;	
		
		int BANHEIRA		;
		int DIVISAO_AREAS	;
		int OFFSET_BANHEIRA	;
		int MEIO_GOL_X		;
		int MEIO_GOL_Y		;
		int MAX_GOL_Y		;
		int MIN_GOL_Y		;
		int LINHA_ZAGA		;
		// Parametros para atacante sem bola
		int OFFSET_RATIO	;
		int CONE_RATIO		;
	
		// Parametros do Defensor na defesa
		int DESLOCAMENTO_ZAGA_ATAQUE	;
		
Strategy()									// TUDO EM METROS
	{
		LS_ball_x.init(PREDICAO_NUM_SAMPLES,1);
		LS_ball_y.init(PREDICAO_NUM_SAMPLES,1);
	}
	
void set_constants(int w, int h){
		 width = w;
		 height = h;
		 COMPRIMENTO_CAMPO = 	round(1.50*float(width)/1.70); 
		 LARGURA_CAMPO 	= 		height; 
		 TAMANHO_GOL 	= 		round(0.35*float(height)/1.30); 
		 TAMANHO_AREA 	= 		247; 
		
		 BANHEIRA		=	round((0.50*COMPRIMENTO_CAMPO))+round(0.16*float(width)/1.70);
		 DIVISAO_AREAS	=	round((0.50*COMPRIMENTO_CAMPO));
		 OFFSET_BANHEIRA=	round(0.20*float(width)/1.70);
		 MEIO_GOL_X		=	round(COMPRIMENTO_CAMPO+round(0.1*float(width)/1.70));
		 MEIO_GOL_Y		=	round(LARGURA_CAMPO/2);
		 MAX_GOL_Y		=	round(MEIO_GOL_Y + TAMANHO_GOL/2);
		 MIN_GOL_Y		=	round(MEIO_GOL_Y - TAMANHO_GOL/2);
		 LINHA_ZAGA		=	round(0.30*COMPRIMENTO_CAMPO);
		 
		// Parametros para atacante sem bola
		 OFFSET_RATIO	=	round(0.12*float(width)/1.70);
		 CONE_RATIO		=	round(0.8*float(width)/1.70);
		 
		 
		// Parametros do Defensor na defesa
		DESLOCAMENTO_ZAGA_ATAQUE	=	round(1.3*float(width)/1.70);
	
	}

		
cv::Point get_Attack_Classic(cv::Point robot) { // Estratégia de ataque clássico (Antigo Ojuara)
	
	distBall = sqrt(pow(robot.x - Ball.x, 2) + pow(robot.y - Ball.y, 2));

	if (Ball.x > DIVISAO_AREAS) { //Bola no ataque?
		Attack.fixedPos=false;
		//cout<<"Bola no Ataque "<<"|";
		if (distBall < round(0.08*float(width)/1.70) && robot.x < Ball.x) { //Posse de bola? && Orientação robô + bola voltada para o lado dversário?
			//	cout<<"Posse de Bola - ";

			if (robot.x > COMPRIMENTO_CAMPO - round(0.2*float(width)/1.70) && ( robot.y < MIN_GOL_Y || robot.y > MAX_GOL_Y)) {
			//	cout<<"Retangulo Lateral - ";
				// robo esta nos retangulos laterais(com aprox 20cm a partir do fundo do gol) do gol adversario?
				target.x = Ball_Est.x; //Vai na estimativa da bola
				target.y = Ball_Est.y;	
			}else {
			//	cout<<"Não esta no Retangulo Lateral - ";
				target.x = MEIO_GOL_X; //Vai na projeçao da bola na linha vertical do meio do gol
				target.y = Ball.y;
				
				if ( target.y > MAX_GOL_Y) target.y = MAX_GOL_Y;
				if ( target.y < MIN_GOL_Y) target.y = MIN_GOL_Y;	
			}	
		}
		else { //Sem a bola
			//	cout<<"Sem Posse de Bola - ";
			if(robot.x > Ball_Est.x+round(0.04*float(width)/1.70)){ // Atacante a frente da bola?
			//	cout<<"Atk na frente da bola - ";
				target.x = Ball_Est.x-round(0.16*float(width)/1.70);	//Coisas fuzzy que ninguem entende a partir daqui----------------------------|
				
				// Ataque adiantado, da a volta na bola
				float diff = (float(Ball_Est.y) - float(robot.y))/float(LARGURA_CAMPO);
				
				float offset = pow(cos(PI*diff/2),6)*float(OFFSET_RATIO)*cos(2*PI*diff);
			
				float mixrb = abs(diff)*float(Ball_Est.y) + (1-abs(diff))*float(robot.y);
				
				if(diff>0)
					target.y = round(mixrb - offset);
				else
					target.y = round(mixrb + offset);

				// Não permite que o target seja colocado além dos limites do campo
				if(target.y<0)			
					target.y = round(mixrb + offset);	
				else if(target.y>LARGURA_CAMPO)
					target.y = round(mixrb - offset);
			
			
			}
			else{
				cout<<"Atk atras da bola - ";
				// Ataque recuado
				if(robot.x < COMPRIMENTO_CAMPO- round(0.2*float(width)/1.70)){
					cout<<"Fora da area - ";
					float phi = atan(float(MEIO_GOL_Y - Ball.y)/float(MEIO_GOL_X - Ball.x));					// Angulo entre o gol e a bola
					float theta = atan(float(MEIO_GOL_Y - robot.y)/float(MEIO_GOL_X - robot.x));	// Angulo entre o gol e o atacante
					target.x = Ball_Est.x - round(CONE_RATIO*cos(phi)*2*(abs(phi-theta))/PI);
					target.y = Ball_Est.y - round(CONE_RATIO*sin(phi)*2*(abs(phi-theta))/PI);
					if(target.y<0 || target.y > LARGURA_CAMPO){
					cout<<"Alvo fora - "; 	
						target.x = Ball_Est.x;
						target.y = Ball_Est.y;
					}
				}else{
					cout<<"Na area - ";
					target.x = Ball_Est.x;
					target.y = Ball_Est.y;				// Fim das coisas fuzzy queninguem entende----------------------------|
				}
			}
		}
	}
	else {
		//cout<<"Bola na Defesa - ";
		Attack.fixedPos=true;
		//Bola na defesa
		target.x = BANHEIRA;
		if(Ball.y < LARGURA_CAMPO/2)
			target.y = Ball.y + OFFSET_BANHEIRA;
		else
			target.y = Ball.y - OFFSET_BANHEIRA;
//		cout<<endl;
	}
	//cout<<target.x<<" - "<<target.y<<endl; 
	
	return target;
	
}

cv::Point get_Defense_Classic(cv::Point robot) { // Estratégia de defesa clássica (Antigo Lenhador)
	distBall = round(sqrt(pow(robot.x - Ball.x, 2) + pow(robot.y - Ball.y, 2)));
	
	if (Ball.x < DIVISAO_AREAS) { //Bola na defesa?
		Defense.fixedPos=false;
		cout<<"Bola na Defesa - ";
		if (distBall < round(0.08*float(width)/1.70) && robot.x < Ball.x) { //Posse de bola? && Orientação robô + bola voltada para o lado dversário?
			cout<<"Posse de Bola "<<"|";
				target.x = MEIO_GOL_X; //Vai na projeçao da bola na linha vertical do meio do gol
				target.y = Ball.y;
				if ( target.y > MAX_GOL_Y) target.y = MAX_GOL_Y;
				if ( target.y < MIN_GOL_Y) target.y = MIN_GOL_Y;	
		}
		else { // Sem a bola
			cout<<"Sem Posse de Bola - ";
			if (robot.x > Ball.x + round(0.04*float(width)/1.70)) {
				target.x = Ball_Est.x-round(0.16*float(width)/1.70);	//Coisas fuzzy que ninguem entende a partir daqui----------------------------|
				// Ataque adiantado, da a volta na bola
				float diff = float(Ball_Est.y - robot.y)/float(LARGURA_CAMPO);
				float offset = pow(cos(PI*diff/2),6)*float(OFFSET_RATIO)*cos(2*PI*diff);
				float mixrb = abs(diff)*float(Ball_Est.y) + (1-abs(diff))*float(robot.y);
				
				if(diff>0)
					target.y = round(mixrb - offset);
				else
					target.y = round(mixrb + offset);

				// Não permite que o target seja colocado além dos limites do campo
				if(target.y<0)			
					target.y = round(mixrb + offset);	
				else if(target.y>LARGURA_CAMPO)
					target.y = round(mixrb - offset);
					
			}
			else{
				// Ataque recuado
				cout<<"Robo atras - ";
				if(robot.x < COMPRIMENTO_CAMPO-round(0.2*float(width)/1.70)){
					cout<<"Dentro linha area - ";
					// Dentro da area
					float phi = atan(MEIO_GOL_Y - Ball.y)/(MEIO_GOL_X - Ball.x);		// Angulo entre o gol e a bola
					float theta = atan(MEIO_GOL_Y - robot.y)/(MEIO_GOL_X - robot.x);	// Angulo entre o gol e o atacante
					target.x = Ball_Est.x - round(CONE_RATIO*cos(phi)*2*(abs(phi-theta))/PI);
					target.y = Ball_Est.y - round(CONE_RATIO*sin(phi)*2*(abs(phi-theta))/PI);
					if(target.y<0 || target.y > LARGURA_CAMPO){
						theta = atan((robot.y - Ball.y)/(robot.x - Ball.x));
						target.x = Ball_Est.x;
						target.y = Ball_Est.y;
					}
				}else{
					cout<<"fora linha area - ";
					target.x = Ball_Est.x;
					target.y = Ball_Est.y;				// Fim das coisas fuzzy que ninguem entende----------------------------|
				}
			}
		}
	}
	else {
		cout<<"Bola no Ataque - ";
		// Bola no ataque
		target.x = LINHA_ZAGA;
		Defense.fixedPos=true;
		if (ZAGA_CENTRALIZADA) {
			//cout<<"Entrou no if porra "<<endl;			
			
			float alpha = float(Ball_Est.y - DIVISAO_AREAS)/float(DESLOCAMENTO_ZAGA_ATAQUE*COMPRIMENTO_CAMPO - DIVISAO_AREAS);
		//	cout<<"alpha  "<<long(alpha);
			target.y = round((alpha*LARGURA_CAMPO/2) + (1+alpha)*Ball_Est.y);
		}
		else {
			target.y = Ball_Est.y;
		}
	}
	
	if(target.x < 113 && (target.y > LARGURA_CAMPO/2-TAMANHO_AREA/2 && target.y < LARGURA_CAMPO/2+TAMANHO_AREA/2)){
		target.x = LINHA_ZAGA;
		Defense.fixedPos=true;
		target.y = Ball.y;
		cout<<"Nao dexa area - ";
	}

	if (target.x < 38) {
		target.x = 47;
		cout<<"Nao dexa fora - ";
	}
	
	cout<<endl;
	return target;
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

		Goalkeeper.target.x = 55;
		Goalkeeper.target.y = Ball_Est.y;
	
		if (Goalkeeper.target.y > 354)
		Goalkeeper.target.y = 354;
	else if (Goalkeeper.target.y < 150)
		Goalkeeper.target.y = 150;	
		Goalkeeper.fixedPos=true;
		return Goalkeeper.target;

	}
	
	}; 
#endif /* STRATEGY_HPP_ */
