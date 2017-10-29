#ifndef PLANNER_HPP_
#define PLANNER_HPP_

#include <vector>
#include "opencv2/opencv.hpp"
#include "../pack-capture-gui/capture-gui/Robot.hpp"

using namespace cv;
using namespace std;

class Planner {
private:
    typedef struct _obstacle {
        Point position;
        double distance;
    } Obstacle;

    vector<Point> hist; // armazena os três últimos frames

    // recebe três pontos e retorna o ângulo do vetor que deve ser executado para a curva nesse passo
    double curve_control(Point start, Point mid, Point end);

public:
    // criar função de planejamento de trajetória
    // controle por curva

    // recebe um ponteiro para os robôs pois após tudo ser calculado os alvos devem ser atualizados
    void plan(vector<Robot> * pRobots, Point * advRobots, Point ball);
};

#endif /* TAG_HPP_ */
