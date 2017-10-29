#include "Planner.hpp"

void Planner::set_constants(int midfieldX, int midgoalY, int maxY) {
    COORD_MID_FIELD_X = midfieldX;
    MEIO_GOL_Y = midgoalY;
    MAX_Y = maxY;
}

Planner::State Planner::gen_state(std::vector<Robot> robots, cv::Point * advRobots, cv::Point ball) {
    State new_state;

    new_state.objects.push_back(ball);
    for(int i = 0; i < 3; i++) {
        if(i >= robots.size())
            new_state.objects.push_back(cv::Point(-1, -1));
        else
            new_state.objects.push_back(robots.at(i).position);
    }
    for(int i = 0; i < 3; i++) {
        new_state.objects.push_back(advRobots[i]);
    }

    return new_state;
}

void Planner::plan(std::vector<Robot> * pRobots, cv::Point * advRobots, cv::Point ball) {
    std::vector<Robot> robots = *pRobots;

    State current_state = gen_state(*pRobots, advRobots, ball);

    // atualiza o histórico para fazer as predições
    update_hist(current_state);

    // planeja os alvos de cada robô
    for(int i = 0; i < robots.size(); i++) {
        // só planeja a trajetória de robôs que têm um alvo
        if(robots.at(i).cmdType != POSITION)
            continue;

        // desabilita o campo potencial para esse robô
        robots.at(i).using_pot_field = false;

        // !TODO curve_control não funciona
        // teste do controle por cuva fazendo com que sempe que seja passado um ponto como alvo
        // ele desenvolva uma curva que passa pelo meio do campo
        //Planner::VelocityVector vector;
        //vector = curve_control(robots.at(i).position, cv::Point(COORD_MID_FIELD_X, MEIO_GOL_Y), robots.at(i).target, robots.at(i).vdefault);

        State predicted_state = predict_positions(robots.at(i).vmax);

        std::vector<Obstacle> obstacles = find_obstacles(predicted_state, i + 1, robots.at(i).target);

        cv::Point * deviation_points = find_deviation(robots.at(i).position, robots.at(i).target);

        // salva o novo passo da trajetória no robô
        robots[i].target = vector.angle;
    }
    // atualiza o vetor pra estratégia
    *pRobots = robots;
}

Planner::VelocityVector Planner::curve_control(cv::Point start, cv::Point mid, cv::Point end, double vdefault) {
    Planner::VelocityVector vector;
    double a, b, c, x1, y1, x2, y2, x3, y3, derivada;
    cv::Point target;

    // seta os valores pra equação ficar mais legível
    // inverte o lado para que y cresce para a parábola ficar correta
    // !TODO encontrar uma forma de transformar esses pontos para a origem e voltar para as coordenadas do campo
    x1 = start.x;
    y1 = MAX_Y - start.y;
    x2 = mid.x;
    y2 = MAX_Y - mid.y;
    x3 = end.x;
    y3 = MAX_Y - end.y;

    // gera os coeficientes da curva
    a = y1/((x1-x2)*(x1-x3)) +
        y2/((x2-x1)*(x2-x3)) +
        y3/((x3-x1)*(x3-x2));

    b = -y1*(x2+x3)/((x1-x2)*(x1-x3))
        -y2*(x1+x3)/((x2-x1)*(x2-x3))
        -y3*(x1+x2)/((x3-x1)*(x3-x2));

    c = y1*x2*x3/((x1-x2)*(x1-x3))
      + y2*x1*x3/((x2-x1)*(x2-x3))
      + y3*x1*x2/((x3-x1)*(x3-x2));


    vector.angle = atan2(double(start.y - target.y), - double(start.x - target.x))

    // calcula a velocidade de forma a aproveitar ao máximo ângulos menores
    vector.velocity = vdefault * (1 - pow(vector.angle, 2));
    // faz um crop caso o ângulo seja muito grande
    if(vector.velocity < 0.5)
        vector.velocity = 0.5;

    return vector;
}

void Planner::update_hist(State current_state) {
    hist.push_back(current_state);
    if(hist.size() > HIST_SIZE) {
        hist.erase(hist.begin());
    }
}

void predict_positions(double timeAhead) {
    return hist.end();
}

double distance_to_line(cv::Point start, cv::Point end, cv::Point mid){
    // translada para a origem
    end -= start;
    mid -= start;

    double area = mid.x*end.y - mid.y*end.x;

    return area / norm(end);
}

double Planner::distance(cv::Point p1, cv::Point p2) {
  return sqrt(pow(p1.x-p2.x,2) + pow(p1.y-p2.y,2));
}

std::vector<Obstacle> Planner::find_obstacles(State predicted_state, int start_index, cv::Point target) {
    std::vector<Obstacle> obstacles;
    double tempDist;

    // para cada objeto na pista, verifica se este é um obstáculo
    for(int i = 0; i < predicted_state.size(); i++) {
        if(i == start_index)
            continue;

        // verifica se o obstáculo está perto suficiente da reta
        if((tempDist = distance_to_line(predicted_state.at(start_index), target, predicted_state.at(i))) < 2 * ROBOT_RADIUS) {
            Obstacle obstacle;
            obstacle.position = predicted_state.at(i);
            obstacle.distance = tempDist;
            obstacles.push_back(obstacle);
        }
    }

    // ordena o vetor de obstáculos para que obstáculos mais próximos apareçam primeiro
    // cria o operador
    struct closest_to_start {
        inline bool operator() (const Obstacle& obstacle1, const Obstacle& obstacle2) {
            return (distance(predicted_state.at(start_index), obstacle1.position) < distance(predicted_state.at(start_index), obstacle2.position));
        }
    };
    // ordena
    std::sort (obstacles.begin(), obstacles.end(), closest_to_start());
}

cv::Point * find_deviation(cv::Point start, cv::Point end, Obstacle obstacle) {
    cv::Point * deviations = new cv::Point[2];

    // encontra o angulo entre start e end, soma 90 pra encontrar o angulo da perpendicular

    // gera uma reta a partir do obstacle

    return deviations;
}
