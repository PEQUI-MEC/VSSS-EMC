#ifndef PLAYER_HPP_
#define PLAYER_HPP_
#define PI 3.14159265453
#include "opencv2/opencv.hpp"
class player
{
public:
	
player()
	{
		bool backward = false;
		double thetaError = 0;
		double thetaErrorSum = 0;
		double thetaErrorPrevious = 0;
		float Kp = 0;
		float Kd = 0;
		float Ki = 0;
		float W = 0;
		float V = 0;
	} 



void player::control(pt::Point target,pt::Point b,bool fixate){ // passar b para o checa batida
	
	pid(target,pt::Point(robo.X,robo.Y),robo.FI);
	if(abs(robo.W) > 1)
		robo.W = robo.W/abs(robo.W);
	if(fixate){
		double d = sqrt(pow(robo.X - target.x,2)+pow(robo.Y - target.y,2));
		robo.V *= (1 - abs(robo.W))*d*4;
		robo.histWipe();
	}else{
		robo.V *= (1 - abs(robo.W));
	 }
	if(abs(robo.V) > 1)
		robo.V =  robo.V/abs(robo.V); 

	 //verifica se esta batido
	if (robo.checa_batida()){
		//printf("BATEU \n");
		//verifica se esta perto da bola
		if (sqrt(pow(robo.X - b.x, 2) + pow(robo.Y - b.y, 2)) < 0.08){
			//verifica o lado que deve girar
			if (b.y > 0.65){
				robo.W = -1.0;
				robo.V = 0.0;
				flagGirando = true;
			}
			else{
				robo.W = 1.0;
				robo.V = 0.0;
				flagGirando = true;
			}
		}else{ //se nao esta perto da bola, o robo deve dar re
			if(flagGirando){
					flagGirando = false; 
					robo.count_checa_batida = NUMBER_OF_CHECA_BATIDA;
					robo.histWipe();
					return;
				}
				else{
					robo.count_checa_batida--;
					robo.V=-1*abs(robo.V)/robo.V; 
					robo.W=0;			
				}
		}
	}

	// Impede o robo de entrar na nossa area do gol
	if( (target.x < 0.25 && target.y > LARGURA_CAMPO/2-TAMANHO_AREA/2 && target.y < LARGURA_CAMPO/2+TAMANHO_AREA/2) || (robo.X < 0.25 && robo.Y > LARGURA_CAMPO/2-TAMANHO_AREA/2 && robo.Y < LARGURA_CAMPO/2+TAMANHO_AREA/2) ){
		target.setX(0.35);
		/*if (target.y < LARGURA_CAMPO/2)
		target.setY(LARGURA_CAMPO/2-TAMANHO_AREA/2);
		else
		target.setY(LARGURA_CAMPO/2+TAMANHO_AREA/2);*/
		double dist =sqrt(pow(robo.Y - target.y,2));
		pid(target,pt::Point(robo.X,robo.Y),robo.FI);
		robo.V *= (1 - abs(robo.W))*dist*6; 
		robo.histWipe();
	}

	 
}

void player::pid(cv::Point targetPos, cv::Point currentPos, double currentTheta){
	
	double targetTheta = (atan2(targetPos.y - currentPos.y, targetPos.x - currentPos.x));
	
	currentTheta=atan2(sin(currentTheta),cos(currentTheta));

	if(((atan2(sin(targetTheta-currentTheta+PI/2),cos(targetTheta-currentTheta+PI/2))))<0){
		backward=true;
	}
	else{ 
		backward=false;
	}
	if (backward)
	{
		currentTheta=currentTheta+PI;
		currentTheta=atan2(sin(currentTheta),cos(currentTheta));
	}
	
	thetaError = atan2(sin(targetTheta-currentTheta), cos(targetTheta-currentTheta));

	thetaErrorSum+=thetaError;
	
	W = (Kp*thetaError + Kd*(thetaError - thetaErrorPrevious) + Ki*thetaErrorSum);
	thetaErrorPrevious = thetaError;

		if(backward){
		V = -1;	
	}else{
		V = 1;
	}	
}
	

};
#endif /* CONTROLGUI_HPP_ */




