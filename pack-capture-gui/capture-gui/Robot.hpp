#ifndef ROBOT_HPP_
#define ROBOT_HPP_
#define PI 3.14159265453
#define MAX_SAMPLES_HIST 25 // REGULA O TEMPO DE DETECÇÃO DA BATIDA
#define MAX_COLLISIONS 30 // REGULA O TEMPO DE REAÇÃO À BATIDA
#include "opencv2/opencv.hpp"

class Robot
{
    public:
        cv::Point position, secundary, target, transTarget, ternary;
        char ID;
        bool pink = false;
        double orientation=0;
        double orientation2=0;
        bool backward = false;
        double thetaError = 0;
        double thetaErrorSum = 0;
        double thetaErrorPrevious = 0;
        double vmax = 6; // m/s
        cv::Point hist[MAX_SAMPLES_HIST];
        int hist_index =0;
        bool stuck = false;
        int count_collisions =0;
        float V = 6.0f;  // m/s
        float m=1; //Direção da velocidade
        float Vl,Vr;  // RPS
        bool spin = false;
        bool fixedPos=false;
        int role=0;
        int status = 0; // 0 = estado de jogo, 1 = pegando a bola, 2 = acelerando
        int previous_status =0;
        Robot()
        {
            ternary = cv::Point(-1,-1);
            secundary= cv::Point(-1,-1);
            position= cv::Point(-1,-1);
            target = cv::Point(-1,-1);
        }

        void feedHist(cv::Point pos) {
            hist[hist_index]=pos;
            hist_index++;
            if(hist_index==MAX_SAMPLES_HIST) {
                hist_index=0;
            }
        }


        void histWipe() {
            for (int i=0; i<MAX_SAMPLES_HIST; i++) {
                this->hist[i] = cv::Point(10*(double)rand(),10*(double)rand());
            }
            hist_index=0;
            count_collisions=MAX_COLLISIONS;
            stuck=false;
        }


        bool check_collision() {
            if(stuck) {
                count_collisions--;
                if(!spin)
                    count_collisions--;
                //std::cout<<count_collisions<<std::endl;
                if(count_collisions<=0) {
                    count_collisions = MAX_COLLISIONS;
                    histWipe();
                    stuck=false;
                    return false;
                }
                return true;
            } else if(is_Stuck()) {
                stuck=true;
                return true;
            } else
                return false;
        }


        bool is_Stuck() {
            if(hist_index<=MAX_SAMPLES_HIST/2)
                return false;
            float meanx=0;
            float meany=0;
            float stddivx=0;
            float stddivy=0;
            float result = 0;


            for(int i=0; i<MAX_SAMPLES_HIST; i++) {
                meanx+=hist[i].x;
                meany+=hist[i].y;
            }

            meanx/=MAX_SAMPLES_HIST;
            meany/=MAX_SAMPLES_HIST;

            for(int i=0; i<MAX_SAMPLES_HIST; i++) {
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

        void goTo(cv::Point targetPos,cv::Point Ball) {

            double currentTheta = orientation;
            cv::Point currentPos = position;
            float d = 1.0;
            double targetTheta = (atan2((targetPos.y - currentPos.y)*1.3/480, (targetPos.x - currentPos.x)*1.5/640));

            currentTheta=atan2(sin(currentTheta),cos(currentTheta));

            if(((atan2(sin(targetTheta-currentTheta+PI/2),cos(targetTheta-currentTheta+PI/2))))<0) {
                backward=true;
            }
            else {
                backward=false;
            }
            if (backward)
            {
                currentTheta=currentTheta+PI;
                currentTheta=atan2(sin(currentTheta),cos(currentTheta));
            }


            if(backward) {
                m = -1;
            } else {
                m = 1;
            }
            if(fixedPos) {
                d = sqrt(pow((targetPos.y - currentPos.y),2)+pow((targetPos.x - currentPos.x),2))/30;

                if(d>1)
                    d=1;
                else {
                    histWipe();
                    if(d<0.3)
                        d=0;
                }
            }

            double LOS = atan2(sin(targetTheta-currentTheta),cos(targetTheta-currentTheta));



            if(fixedPos&&((LOS < PI/6 && LOS > -PI/6)||(LOS > 5*PI/6 && LOS < -5*PI/6))) {
                Vr = (d*(m-sin(LOS)));
                Vl = (d*(m+sin(LOS)));

            } else if(fixedPos&&(sqrt(pow((targetPos.y - currentPos.y),2)+pow((targetPos.x - currentPos.x),2))<30)) {
                Vl = (abs(LOS)/LOS)*(d*(1+sin(abs(LOS))))/8;
                Vr = -(abs(LOS)/LOS)*(d*(1+sin(abs(LOS))))/8;

            } else {

                Vr = (d*(m-sin(LOS)));
                Vl = (d*(m+sin(LOS)));
            }
            //cout<<"LOS "<<LOS*180/PI<<" | "<<Vr<<" | "<<Vl<<" | d "<<d<<endl;


            if (abs(Vl)>1) {
                Vl=1*Vl/abs(Vl);
            }
            if (abs(Vr)>1) {
                Vr=1*Vr/abs(Vr);
            }


            switch(status) {
            // 0 = estado de jogo, 1 = pegando a bola, 2 = acelerando
            case 0:
                //if(V<vmax)
                //V=V+0.05;
                //else if(V>vmax)
                //V=V-0.05;
                //else
                V=vmax;
                break;
            case 1:
                V=V-0.2;
                if(V<vmax-1)
                    V=vmax-1;
                break;
            case 2:
                V=V+0.1;
                if(V>vmax+1)
                    V=vmax+1;
                break;
            case 3:
                V=vmax;
                break;
            case 4:
                //std::cout << "GIRA GIRA GIRA"<<endl; ;
                V=vmax+1;
                if(currentPos.y<targetPos.y) {
                    if(role==3){
                    Vl=1;
                    Vr=-1;
                    }else{
					Vr=1;
                    Vl=-1;
					}
                } else {
                    if(role==3){
                    Vl=-1;
                    Vr=1;
                    }else{
					Vr=-1;
                    Vl=1;
					}
                }
                break;
            default:
                V=vmax;
                break;
            }
            previous_status = status;
            //std::cout << "Vr " <<Vr<<"Vl "<<Vl<<std::endl;
            Vl=V*Vl;
            Vr=V*Vr;


            if(check_collision()) {
                //verifica se esta perto da bola
                if (sqrt(pow((Ball.y - currentPos.y),2)+pow((Ball.x - currentPos.x),2)) < 20) {
                    //std::cout<<"gira"<<std::endl;
                    //verifica o lado que deve girar
                   if(role==3){
                    if (targetPos.y < 240) {
                        Vr = vmax;
                        Vl = -vmax;
                        spin = true;
                    }
                    else {
                        Vr = -vmax;
                        Vl = vmax;
                        spin = true;
                    }
				}
                    else{
						if (targetPos.y > 240) {
                        Vr = vmax;
                        Vl = -vmax;
                        spin = true;
                    }
                    else {
                        Vr = -vmax;
                        Vl = vmax;
                        spin = true;
                    }


						}
                } else { //se nao esta perto da bola, o robo deve dar re
                    if(spin) {
                        spin = false;
                        count_collisions = MAX_COLLISIONS;
                        histWipe();
                        return;
                    }
                    else {
                        double temp = -vmax*(Vr+Vl)/abs(Vr+Vl);
                        Vr = temp;
                        Vl = temp;
                    }
                }
            }

        }

};
#endif /* ROBOT_HPP_ */
