#include "Planner.hpp"

// recalcula os targets dos robôs controlados por posição desviando de obstáculos
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

        // teste do controle por cuva fazendo com que sempe que seja passado um ponto como alvo
        // ele desenvolva uma curva que passa pelo meio do campo
        if(true) {
            Planner::VelocityVector vector;
            // parâmetros para fazer com que o robô sempre chegue em seu alvo alinhado com o gol
            vector = curved_arrival_control(robots.at(i).position, robots.at(i).target, cv::Point(WIDTH, HEIGHT/2), robots.at(i).vdefault);

            // salva o novo passo da trajetória no robô
            // - controle por vetor
            robots[i].cmdType = VECTOR;
            robots[i].transAngle = vector.angle;
            robots[i].vmax = vector.velocity;
        }
        else {
            cv::Point target = robots.at(i).target;
            State predicted_state = predict_positions(robots.at(i).vmax);
            // encontra os obstáculos na trajetória; i+1 pois o vetor de estados começa com a bola
            std::vector<Obstacle> obstacles = find_obstacles(predicted_state, predicted_state.objects.at(i + 1), robots.at(i).target);
            // se há obstáculos
            if(obstacles.size() > 0) {
                // tem tamanho 2, o robô pode desviar pra direita ou para esquerda
                cv::Point * deviation_points = find_deviation(robots.at(i).position, target, obstacles.at(0));
                // calcula pelo menos mais um obstáculo a partir desse ponto para ser possível gerar a curva
                obstacles = find_obstacles(predicted_state, deviation_points[0], robots.at(i).target);
                // se ainda há obstáculos
                if(obstacles.size() > 0) {
                    cv::Point * sec_deviation_points = find_deviation(deviation_points[0], target, obstacles.at(0));
                    target = sec_deviation_points[0];
                }

                Planner::VelocityVector vector;
                vector = curved_deviation_control(robots.at(i).position, deviation_points[0], target, robots.at(i).vdefault);

                // salva o novo passo da trajetória no robô
                // - controle por vetor
                robots[i].cmdType = VECTOR;
                robots[i].transAngle = vector.angle;
                robots[i].vmax = vector.velocity;
            }
            else {
                // salva o novo passo da trajetória no robô
                // - controle por pontos de intermédio
                robots.at(i).cmdType = POSITION;
                robots.at(i).target = target;
            }
        }
    }
    // atualiza o vetor pra estratégia
    *pRobots = robots;
}

// função de inicialização do Planner
void Planner::set_constants(int width, int height) {
    WIDTH = width;
    HEIGHT = height;
}

// gera um estado a partir dos pontos
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

// função que faz a transformação da base canônica para base na qual o robô é a origem
// retorna a base que foi encontrada
double Planner::canonic_to_robot_base(cv::Point * origin, cv::Point * mid, cv::Point * target) {
    // só pra ficar mais fácil de trabalhar
    cv::Point ori = *origin, mi = *mid, tar = *target;
    double theta;

    // translada pra origem
    mi -= ori;
    tar -= ori;
    ori -= ori;

    // encontra a base
    // - encontra a inclinação do eixo
    theta = atan2(tar.y, tar.x);

    // faz a mudança de base
    tar = cv::Point(tar.x*cos(-theta) - tar.y*sin(-theta), tar.x*sin(-theta) + tar.y*cos(-theta));
    mi = cv::Point(mi.x*cos(-theta) - mi.y*sin(-theta), mi.x*sin(-theta) + mi.y*cos(-theta));

    // atualiza os valores a retornar
    *origin = ori;
    *mid = mi;
    *target = tar;

    return theta;
}
// função que faz a transformação da base na qual o robô é a origem para a base canônica
cv::Point Planner::robot_base_to_canonic(cv::Point base, double theta, cv::Point point) {
    // desfaz a rotação do vetor
    point = cv::Point(point.x*cos(theta) - point.y*sin(theta), point.x*sin(theta) + point.y*cos(theta));
    // translada pro ponto original
    point += base;

    return point;
}

// na visão, o y cresce pra baixo. faz ele crescer pra cima e vice-versa
double Planner::compl_y(double y) {
    return HEIGHT - y;
}

/* GERAÇÃO DA CURVA */
double Planner::determinant(double a, double b, double c, double d) {
    return (a * d) - (b * c);
}
bool Planner::find_parabola(double * a, double * b, double * c, cv::Point p1, cv::Point p2, cv::Point p3) {
    /*
    a = y1/((x1-x2)*(x1-x3)) +
        y2/((x2-x1)*(x2-x3)) +
        y3/((x3-x1)*(x3-x2));

    b = -y1*(x2+x3)/((x1-x2)*(x1-x3))
        -y2*(x1+x3)/((x2-x1)*(x2-x3))
        -y3*(x1+x2)/((x3-x1)*(x3-x2));

    c = y1*x2*x3/((x1-x2)*(x1-x3))
      + y2*x1*x3/((x2-x1)*(x2-x3))
      + y3*x1*x2/((x3-x1)*(x3-x2));
    */
    double ax = p1.x;
    double ay = p1.y;
    double bx = p2.x;
    double by = p2.y;
    double cx = p3.x;
    double cy = p3.y;

    double A1 = (ax * ax) - (bx * bx);
    double B1 = ax - bx;
    double C1 = ay - by;
    double A2 = (bx * bx) - (cx * cx);
    double B2 = bx - cx;
    double C2 = by - cy;

    double det = determinant(A1, B1, A2, B2);
    double deta = determinant(C1, B1, C2, B2);
    double detb = determinant(A1, C1, A2, C2);

    if(det != 0) {
        double coefa = deta / det;
        double coefb = detb / det;
        double cc1 = coefa * ax;
        double cc2 = ay - (cc1 * ax);
        double coefc = cc2 - (coefb * ax);

        *a = coefa;
        *b = coefb;
        *c = coefc;
        return true;
    } else {
        // não foi possível encontrar a equação
        return false;
    }
}
/* CURVA GERADA */

// resolve um sistema de equações de três variáveis e três incógnitas
bool Planner::solve_eq_system(double * a, double * b, double * c, cv::Point p1, cv::Point p2, double end_slope) {
    double D = (p1.x*p2.x+pow(p2.x, 2))-(pow(p1.x, 2)+p2.x*p2.x);

    if(D != 0) {
        *a = ((p2.x*end_slope+p1.y)-(p1.x*end_slope+p2.y))/D;
        *b = ((pow(p1.x, 2)*end_slope+p2.x*p2.y)-(pow(p2.x, 2)*end_slope+p1.y*p2.x))/D;
        *c = ((pow(p1.x, 2)*p2.y+p1.x*pow(p2.x, 2)*end_slope+p1.y*p2.x*p2.x)-(pow(p1.x, 2)*p2.x*end_slope+p1.x*p2.x*p2.y+p1.y*pow(p2.x, 2)))/D;
        return true;
    }
    else {
        return false;
    }
}

Planner::VelocityVector Planner::curved_arrival_control(cv::Point start, cv::Point end, cv::Point after_end, double vdefault) {
    Planner::VelocityVector vector;
    double a, b, c, slope, baseTheta;
    cv::Point target, base = start;

    // coloca os y pra crescer pra cima
    start.y = compl_y(start.y);
    base.y = start.y; // são o mesmo valor
    end.y = compl_y(end.y);
    after_end.y = compl_y(after_end.y);

    // converte os pontos para a base do ponto start
    end -= start;
    after_end -= start;
    start -= start;
    baseTheta = atan2(end.y, end.x);
    end = cv::Point(end.x*cos(-baseTheta) - end.y*sin(-baseTheta), end.x*sin(-baseTheta) + end.y*cos(-baseTheta));
    after_end = cv::Point(after_end.x*cos(-baseTheta) - after_end.y*sin(-baseTheta), after_end.x*sin(-baseTheta) + after_end.y*cos(-baseTheta));

    // calcula o slope da reta que o robô deve chegar
    slope = (after_end.y - end.y) / (after_end.x - end.x);

    // verifica se o sistema é resolvível
    if(solve_eq_system(&a, &b, &c, start, end, slope)) {
        // encontra o próximo ponto de alvo
        target.x = start.x + DELTA_X;
        // aplica a equação
        target.y = a * pow(target.x, 2) + b * target.x + c;

        std::cout << "Raw Target: " << target <<"\n";

        // desconverte o alvo encontrado
        target = robot_base_to_canonic(base, baseTheta, target);
        // desconverte o Y do alvo
        target.y = compl_y(target.y);
    } // senão, vai reto mesmo
    else {
        target = end;
        // desconverte o alvo encontrado
        target = robot_base_to_canonic(base, baseTheta, target);
        // desconverte o Y do alvo
        target.y = compl_y(target.y);
        //std::cout << "Deu não\n";
    }

    // volta o start pra posição que tava antes e desconverte seu Y
    start = robot_base_to_canonic(base, baseTheta, start);
    start.y = compl_y(start.y);

    // calcula o angulo do alvo calculado
    vector.angle = atan2((target.y-start.y),-(target.x-start.x));

    std::cout << "target: " << target << " angle: " << vector.angle << "\n";

    // calcula a velocidade de forma a aproveitar ao máximo ângulos menores
    vector.velocity = vdefault * (1 - pow(vector.angle, 4));
    // faz um crop caso o ângulo seja muito grande
    if(vector.velocity < 0.5)
        vector.velocity = 0.5;
}
Planner::VelocityVector Planner::curved_deviation_control(cv::Point start, cv::Point mid, cv::Point end, double vdefault) {
    Planner::VelocityVector vector;
    double a, b, c, baseTheta;
    cv::Point target, base = start;

    std::cout << "\nStart: " << start << " Mid: " << mid << " End: " << end << "\n";

    // coloca os y pra crescer pra cima
    start.y = compl_y(start.y);
    base.y = start.y; // é o mesmo valor
    mid.y = compl_y(mid.y);
    end.y = compl_y(end.y);

    // converte os pontos para a base do ponto start
    baseTheta = canonic_to_robot_base(&start, &mid, &end);

    std::cout << "baseTheta: " << baseTheta << "\nStart: " << start << " Mid: " << mid << " End: " << end << "\n";

    // gera os coeficientes da curva
    if(find_parabola(&a, &b, &c, start, mid, end)) {
        std::cout << "a:" << a << " b:" << b << " c: " << c << "\n";

        // encontra o próximo ponto de alvo
        target.x = start.x + DELTA_X;
        // aplica a equação
        target.y = a * pow(target.x, 2) + b * target.x + c;

        std::cout << "Raw Target: " << target <<"\n";

        // desconverte o alvo encontrado
        target = robot_base_to_canonic(base, baseTheta, target);
        // desconverte o Y do alvo
        target.y = compl_y(target.y);

        std::cout << target << "\n";
    }
    else {
        target = end;
        // desconverte o alvo encontrado
        target = robot_base_to_canonic(base, baseTheta, target);
        // desconverte o Y do alvo
        target.y = compl_y(target.y);
    }

    // volta o start pra posição que tava antes e desconverte seu Y
    start = robot_base_to_canonic(base, baseTheta, start);
    start.y = compl_y(start.y);

    // calcula o angulo do alvo calculado
    vector.angle = atan2((target.y-start.y),-(target.x-start.x));

    std::cout << "target: " << target << " angle: " << vector.angle << "\n";

    // calcula a velocidade de forma a aproveitar ao máximo ângulos menores
    vector.velocity = vdefault * (1 - pow(vector.angle, 4));
    // faz um crop caso o ângulo seja muito grande
    if(vector.velocity < 0.5)
        vector.velocity = 0.5;

    return vector;
}

// atualiza o histórico de estados
void Planner::update_hist(State current_state) {
    hist.push_back(current_state);
    if(hist.size() > HIST_SIZE) {
        hist.erase(hist.begin());
    }
}

// prevê posições futuras baseadas no tempo
// !TODO ainda não leva em consideração nada da física e nem usa o tempo
Planner::State Planner::predict_positions(double timeAhead) {
    return hist.back();
}

// calcula a distância entre um ponto e uma reta
double Planner::distance_to_line(cv::Point start, cv::Point end, cv::Point mid){
    // translada para a origem
    end -= start;
    mid -= start;

    double area = mid.x*end.y - mid.y*end.x;

    return area / norm(end);
}

// encontra obstáculos entre dois pontos e retorna um vetor ordenado
// os obstáculos mais próximos do ponto de início vêm primeiro
std::vector<Planner::Obstacle> Planner::find_obstacles(State predicted_state, cv::Point startPos, cv::Point target) {
    std::vector<Obstacle> obstacles;
    double tempDist;

    // para cada objeto na pista, verifica se este é um obstáculo
    for(int i = 0; i < predicted_state.objects.size(); i++) {
        if(predicted_state.objects.at(i) == startPos)
            continue;

        // verifica se o obstáculo está perto suficiente da reta
        if((tempDist = distance_to_line(startPos, target, predicted_state.objects.at(i))) < 2 * ROBOT_RADIUS) {
            Obstacle obstacle;
            obstacle.position = predicted_state.objects.at(i);
            obstacle.distance = tempDist;
            obstacles.push_back(obstacle);
        }
    }

    // ordena o vetor de obstáculos para que obstáculos mais próximos apareçam primeiro
    // cria o operador
    struct closest_to_start {
        cv::Point start;

        double distance(cv::Point p1, cv::Point p2) {
            return sqrt(pow(p1.x-p2.x,2) + pow(p1.y-p2.y,2));
        }

        inline bool operator () (const Obstacle& obstacle1, const Obstacle& obstacle2) {
            double dist1 = distance(start, obstacle1.position);
            double dist2 = distance(start, obstacle2.position);
            return dist1 < dist2;
        }
    };
    // ordena
    struct closest_to_start cts;
    cts.start = startPos;

    std::sort (obstacles.begin(), obstacles.end(), cts);

    return obstacles;
}

// encontra desvios dados obstáculos e a reta de trajetória inicial
cv::Point * Planner::find_deviation(cv::Point start, cv::Point end, Planner::Obstacle obstacle) {
    cv::Point * deviations = new cv::Point[2];

    // encontra o angulo entre start e end, soma 90 pra encontrar o angulo da perpendicular
    double angle = atan2(double(start.y - end.y), - double(start.x - end.x));
    angle -= PI/2;

    // gera uma reta a partir do obstacle.position
    deviations[0] = cv::Point(obstacle.position.x + 2*ROBOT_RADIUS*cos(angle), obstacle.position.y + 2*ROBOT_RADIUS*sin(angle));
    deviations[1] = cv::Point(obstacle.position.x + 2*ROBOT_RADIUS*cos(angle + PI/2), obstacle.position.y + 2*ROBOT_RADIUS*sin(angle + PI/2));

    // ordena pro desvio mais próximo
    if(distance_to_line(start, end, deviations[0]) > distance_to_line(start, end, deviations[1])) {
        cv::Point aux = deviations[0];
        deviations[0] = deviations[1];
        deviations[1] = aux;
    }

    return deviations;
}
