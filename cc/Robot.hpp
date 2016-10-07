#ifndef ROBOT_HPP_
#define ROBOT_HPP_
#define PI 3.14159265453
#define MAX_SAMPLES_HIST 40 // REGULA O TEMPO DE DETECÇÃO DA BATIDA
#define MAX_COLLISIONS 20 // REGULA O TEMPO DE REAÇÃO À BATIDA
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
		double vmax = 6; // m/s
		double wmax = 6; // rad/s
		cv::Point hist[MAX_SAMPLES_HIST];
		int hist_index =0;
		bool stuck = false;
		int count_collisions =0;
		float W = 0;  // rad/s
		float V = 0;  // m/s
		float Vl,Vr;  // RPS
		bool spin = false;
		bool fixedPos=false;
		int role=0;
Robot()
	{
		secundary= cv::Point(-1,-1);
		position= cv::Point(-1,-1);
		target = cv::Point(-1,-1);
	}

void feedHist(cv::Point pos){
	hist[hist_index]=pos;
	hist_index++;
	if(hist_index==MAX_SAMPLES_HIST){
		hist_index=0;
	}
	}
	
	
void histWipe(){
	for (int i=0;i<MAX_SAMPLES_HIST;i++){
		this->hist[i] = cv::Point(10*(double)rand(),10*(double)rand());	
	}
	hist_index=0;
	count_collisions=MAX_COLLISIONS; 
	stuck=false;
	}
	
	
bool check_collision(){
	if(stuck){
		count_collisions--;
		//std::cout<<count_collisions<<std::endl; 
		if(count_collisions<=0){
			count_collisions = MAX_COLLISIONS;
			histWipe();
			stuck=false;
			return false;
		}
		return true;
	}else if(is_Stuck()){
		stuck=true;
		return true;
	}else 	
		return false;
}


bool is_Stuck(){
	if(hist_index<=MAX_SAMPLES_HIST/2)
		return false;
	float meanx=0;
	float meany=0;
	float stddivx=0;
	float stddivy=0;
	float result = 0;


	for(int i=0;i<MAX_SAMPLES_HIST;i++){
		meanx+=hist[i].x;
		meany+=hist[i].y;
	}

	meanx/=MAX_SAMPLES_HIST;
	meany/=MAX_SAMPLES_HIST;

	for(int i=0;i<MAX_SAMPLES_HIST;i++){
		stddivx += pow((hist[i].x-meanx),2);
		stddivy += pow((hist[i].y-meany),2);
	}

	stddivx = sqrt(stddivx/MAX_SAMPLES_HIST);
	stddivy = sqrt(stddivy/MAX_SAMPLES_HIST);

	result = sqrt(pow(stddivx,2)+pow(stddivy,2));

	//std::cout<<" result : "<<this->result<<std::endl;
	if (abs(result) < 5)
		return true;
	else 
		return  false;


}
void goTo(cv::Point targetPos){
	
	 double currentTheta = orientation;
	 cv::Point currentPos = position;
	 float d = 1.0;
	double targetTheta = (atan2((targetPos.y - currentPos.y)*1.3/480, (targetPos.x - currentPos.x)*1.5/640));
	
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

		if(backward){
		V = -1;	
		}else{
		V = 1;
		}
		if(fixedPos){
		d = sqrt(pow((targetPos.y - currentPos.y),2)+pow((targetPos.x - currentPos.x),2))/30;
		
		if(d>1)
		d=1;
		else{
		histWipe();
		if(d<0.3)
		d=0;
		}
		}
		Vr = d*((V-sin(targetTheta-currentTheta)));
		Vl = d*((V+sin(targetTheta-currentTheta)));
		
		if (abs(Vl)>1){
		Vl=1*Vl/abs(Vl);
		}
		if (abs(Vr)>1){
		Vr=1*Vr/abs(Vr);
		}
	
	

		Vl=vmax*Vl;
		Vr=vmax*Vr;
		
		
		if(check_collision()){
		//verifica se esta perto da bola
		if (sqrt(pow((targetPos.y - currentPos.y),2)+pow((targetPos.x - currentPos.x),2)) < 20){
			//std::cout<<"gira"<<std::endl;
			//verifica o lado que deve girar
			if (targetPos.y > 240){
			Vr = vmax;
			Vl = -vmax;
				spin = true;
			}
			else{
				Vr = -vmax;
				Vl = vmax;
				spin = true;
			}
		}else{ //se nao esta perto da bola, o robo deve dar re
			if(spin){
					spin = false; 
					count_collisions = MAX_COLLISIONS;
					histWipe();
					return;
				}
				else{
					double temp = -vmax*(Vr+Vl)/abs(Vr+Vl);
					Vr = temp;	
					Vl = temp;
				}
		}
	}

}
	
	}; 
#endif /* ROBOT_HPP_ */
