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




