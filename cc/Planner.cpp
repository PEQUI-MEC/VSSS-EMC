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
            vector = curve_control(robots.at(i).position, cv::Point(WIDTH/2, HEIGHT/2), ball, robots.at(i).vdefault);

            // salva o novo passo da trajetória no robô
            // - controle por vetor
            robots[i].cmdType = VECTOR;
            robots[i].transAngle = vector.angle;
            robots[i].vmax = vector.velocity;
            std::cout << "atualizou\n";
        }
        else {
            cv::Point target = robots.at(i).target;
            State predicted_state = predict_positions(robots.at(i).vmax);
            std::vector<Obstacle> obstacles = find_obstacles(predicted_state, i + 1, robots.at(i).target);
            // tem tamanho 2, o robô pode desviar pra direita ou para esquerda
            if(obstacles.size() > 0) {
                cv::Point * deviation_points = find_deviation(cv::Point(robots.at(i).position), cv::Point(robots.at(i).target), obstacles.at(0));
                target = deviation_points[0];
            }

            // salva o novo passo da trajetória no robô
            // - controle por pontos de intermédio
            robots.at(i).cmdType = POSITION;
            robots.at(i).target = target;
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

Planner::VelocityVector Planner::curve_control(cv::Point start, double start_orientation, cv::Point end, double end_orientation) {

}
Planner::VelocityVector Planner::curve_control(cv::Point start, cv::Point mid, cv::Point end, double vdefault) {
    Planner::VelocityVector vector;
    double a, b, c, x1, y1, x2, y2, x3, y3, baseTheta;
    cv::Point target, base = start;

    std::cout << "\nStart: " << start << " Mid: " << mid << " End: " << end << "\n";

    // coloca os y na escala correta
    start.y = compl_y(start.y);
    base.y = start.y; // é o mesmo valor
    mid.y = compl_y(mid.y);
    end.y = compl_y(end.y);

    // converte os pontos para a base do ponto start
    baseTheta = canonic_to_robot_base(&start, &mid, &end);
    std::cout << "baseTheta: " << baseTheta << "\nStart: " << start << " Mid: " << mid << " End: " << end << "\n";
    // seta os valores pra equação ficar mais legível
    x1 = start.x;
    y1 = start.y;
    x2 = mid.x;
    y2 = mid.y;
    x3 = end.x;
    y3 = end.y;

    // gera os coeficientes da curva
    a = (y2*(x3-x1) - y1*(x3-x2) - y3*(x2-x1)) /
        (x1*(x2-x3) - x3*(x2-x1) - x2*(x1-x3));

    b = (y2 - y1 + a*(x1 - x2)) /
        x2 - x1;

    c = -a*x1 - b*x1 + y1;
    
    // encontra o próximo ponto de alvo
    target.x = start.x + DELTA_X;
    target.y = a * pow(target.x, 2) + b * target.x + c;

    std::cout << "Raw Target: " << target <<"\n";

    // desconverte os pontos
    target = robot_base_to_canonic(base, baseTheta, target);

    // desconverte os Ys
    target.y = compl_y(target.y);
    start.y = compl_y(start.y);

    // calcula o angulo desse alvo
    vector.angle = atan2((target.y-start.y),-(target.x-start.x));
    std::cout << "target: " << target << " angle: " << vector.angle << "\na:" << a << " b:" << b << " c: " << c << "\n";
    // calcula a velocidade de forma a aproveitar ao máximo ângulos menores
    vector.velocity = vdefault * (1 - pow(vector.angle, 2));
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
std::vector<Planner::Obstacle> Planner::find_obstacles(State predicted_state, int start_index, cv::Point target) {
    std::vector<Obstacle> obstacles;
    double tempDist;

    // para cada objeto na pista, verifica se este é um obstáculo
    for(int i = 0; i < predicted_state.objects.size(); i++) {
        if(i == start_index)
            continue;

        // verifica se o obstáculo está perto suficiente da reta
        if((tempDist = distance_to_line(predicted_state.objects.at(start_index), target, predicted_state.objects.at(i))) < 2 * ROBOT_RADIUS) {
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
    cts.start = predicted_state.objects.at(start_index);

    std::sort (obstacles.begin(), obstacles.end(), cts);
}

// encontra desvios dados obstáculos e a reta de trajetória inicial
// !TODO ainda não tá pronto mas está calculado
cv::Point * Planner::find_deviation(cv::Point start, cv::Point end, Planner::Obstacle obstacle) {
    cv::Point * deviations = new cv::Point[2];

    // encontra o angulo entre start e end, soma 90 pra encontrar o angulo da perpendicular

    // gera uma reta a partir do obstacle.position

    return deviations;
}
