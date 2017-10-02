#ifndef ROBOT_HPP_
#define ROBOT_HPP_
#define PI 3.14159265453
#define MAX_SAMPLES_HIST 25 // REGULA O TEMPO DE DETECÇÃO DA BATIDA
#define MAX_COLLISIONS 30 // REGULA O TEMPO DE REAÇÃO À BATIDA
#include "opencv2/opencv.hpp"
#include "../../cc/vision/tag.hpp"

class Robot
{
    public:
        cv::Point position, secundary, target, transTarget, ternary;
        char ID;
        bool pink = false;
        double orientation=0, transOrientation;
        double orientation2=0;
        int cmdType = 0;
        bool backward = false;
        double thetaError = 0;
        double thetaErrorSum = 0;
        double thetaErrorPrevious = 0;
        double vmax = 1.4; // m/s
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
        cv::Point rearPoint;

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
};
#endif /* ROBOT_HPP_ */
