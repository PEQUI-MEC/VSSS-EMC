#ifndef PLANNER_HPP_
#define PLANNER_HPP_

#include <vector>
#include "opencv2/opencv.hpp"
#include "../pack-capture-gui/capture-gui/Robot.hpp"

#define PI 3.14159265453
#define HIST_SIZE 3
#define DELTA_X 3
#define ROBOT_RADIUS 17

#define POSITION 0
#define SPEED 1
#define ORIENTATION 2
#define VECTOR 3
#define VECTORRAW 4

class Planner {
private:
    // tipos de dados
    typedef struct _obstacle {
        cv::Point position;
        double distance;
    } Obstacle;
    typedef struct _state {
        std::vector<cv::Point> objects;
    } State;
    typedef struct _velocityVector {
        double angle;
        float velocity;
    } VelocityVector;

    // constantes
    int WIDTH, HEIGHT;
    // histórico de estados: armazena os HIST_SIZE últimos frames
    std::vector<State> hist;

    // gera salva o estado atual no histórico
    void update_hist(State current_state);

    // pega os dados e gera um estado
    State gen_state(std::vector<Robot> robots, cv::Point * advRobots, cv::Point ball);

    // encontra os obstáculos entre o ponto de início e de fim
    std::vector<Obstacle> find_obstacles(State predicted_state, cv::Point startPos, cv::Point target);

    double distance(cv::Point p1, cv::Point p2);

    double distance_to_line(cv::Point start, cv::Point end, cv::Point point);

    cv::Point * find_deviation(cv::Point start, cv::Point end, Obstacle obstacle);

    bool validate_target(cv::Point target);

public:
    // criar função de planejamento de trajetória
    // controle por curva

    // recebe um ponteiro para os robôs pois após tudo ser calculado os alvos devem ser atualizados
    void plan(int robot_index, std::vector<Robot> * pRobots, cv::Point * advRobots, cv::Point ball, bool use_this, cv::Point * obstacle, cv::Point * deviation1, cv::Point * deviation2);

    void set_constants(int width, int height);

    State predict_positions(double timeAhead);
};

#endif /* TAG_HPP_ */
