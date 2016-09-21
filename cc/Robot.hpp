#ifndef ROBOT_HPP_
#define ROBOT_HPP_
#define PI 3.14159265453
#include "opencv2/opencv.hpp"

class Robot
{
public:
		cv::Point position, secundary, target;
		char ID;
		double orientation;
		bool backward = false;
		double thetaError = 0;
		double thetaErrorSum = 0;
		double thetaErrorPrevious = 0;
		double vmax = 0.5*(400  * 2 * PI*0.016 /60); // m/s
		double wmax = (2*vmax)/0.08; // rad/s
		
		float Kp = 0.35;
		float Kd = 0;
		float Ki = 0;
		float W = 0;  // rad/s
		float V = 0;  // m/s
		float Vl,Vr;  // RPS
Robot()
	{
		secundary= cv::Point(-1,-1);
		position= cv::Point(-1,-1);
		target = cv::Point(-1,-1);
	}
Robot(float vel_suppressor)
	{
		vmax=vmax*vel_suppressor;
		secundary= cv::Point(-1,-1);
		position= cv::Point(-1,-1);
		target = cv::Point(-1,-1);
	}

void goTo(cv::Point targetPos){
	 double currentTheta = orientation;
	 cv::Point currentPos = position;
	
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
		Vr	= (((2*V*vmax) - (W*wmax*0.08))/(2*0.016))/(2*PI); 
		Vl	= (((2*V*vmax) + (W*wmax*0.08))/(2*0.016))/(2*PI); 
		
	/*
		if(abs(Vr)>6&&abs(Vr)>abs(Vl)){
			
			
		double ratio = abs(Vr)/abs(Vl);
		Vl = 6*(abs(Vl)/Vl)/ratio; 	
		Vr = 6*abs(Vr)/Vr;
		
		
		
		}else if(abs(Vl)>6&&abs(Vl)>abs(Vr)){
			
		double ratio = abs(Vl)/abs(Vr);
		Vr = 6*(abs(Vr)/Vr)/ratio; 	
		Vl = 6*abs(Vl)/Vl;
		
		}else if (abs(Vl)>6&&abs(Vl)==abs(Vr)){
			Vr=6*(abs(Vr)/Vr);
			Vl=6*(abs(Vl)/Vl);
			}
		*/
}
	
	}; 
#endif /* CONTROLGUI_HPP_ */
