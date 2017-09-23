#include "vision.hpp"

#define R_WIDTH 15 // em pixels
#define R_HEIGHT 15 // em pixels

void Vision::run(cv::Mat raw_frame) {
  in_frame = raw_frame.clone();
  preProcessing();
  findTags();
  findElements();
  //pick_a_tag();
}

void Vision::preProcessing() {
  cv::cvtColor(in_frame,in_frame,cv::COLOR_RGB2HSV);
}

void Vision::findTags() {
  for (int i = 0; i < TOTAL_COLORS; i++) {
    threshold_threads.add_thread(new boost::thread(&Vision::segmentAndSearch, this, i));
  }
  threshold_threads.join_all();
}

void Vision::segmentAndSearch(int color) {
  cv::Mat frame = in_frame.clone();

  inRange(frame,cv::Scalar(hue[color][MIN],saturation[color][MIN],value[color][MIN]),
  cv::Scalar(hue[color][MAX],saturation[color][MAX],value[color][MAX]),threshold_frame.at(color));

  posProcessing(color);
  searchTags(color);
}

void Vision::posProcessing(int color) {
  cv::Mat erodeElement = cv::getStructuringElement( cv::MORPH_RECT,cv::Size(3,3));
  cv::Mat dilateElement = cv::getStructuringElement( cv::MORPH_RECT,cv::Size(3,3));

  cv::medianBlur(threshold_frame.at(color), threshold_frame.at(color), blur[color]);
  cv::erode(threshold_frame.at(color),threshold_frame.at(color),erodeElement,cv::Point(-1,-1),erode[color]);
  cv::dilate(threshold_frame.at(color),threshold_frame.at(color),dilateElement,cv::Point(-1,-1),dilate[color]);
}

void Vision::searchTags(int color) {
  std::vector< std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  tags.at(color).clear();

  cv::findContours(threshold_frame.at(color),contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_SIMPLE);

  for (int i = 0; i < contours.size(); i++) {
    double area = contourArea(contours[i]);
    if(area >= areaMin[color]/100) {
      cv::Moments moment = moments((cv::Mat)contours[i]);
      tags.at(color).push_back(Tag(cv::Point(moment.m10/area, moment.m01/area), area));

      // seta as linhas para as tags principais do pick-a-tag
      if(color == MAIN) {
          cv::Vec4f line;
          cv::fitLine(cv::Mat(contours[i]),line,2,0,0.01,0.01);
          int tagsInVec = tags.at(color).size() - 1;
          tags.at(color).at(tagsInVec).setLine(line);
      }
    }
  }
}

void Vision::findElements() {
  int dist;
  double idAngle;


  // OUR ROBOTS
  for (int i = 0; i < tags.at(MAIN).size() && i<3; i++) {
    Robot robot;
    int minDistRef[2] = {9999,9999};
    int minDistIndex1[2] = {-1,-1};
    int minDistIndex2[2] = {-1,-1};

    // Para cada tag principal, verifica quais são as secundárias correspondentes
    for(int j = GREEN; j <= PINK; j++) {
      for(int k = 0; k < tags.at(j).size(); k++) {

        int distance = calcDistance(tags.at(MAIN).at(i).position,tags.at(j).at(k).position);

        if(distance <= minDistRef[0]) {
          minDistRef[1] = minDistRef[0];
          minDistIndex2[0] = minDistIndex1[0];
          minDistIndex2[1] = minDistIndex1[1];

          minDistRef[0] = distance;
          minDistIndex1[0] = j;
          minDistIndex1[1] = k;
          if(j==PINK)
          robot.pink=true;

        } else if(distance < minDistRef[1]) {
          minDistRef[1] = distance;
          minDistIndex2[0] = j;
          minDistIndex2[1] = k;
        }
      }
    }

    // Posição do robô
    robot.position = tags.at(MAIN).at(i).position;

    // Secundárias do robô
    try {
      if (tags.at(minDistIndex1[0]).at(minDistIndex1[1]).area < tags.at(minDistIndex2[0]).at(minDistIndex2[1]).area) {
        robot.secundary = tags.at(minDistIndex2[0]).at(minDistIndex2[1]).position;
        robot.ternary = tags.at(minDistIndex1[0]).at(minDistIndex1[1]).position;
      } else {
        robot.secundary = tags.at(minDistIndex1[0]).at(minDistIndex1[1]).position;
        robot.ternary = tags.at(minDistIndex2[0]).at(minDistIndex2[1]).position;
      }
    }
    catch (const std::out_of_range& oor) {
      continue; // Não calibrado
    }

    // Cálculo da orientação das tags secundária e ternária em relação a tag principal
    robot.orientation = atan2((robot.secundary.y-robot.position.y)*1.3/height,(robot.secundary.x-robot.position.x)*1.5/width);
    robot.orientation2 = atan2((robot.ternary.y-robot.position.y)*1.3/height,(robot.ternary.x-robot.position.x)*1.5/width);

    // Cálculo do ângulo de orientação para diferenciar robôs de mesma cor
    idAngle = atan2(sin(robot.orientation2-robot.orientation+3.1415),
    cos(robot.orientation2-robot.orientation+3.1415));

    // Dá nome aos bois (robôs)
    if(robot.pink){
      robot_list.at(2).position = robot.position; // colocar em um vetor
      robot_list.at(2).secundary = robot.secundary; // colocar em um vetor
      robot_list.at(2).orientation =  robot.orientation;
    }else  if(idAngle>0) {
      robot_list.at(0).position = robot.position; // colocar em um vetor
      robot_list.at(0).secundary = robot.secundary; // colocar em um vetor
      robot_list.at(0).orientation =  robot.orientation;
    }else {
      robot_list.at(1).position = robot.position; // colocar em um vetor
      robot_list.at(1).secundary = robot.secundary; // colocar em um vetor
      robot_list.at(1).orientation =  robot.orientation;
    }
  }

  // ADV ROBOTS
  for (int i = 0; i < tags.at(ADV).size() && i < MAX_ADV; i++) {
    advRobots[i] = tags.at(ADV).at(i).position;
  }

  // BALL POSITION
  if (!tags.at(BALL).empty())
    ball = tags.at(BALL).at(0).position;
}


/// <summary>
/// Seleciona um conjunto de tags para representar cada robô
/// </summary>
/// <description>
/// P.S.: Aqui eu uso a flag 'pink' para representar quando um robô tem as duas bolas laterais.
/// </description>
void Vision::pick_a_tag() {
    int dist, idAngle;

    std::cout << std::endl;

    // OUR ROBOTS
    for (int i = 0; i < tags.at(MAIN).size() && i<3; i++) {
        // cria um robô temporário para armazenar nossas descobertas
        Robot robot;

        // Posição do robô
        robot.position = tags.at(MAIN).at(i).position;

        std::cout << robot.position << std::endl;

        robot.tags.push_back(tags.at(MAIN).at(i));

        // Para cada tag principal, verifica quais são as secundárias correspondentes
        for(int j = 0; j < tags.at(GREEN).size(); j++) {
            // verifica se o rearPoint é realmente o rearPoint e a secundária atual é uma bola dessa tag
            // já faz a atribuição verificando se o valor retornado é 0 (falso)
            if(idAngle = isClose(tags.at(MAIN).at(i).rearPoint, tags.at(GREEN).at(j).position, tags.at(MAIN).at(i).line)) {
                // identifica se já tem mais de uma tag
                if(robot.tags.size() > 1) {
                    robot.pink = true;
                }
                // só guarda se essa tag tá à esquerda
                tags.at(GREEN).at(j).left = idAngle < 0 ? true : false;
                // calculos feitos, joga tag no vetor
                robot.tags.push_back(tags.at(GREEN).at(j));
            }
            else if(idAngle = isClose(tags.at(MAIN).at(i).frontPoint, tags.at(GREEN).at(j).position, tags.at(MAIN).at(i).line)) {
                // se tem uma tag perto da primária atual, então os pontos estão trocados
                // o switch só ocorrerá uma vez, pois isso tá dentro do else
                tags.at(MAIN).at(i).switchPoints();
                idAngle *= -1; // se inverteu os pontos, a direção também inverteu

                // não é possível já ter tag aqui. ao encontrar um ponto, rear e front são invertidos e ele entraria no primeiro if.

                // guarda se essa tag tá à esquerda
                tags.at(GREEN).at(j).left = idAngle < 0 ? true : false;
                // calculos feitos, joga tag no vetor
                robot.tags.push_back(tags.at(GREEN).at(j));
            }
        }

        // Cálculo da orientação de acordo com os pontos rear e front
        // Feito aqui pois caso rear e front estivessem trocados, já teriam sido trocados
        robot.orientation = atan2((tags.at(MAIN).at(i).frontPoint.y-tags.at(MAIN).at(i).rearPoint.y)*1.3/height,(tags.at(MAIN).at(i).frontPoint.x-tags.at(MAIN).at(i).rearPoint.x)*1.5/width);

        // Dá nome aos bois (robôs)
        if(robot.pink){ // pink representa que este tem as duas bolas
            robot_list.at(2).position = robot.position; // colocar em um vetor
            robot_list.at(2).secundary = tags.at(MAIN).at(i).frontPoint; // colocar em um vetor
            robot_list.at(2).orientation =  robot.orientation;

            robot_list.at(2).tags = robot.tags;
        } else if(idAngle>0) {
            robot_list.at(0).position = robot.position; // colocar em um vetor
            robot_list.at(0).secundary = tags.at(MAIN).at(i).frontPoint; // colocar em um vetor
            robot_list.at(0).orientation = robot.orientation;

            robot_list.at(0).tags = robot.tags;
        } else {
            robot_list.at(1).position = robot.position; // colocar em um vetor
            robot_list.at(1).secundary = tags.at(MAIN).at(i).frontPoint; // colocar em um vetor
            robot_list.at(1).orientation =  robot.orientation;

            robot_list.at(1).tags = robot.tags;
        }

        robot_list.at(i).position = robot.position; // colocar em um vetor
    } // OUR ROBOTS

    // ADV ROBOTS
    for (int i = 0; i < tags.at(ADV).size() && i < MAX_ADV; i++) {
        advRobots[i] = tags.at(ADV).at(i).position;
    }

    // BALL POSITION
    if (!tags[BALL].empty())
        ball = tags.at(BALL).at(0).position;
}

/// <summary>
/// Verifica se uma tag secundária pertence a esta pick-a e calcula seu delta.
/// </summary>
/// <param name="base">O suposto ponto que marca a traseira do robô</param>
/// <param name="secondary">O suposto ponto que marca uma bola da tag</param>
/// <param name="originalDirection">Direçao da linha entre as supostas bases</param>
/// <returns>
/// 0, se esta não é uma tag secundária;
/// -1, caso a secundária esteja à esquerda;
/// 1, caso a secundária esteja à direita
/// </returns>
float Vision::isClose(cv::Point base, cv::Point secondary, cv::Vec4f originalDirection) {
    float deltaX, deltaY;
    deltaX = base.y + R_WIDTH * originalDirection[1]; // !TODO não tenho certeza se é y mesmo. pode ser x.
    deltaY = R_HEIGHT / 4.0; // como o delta varia para cima e para baixo do Y da base, 2*delta = altura / 2

    //std::cout << base << ";" << secondary << ";" << deltaX << ";" << deltaY << std::endl;

    // verifica se secondary está dentro do quadrante da base
    if(secondary.x > base.x - deltaX && secondary.x < base.x + deltaX && secondary.y > base.y - deltaY && secondary.y < base.y + deltaY) {
        return (base.x * originalDirection[1] - secondary.x * originalDirection[1] > 0) ? 1 : -1;
    }
    return 0;
}

double Vision::calcDistance(cv::Point p1, cv::Point p2) {
  return sqrt(pow(p1.x-p2.x,2) + pow(p1.y-p2.y,2));
}

void Vision::setCalibParams(int H[5][2], int S[5][2], int V[5][2], int Amin[5], int E[5], int D[5], int B[5])
{
  for (int i = 0; i < 5; i++)
  {
    areaMin[i] = Amin[i];
    erode[i] = E[i];
    dilate[i] = D[i];
    blur[i] = B[i];
    for (int j = 0; j < 2; j++)
    {
      hue[i][j] = H[i][j];
      saturation[i][j] = S[i][j];
      value[i][j] = V[i][j];
    }
  }
}

cv::Point Vision::getBall() {
  return ball;
}

Robot Vision::getRobot(int index) {
  return robot_list.at(index);
}

cv::Point Vision::getRobotPos(int index) {
  return robot_list.at(index).position;
}

cv::Point Vision::getAdvRobot(int index) {
  if (index < 0 || index >= MAX_ADV) {
    std::cout << "Vision::getAdvRobot: index argument is invalid." << std::endl;
    return cv::Point(-1,-1);
  } else {
    return advRobots[index];
  }
}

int Vision::getRobotListSize() {
  return (int)robot_list.size();
}

int Vision::getAdvListSize() {
  return MAX_ADV;
}

cv::Mat Vision::getThreshold(int index) {
  cv::cvtColor(threshold_frame.at(index), threshold_frame.at(index), cv::COLOR_GRAY2RGB);
  return threshold_frame.at(index);
}

Vision::Vision(int w, int h)
{
  // Variables Init
  cv::Mat mat;
  std::vector<Tag> tagVec;
  Robot robot;

  for (int i = 0; i < TOTAL_COLORS; i++) {
    threshold_frame.push_back(mat);
    tags.push_back(tagVec);
  }

  robot_list.push_back(robot);
  robot_list.push_back(robot);
  robot_list.push_back(robot);

  width = w;
  height = h;
}

Vision::~Vision()
{

}
