#ifndef PLANNER_HPP_
#define PLANNER_HPP_

#include "opencv2/opencv.hpp"

class Planner {
private:
    // armazenar últimos três frames para previsão
public:
    // criar função de planejamento de trajetória
    // controle por curva
    cv::Point plan();
    cv::Point plan();
};

#endif /* TAG_HPP_ */
