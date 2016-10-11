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
		cv::Point targetlock = cv::Point(-1,-1);	
		int height, width;
		int COMPRIMENTO_CAMPO	;
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
		 LIMITE_AREA_X	=		113;
		
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
		 MAX_DECISION_COUNT = 10;
		 COUNT_DECISION = MAX_DECISION_COUNT;
		 TARGET_LOCK_COUNT = 10;
		 MAX_TARGET_LOCK_COUNT = 10;
		// Parametros do Defensor na defesa
		DESLOCAMENTO_ZAGA_ATAQUE	=	round(1.3*float(width)/1.70);
	
	}

		
cv::Point get_atk_target(cv::Point robot, double orientation) { // Estratégia de ataque clássico (Antigo Ojuara)
	
	Attack.previous_status = Attack.status;
	
	distBall = sqrt(pow(robot.x - Ball.x, 2) + pow(robot.y - Ball.y, 2));
//	cout<<"Status - "<<Attack.status<<endl;
	
	Attack.status = 0;

	
//	cout<<"atan(1) "<<atan2(float(MIN_GOL_Y - robot.y),float(COMPRIMENTO_CAMPO - robot.x))<<" atan(2) "<<atan2(float(MAX_GOL_Y - robot.y) , float(COMPRIMENTO_CAMPO - robot.x))<<" orientation "<<orientation<<endl;
	if (Ball.x > DIVISAO_AREAS) { //Bola no ataque?
		Attack.fixedPos=false;
		//cout<<"Bola no Ataque "<<"|";
		if(distBall < round(0.08*float(width)/1.70) ) {
			//Posse de bola?????????? SIM
			cout<<"posse de bola"<<endl;
//			if (robot.x > Ball.x) {
//				Attack.status = 1; //Tratamento atacante re
//			}
			//Definicao de acao para acelerar em direcao ao gol
			if (robot.y > MAX_GOL_Y && orientation >= 0) {
				orientation = orientation - double(PI);
			}
			if (robot.y < MIN_GOL_Y && orientation < 0) {
				orientation = orientation + double(PI);
			}
			
			if (robot.x >= COMPRIMENTO_CAMPO - LIMITE_AREA_X  && (robot.y < MIN_GOL_Y || robot.y > MAX_GOL_Y) ) {
			// Posse de Bola? Retangulo lateral?	
				
					//cout<<"Retangulo Lateral - ";
					
					// robo esta nos retangulos laterais(com aprox 20cm a partir do fundo do gol) do gol adversario?
				
					target.x = Ball.x; //Vai na bola
					target.y = Ball.y;
					Attack.status = 3;	
			}
			else if (Attack.previous_status == 3||COUNT_DECISION<MAX_DECISION_COUNT)
			{
				target.x = Ball.x;
				target.y = Ball.y;
				Attack.status = 4;
				COUNT_DECISION--;
				if(COUNT_DECISION<=0)
				COUNT_DECISION = MAX_DECISION_COUNT;
				
			}
			else if (orientation >  atan2(float(MIN_GOL_Y - robot.y),float(COMPRIMENTO_CAMPO - robot.x)) && 
				orientation <  atan2(float(MAX_GOL_Y - robot.y) , float(COMPRIMENTO_CAMPO - robot.x)) && robot.x < Ball.x) { 
				//Posse de bola? && Orientação robô + bola voltada para o lado adversário?
			
//				cout<<"Angulos "<<endl;	
				Attack.status = 2;			

	//			cout<<"Status - "<<Attack.status<<endl;
				if ( robot.y < MEIO_GOL_Y) { 
					target.x = MEIO_GOL_X;
					target.y = MAX_GOL_Y;	
				}
				else{
					target.x = MEIO_GOL_X;
					target.y = MIN_GOL_Y;
				}
				
				if(TARGET_LOCK_COUNT==MAX_TARGET_LOCK_COUNT){
					targetlock.x = target.x;
					targetlock.y = target.y;
					TARGET_LOCK_COUNT--;
					}
				if (TARGET_LOCK_COUNT < MAX_TARGET_LOCK_COUNT) {
					target.x = targetlock.x;
					target.y = targetlock.y;
					TARGET_LOCK_COUNT--;
					if (TARGET_LOCK_COUNT <= 0) {
						TARGET_LOCK_COUNT = MAX_TARGET_LOCK_COUNT;
					}
				}
							
			
			}
			else if ( robot.y < MAX_GOL_Y && robot.y > MIN_GOL_Y && robot.x < (COMPRIMENTO_CAMPO - 0.1*float(width)/1.70)) {
				
				Attack.status = 2;
				
				if ( robot.y < MEIO_GOL_Y) { 
					target.x = MEIO_GOL_X;
					target.y = MAX_GOL_Y;
//					cout<<"Centro 1 "<<endl;	
				}
				else if (robot.y >= MEIO_GOL_Y) {
					target.x = MEIO_GOL_X;
					target.y = MIN_GOL_Y;
//					cout<<"Centro 2"<<endl;
				}
				if(TARGET_LOCK_COUNT==MAX_TARGET_LOCK_COUNT){
					targetlock.x = target.x;
					targetlock.y = target.y;
					TARGET_LOCK_COUNT--;
					}
				if (TARGET_LOCK_COUNT < MAX_TARGET_LOCK_COUNT) {
					target.x = targetlock.x;
					target.y = targetlock.y;
					TARGET_LOCK_COUNT--;
					if (TARGET_LOCK_COUNT <= 0) {
						TARGET_LOCK_COUNT = MAX_TARGET_LOCK_COUNT;
					}
				}
			
			
				
			}
			else{
				target.y = Ball.y;
				target.x = Ball.x;
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
//				cout<<"Atk atras da bola - ";
				// Ataque recuado
				if(robot.x < COMPRIMENTO_CAMPO- round(0.2*float(width)/1.70)){
//					cout<<"Fora da area - ";
					
					Attack.status = 1;
//					cout<<"Status - "<<Attack.status<<endl;
					
					float phi = atan(float(MEIO_GOL_Y - Ball.y)/float(MEIO_GOL_X - Ball.x));		// Angulo entre o gol e a bola
					float theta = atan(float(MEIO_GOL_Y - robot.y)/float(MEIO_GOL_X - robot.x));	// Angulo entre o gol e o atacante
					target.x = Ball_Est.x - round(CONE_RATIO*cos(phi)*2*(abs(phi-theta))/PI);
					target.y = Ball_Est.y - round(CONE_RATIO*sin(phi)*2*(abs(phi-theta))/PI);
					if(target.y<0 || target.y > LARGURA_CAMPO){
//					cout<<"Alvo fora - "; 	
						target.x = Ball_Est.x;
						target.y = Ball_Est.y;
					}
				}else{
//					cout<<"Na area - ";
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
	//cout<<" - Attack Status - "<<Attack.status<<" Previous - "<<Attack.previous_status<<endl;
	return target;
	
}

cv::Point get_def_target(cv::Point robot) { // Estratégia de defesa clássica (Antigo Lenhador)
	distBall = round(sqrt(pow(robot.x - Ball.x, 2) + pow(robot.y - Ball.y, 2)));

	Defense.status = 0;
	
	if (Ball.x < DIVISAO_AREAS) { //Bola na defesa?
		Defense.fixedPos=false;
		//cout<<"Bola na Defesa - ";

		if (distBall < round(0.08*float(width)/1.70) && robot.x < Ball.x - 15) { //Posse de bola? && Orientação robô + bola voltada para o lado dversário?
			//cout<<"Posse de Bola "<<"|";

				Defense.status = 2;
				
				target.x = MEIO_GOL_X; //Vai na projeçao da bola na linha vertical do meio do gol
				target.y = Ball.y;
				if ( target.y > MAX_GOL_Y) target.y = MIN_GOL_Y;
				if ( target.y < MIN_GOL_Y) target.y = MAX_GOL_Y;	
		}
		else { // Sem a bola
			//cout<<"Sem Posse de Bola - ";
			if (Ball.x < LIMITE_AREA_X&&(Ball.y < LARGURA_CAMPO/2-TAMANHO_AREA/2 || Ball.y > LARGURA_CAMPO/2+TAMANHO_AREA/2)) {
		//Bola na linha de fundo
		//	cout<<"Bola na linha de fundo - ";
		if(robot.x>Ball.x+17){
		//Bola atras
		//cout<<"Bola atras - ";
		if(Ball.y<LARGURA_CAMPO/2-TAMANHO_AREA/2 - 17) {
			// lINHA DE FUNDO SUPERIOR
		
		if(robot.y>LARGURA_CAMPO/2-TAMANHO_AREA/2){
		target.x = LIMITE_AREA_X;
		}else{
		target.x = Ball_Est.x;
		}
			
		target.y =  LARGURA_CAMPO/2-TAMANHO_AREA/2 - 17;
		//cout<<"Fundo superior - ";
		}else{   
			 // lINHA DE FUNDO INFERIOR
		if(robot.y<LARGURA_CAMPO/2+TAMANHO_AREA/2){
		target.x = LIMITE_AREA_X;
		}else{
		target.x = Ball_Est.x;
		}	
		                   
		target.y =  LARGURA_CAMPO/2+TAMANHO_AREA/2 + 17;
		//cout<<"Fundo inferior - ";
		}	
		}else{
			//cout<<"Bola frente - ";
			//Bola frente/lado
			target.x = Ball_Est.x;
			target.y = Ball_Est.y;
			
			}
			
	}else if (robot.x > Ball.x + round(0.04*float(width)/1.70)) {
				target.x = Ball_Est.x-round(0.16*float(width)/1.70);	//Coisas fuzzy que ninguem entende a partir daqui----------------------------|
				//cout<<" Defesa adiantado, da a volta na bola";
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
				//cout<<"Robo atras - ";
				if(robot.x < COMPRIMENTO_CAMPO-round(0.2*float(width)/1.70)){
					//cout<<"Dentro linha area - ";
					// Dentro da area
					float phi = atan(float(MEIO_GOL_Y - Ball.y))/float(MEIO_GOL_X - Ball.x);		// Angulo entre o gol e a bola
					float theta = atan(float(MEIO_GOL_Y - robot.y))/float(MEIO_GOL_X - robot.x);	// Angulo entre o gol e o atacante
					target.x = Ball_Est.x - round(CONE_RATIO*cos(phi)*2*(abs(phi-theta))/PI);
					target.y = Ball_Est.y - round(CONE_RATIO*sin(phi)*2*(abs(phi-theta))/PI);
					if(target.y<0 || target.y > LARGURA_CAMPO){
						target.x = Ball_Est.x;
						target.y = Ball_Est.y;
					}
				}else{
				//cout<<"fora linha area - ";
					target.x = Ball_Est.x;
					target.y = Ball_Est.y;				// Fim das coisas fuzzy que ninguem entende----------------------------|
				}
			}
		}
		
	
	}
	else {
//		cout<<"Bola no Ataque - ";
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
	
	if(target.x < LIMITE_AREA_X && (target.y > LARGURA_CAMPO/2-TAMANHO_AREA/2 && target.y < LARGURA_CAMPO/2+TAMANHO_AREA/2)){
		target.x = LINHA_ZAGA;
		Defense.fixedPos=true;
		target.y = Ball.y;
	//	cout<<"Nao deixa area - ";
//		Não permite que o alvo esteja dentro da área
	}

	

	
	//cout<<endl;
	return target;
}


		
		
	
cv::Point get_Ball_Est(){
	return Ball_Est;
	}
	
void set_Ball(cv::Point b){
	Ball = b;
	LS_ball_x.addValue(Ball.x);
	LS_ball_y.addValue(Ball.y);
	
	Ball_Est.x =  LS_ball_x.estimate(5);
	Ball_Est.y =  LS_ball_y.estimate(5);

	}
	

cv::Point get_gk_target(vector< cv::Point > Adv_Main){
		Goalkeeper.fixedPos=true;
		Goalkeeper.target.x = 640-60;
		if (Ball.x < DIVISAO_AREAS)
		{ // Bola na defesa
			
			Goalkeeper.target.y = Ball_Est.y;
			/*if(Ball.x<LINHA_ZAGA)
			GOAL_DANGER_ZONE = true;
			else
			GOAL_DANGER_ZONE = false;*/
		
			if (Goalkeeper.target.y > MAX_GOL_Y)
				Goalkeeper.target.y = MAX_GOL_Y;
			else if (Goalkeeper.target.y < MIN_GOL_Y)
				Goalkeeper.target.y = MIN_GOL_Y;

			

		
			if( Ball.x<LIMITE_AREA_X && Ball.y>LARGURA_CAMPO/2-TAMANHO_AREA/2 && Ball.y<LARGURA_CAMPO/2+TAMANHO_AREA/2 )
			{
				//cout<<"BOLA NA AREA"<<endl;
				ADV_NA_AREA=false;
				for(int i=0;i<Adv_Main.size();i++){
					if((Adv_Main[i].x<LIMITE_AREA_X&&Adv_Main[i].y>LARGURA_CAMPO/2-TAMANHO_AREA/2&&Adv_Main[i].y<LARGURA_CAMPO/2+TAMANHO_AREA/2))
					{
					//cout<<"Adv NA AREA"<<endl;
					ADV_NA_AREA=true;
					}
				}
				
				if(!ADV_NA_AREA)
				{
					Goalkeeper.target=Ball;
				}
			}
			else if (Ball.x<LIMITE_AREA_X && (Ball.y<LARGURA_CAMPO/2-TAMANHO_AREA/2 || Ball.y>LARGURA_CAMPO/2+TAMANHO_AREA/2))
			{
				
				//cout<<"bola nas laterais da area"<<endl;
				
				if (Ball.y > LARGURA_CAMPO/2)
					Goalkeeper.target.y = 354;
				else if (Ball.y < LARGURA_CAMPO/2)
					Goalkeeper.target.y = 135;
			}
		}
		else
		{ // Bola no ataque
			Goalkeeper.target.y = Ball_Est.y;
			
			if (Goalkeeper.target.y > MAX_GOL_Y)
				Goalkeeper.target.y = MAX_GOL_Y;
			else if (Goalkeeper.target.y < MIN_GOL_Y)
				Goalkeeper.target.y = MIN_GOL_Y;
			
		}
		//cout<<"GK TARGET "<<Goalkeeper.target.x<<" - "<<Goalkeeper.target.y<<endl;
		return Goalkeeper.target;

	}
	
	}; 
#endif /* STRATEGY_HPP_ */
