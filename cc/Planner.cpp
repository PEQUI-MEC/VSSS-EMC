#include "Planner.hpp"

// recalcula os targets dos robôs controlados por posição desviando de obstáculos
void Planner::plan(int robot_index, std::vector<Robot> * pRobots) {
    if(!use_this || pRobots == NULL ||
        robot_index < 0 || robot_index >= (*pRobots).size() || (*pRobots).at(robot_index).ignore_obstacles) {
        //std::cout << "Wrong state. Planning not used.\n";
        return;
    }

    // pra ficar mais fácil manipular o vetor
    std::vector<Robot> robots = *pRobots;

    // desabilita o campo potencial para esse robô
    robots.at(robot_index).using_pot_field = false;

    // auxiliar de fallback caso não seja necessário mudar o alvo do robô
    cv::Point target = robots.at(robot_index).target;
    //std::cout << "(" << robot_index + 1 << ") my position: " << robots.at(robot_index).position << "\n";
    //std::cout << "(" << robot_index + 1 << ") original target: " << target << "\n";

    // prevê estados futuros
    State predicted_state = predict_positions(0); // !TODO pensar num tempo bom de previsão

    // encontra os obstáculos na trajetória; i+1 pois o vetor de estados começa com a bola
    std::vector<Obstacle> obstacles;

    // se o alvo é a bola e seu x é maior, desvia da bola, senão, vai na bola mesmo
    if(target == predicted_state.objects.at(0) && robots.at(robot_index).position.x < target.x)
        obstacles = find_obstacles(predicted_state, predicted_state.objects.at(robot_index + 1), target, 1);
    else
        obstacles = find_obstacles(predicted_state, predicted_state.objects.at(robot_index + 1), target, 0);

    // se há obstáculos
    if(obstacles.size() > 0) {
        //std::cout << "(" << robot_index + 1 << ") obstacle: " << obstacles[0].position << "\n";

        // tem tamanho 2, o robô pode desviar pra direita ou para esquerda
        cv::Point * deviation_points = find_deviation(robots.at(robot_index).position, target, obstacles.at(0));
        // verifica se o ponto de desvio é válido
        if(!validate_target(deviation_points[0])) {
            if(!validate_target(deviation_points[1])) {
                // desvios inválidos
                target = crop_target(deviation_points[0]);
            }
            else {
                // segundo desvio é válido
                target = deviation_points[1];
            }
        }
        else {
            // melhor desvio é válido
            target = deviation_points[0];
        }

        //std::cout << "(" << robot_index + 1 << ") new target: " << target << "\n\n";

        robots.at(robot_index).target = target;
    } // se obstáculos > 0

    // atualiza o vetor pra estratégia
    *pRobots = robots;
}

void Planner::update_planner(std::vector<Robot> robots, cv::Point * advRobots, cv::Point ball, bool use_this) {
    this->use_this = use_this;

    // gera um estado ordenado e organizado
    State current_state = gen_state(robots, advRobots, ball);

    // atualiza o histórico para fazer as predições
    update_hist(current_state);
}

cv::Point Planner::best_shot_target(int robot_index) {
    cv::Point target;

    State predicted_state = predict_positions(0); // !TODO pensar num tempo bom de previsão

    target.x = COORD_GOAL_ATK_FRONT_X;
    target.y = predicted_state.objects.at(robot_index + 1).y > COORD_GOAL_MID_Y ? COORD_BOX_UP_Y : COORD_BOX_DWN_Y;

    // encontra os obstáculos na trajetória; i+1 pois o vetor de estados começa com a bola
    std::vector<Obstacle> obstacles;

    // encontra obstáculos na trajetória ignorando a bola
    obstacles = find_obstacles(predicted_state, predicted_state.objects.at(robot_index + 1), target, 1);

    // se há obstáculos
    if(obstacles.size() > 0) {
        // tem tamanho 2, o robô pode desviar pra direita ou para esquerda
        cv::Point * deviation_points = find_deviation(robots.at(robot_index).position, target, obstacles.at(0));
        // verifica se o ponto de desvio é válido
        if(!validate_shot_target(deviation_points[0])) {
            if(!validate_shot_target(deviation_points[1])) {
                // desvios inválidos
                target = cv::Point(COORD_GOAL_ATK_FRONT_X, COORD_GOAL_MID_Y);
            }
            else {
                // segundo desvio é válido
                target.y = deviation_points[1].y;
            }
        }
        else {
            // melhor desvio é válido
            target.y = deviation_points[0].y;
        }

    }

    return target;
}

// dá um crop no alvo caso não tenha outra possibilidade
cv::Point Planner::crop_target(cv::Point target) {
    if(target.y < 0) target.y = 0;
    if(target.y > ABS_FIELD_HEIGHT) target.y = ABS_FIELD_HEIGHT;
    if(target.x < 0) target.x = 0;
    if(target.x > COORD_GOAL_ATK_FRONT_X) target.x = COORD_GOAL_ATK_FRONT_X;

    if (target.x < COORD_BOX_DEF_X && target.y > COORD_BOX_UP_Y && target.y < COORD_GOAL_DWN_Y) {
        target.x = def_corner_line;
    }
}

// verifica se alvo está dentro do gol
bool Planner::validate_shot_target(cv::Point target) {
    return target.y > COORD_BOX_UP_Y + ABS_ROBOT_SIZE/2 && target.y < COORD_BOX_DWN_Y - ABS_ROBOT_SIZE/2;
}

// verifica se alvo está dentro do campo
bool Planner::validate_target(cv::Point target) {
    return target.x > ROBOT_RADIUS && target.x < width - ROBOT_RADIUS && target.y > ROBOT_RADIUS && target.y < height - ROBOT_RADIUS;
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
    State ret_state;
    if(hist.size() > 0)
        return hist.back();
    else
        return ret_state;
}

double Planner::distance(cv::Point p1, cv::Point p2) {
    return sqrt(pow(p1.x-p2.x,2) + pow(p1.y-p2.y,2));
}
// calcula a distância de um ponto pra uma linha, transformando pra origem e rotacionando de forma que D seja equivalente ao Y do ponto transformado
double Planner::distance_to_line(cv::Point start, cv::Point end, cv::Point point) {
    double theta;

    // translada pra origem
    end -= start;
    point -= start;
    start -= start;

    // encontra a inclinação do eixo
    theta = atan2(end.y, end.x);

    // rotaciona o ponto
    point = cv::Point(point.x*cos(-theta) - point.y*sin(-theta), point.x*sin(-theta) + point.y*cos(-theta));

    // como o ponto foi rotacionado em torno da origem em direção ao eixo x onde end.y = 0, a altura do ponto corresponde à sua distancia
    return point.y;
}

// encontra obstáculos entre dois pontos e retorna um vetor ordenado
// os obstáculos mais próximos do ponto de início vêm primeiro
std::vector<Planner::Obstacle> Planner::find_obstacles(State predicted_state, cv::Point startPos, cv::Point target, int start_index) {
    std::vector<Obstacle> obstacles;
    double tempDist;

    // para cada objeto na pista, verifica se este é um obstáculo
    for(int i = start_index; i < predicted_state.objects.size(); i++) {
        if(predicted_state.objects.at(i) == startPos)
            continue;

        // verifica se o obstáculo está perto suficiente da reta
        if(abs(tempDist = distance_to_line(startPos, target, predicted_state.objects.at(i))) < 2 * ROBOT_RADIUS) {
            // se tá perto, verifica ainda se tá realmente na frente
            if(distance(startPos, target) < distance(predicted_state.objects.at(i), target) || distance(startPos, target) < distance(startPos, predicted_state.objects.at(i))) {
                continue;
            }
            //std::cout << "From " << startPos << " to " << target << " there is " << predicted_state.objects.at(i) << " at a " << tempDist << " pixels distance\n";
            Obstacle obstacle;
            obstacle.position = predicted_state.objects.at(i);
            obstacle.distance = abs(tempDist);
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
    double angle = -atan2(double(end.y - start.y), - double(end.x - start.x));

    angle += PI/2;

    // gera uma reta a partir do obstacle.position
    deviations[0] = cv::Point(obstacle.position.x + DEVIATION_DELTA*ROBOT_RADIUS*cos(angle), obstacle.position.y + DEVIATION_DELTA*ROBOT_RADIUS*sin(angle));
    deviations[1] = cv::Point(obstacle.position.x - DEVIATION_DELTA*ROBOT_RADIUS*cos(angle), obstacle.position.y - DEVIATION_DELTA*ROBOT_RADIUS*sin(angle));

    // ordena pro desvio mais próximo
    if(abs(distance_to_line(start, end, deviations[0])) > abs(distance_to_line(start, end, deviations[1]))) {
        cv::Point aux = deviations[0];
        deviations[0] = deviations[1];
        deviations[1] = aux;
    }

    return deviations;
}

// encontra desvios no chute pro gol
cv::Point * Planner::find_kick_deviation(cv::Point start, cv::Point end, Planner::Obstacle obstacle) {
    cv::Point * deviations = new cv::Point[2];

    // encontra o angulo entre start e end, soma 90 pra encontrar o angulo da perpendicular
    double angle = -atan2(double(end.y - start.y), - double(end.x - start.x));

    angle += PI/2;

    // gera uma reta a partir do obstacle.position
    deviations[0] = cv::Point(obstacle.position.x + ABS_ROBOT_SIZE*cos(angle), obstacle.position.y + ABS_ROBOT_SIZE*sin(angle));
    deviations[1] = cv::Point(obstacle.position.x - ABS_ROBOT_SIZE*cos(angle), obstacle.position.y - ABS_ROBOT_SIZE*sin(angle));

    // ordena pro desvio mais distante pois é mais difícil do goleiro defender
    if(abs(distance_to_line(start, end, deviations[0])) < abs(distance_to_line(start, end, deviations[1]))) {
        cv::Point aux = deviations[0];
        deviations[0] = deviations[1];
        deviations[1] = aux;
    }

    return deviations;
}