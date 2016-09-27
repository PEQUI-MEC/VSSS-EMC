#ifndef STRATEGY_HPP_
#define STRATEGY_HPP_
#define PI 3.14159265453
#include "opencv2/opencv.hpp"
#include "Robot.hpp"
#include "SerialW.hpp"
class Strategy
{
public:
		Robot Goalkeeper;
		Robot Attack;
		Robot Defense;
		cv::Point Ball;
		SerialW serial;
Strategy()
	{

		
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
void set_Ball(cv::Point b){
	Ball = b;
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
void atk_strategy(){
	
	
	
	}
void def_strategy(){
	
	
	
	}
	
	}; 
#endif /* STRATEGY_HPP_ */
