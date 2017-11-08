#include "vision.hpp"

void Vision::run(cv::Mat raw_frame) {
  in_frame = raw_frame.clone();

  if (bOnAir) recordToVideo();

  preProcessing();
  findTags();
  //findElements();
  pick_a_tag();
}

void Vision::runGMM(std::vector<cv::Mat> thresholds) {
  searchGMMTags(thresholds);
  pick_a_tag();
}

void Vision::preProcessing() {
  cv::cvtColor(in_frame,hsv_frame,cv::COLOR_RGB2HSV);
}

void Vision::findTags() {
  for (int i = 0; i < TOTAL_COLORS; i++) {
    threshold_threads.add_thread(new boost::thread(&Vision::segmentAndSearch, this, i));
  }
  threshold_threads.join_all();
}

void Vision::segmentAndSearch(int color) {
  cv::Mat frame = hsv_frame.clone();

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

void Vision::searchGMMTags(std::vector<cv::Mat> thresholds) {
  std::vector< std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  for (int color = 0; color < TOTAL_COLORS; color++) {
    tags.at(color).clear();

    cv::Mat tmp;
    cv::cvtColor(thresholds.at(color), tmp, CV_RGB2GRAY);

    cv::findContours(tmp,contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_NONE);

    for (int i = 0; i < contours.size(); i++) {
      double area = contourArea(contours[i]);
      // if(area >= areaMin[color]) {
        cv::Moments moment = moments((cv::Mat)contours[i]);
        tags.at(color).push_back(Tag(cv::Point(moment.m10/area, moment.m01/area), area));

        // seta as linhas para as tags principais do pick-a-tag
        if(color == MAIN) {
            cv::Vec4f line;
            cv::fitLine(cv::Mat(contours[i]),line,2,0,0.01,0.01);
            int tagsInVec = tags.at(color).size() - 1;
            tags.at(color).at(tagsInVec).setLine(line);
        }
      // }
    }
  }
}

void Vision::searchTags(int color) {
  std::vector< std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  tags.at(color).clear();

  cv::findContours(threshold_frame.at(color),contours,hierarchy,cv::RETR_CCOMP,cv::CHAIN_APPROX_NONE);

  for (int i = 0; i < contours.size(); i++) {
    double area = contourArea(contours[i]);
    if(area >= areaMin[color]) {
      cv::Moments moment = moments((cv::Mat)contours[i]);

      // seta as linhas para as tags principais do pick-a-tag
      if(color == MAIN) {
          tags.at(color).push_back(Tag(cv::Point(moment.m10/area, moment.m01/area), area));

          // tem que ter jogado a tag no vetor antes de mexer nos valores dela
          cv::Vec4f line;
          cv::fitLine(cv::Mat(contours[i]),line,2,0,0.01,0.01);
          int tagsInVec = tags.at(color).size() - 1;
          tags.at(color).at(tagsInVec).setLine(line);
      } else if(color == ADV) {
          if(tags.at(color).size() >= 3) {
              // pega o menor índice
              int menor = 0;
              for(int j = 1; j < 3; j++) {
                  if(tags.at(color).at(j).area < tags.at(color).at(menor).area)
                    menor = j;
              }
              if(menor < tags.at(color).size() && area > tags.at(color).at(menor).area)
                tags.at(color).at(menor) = Tag(cv::Point(moment.m10/area, moment.m01/area), area);
          }
          else {
              tags.at(color).push_back(Tag(cv::Point(moment.m10/area, moment.m01/area), area));
          }
      } else {
          tags.at(color).push_back(Tag(cv::Point(moment.m10/area, moment.m01/area), area));
      }
    }
  }
}

// void Vision::findElements() {
//   int dist;
//   double idAngle;
//
//
//   // OUR ROBOTS
//   for (int i = 0; i < tags.at(MAIN).size() && i<3; i++) {
//     Robot robot;
//     int minDistRef[2] = {9999,9999};
//     int minDistIndex1[2] = {-1,-1};
//     int minDistIndex2[2] = {-1,-1};
//
//     // Para cada tag principal, verifica quais são as secundárias correspondentes
//     for(int j = GREEN; j <= PINK; j++) {
//       for(int k = 0; k < tags.at(j).size(); k++) {
//
//         int distance = calcDistance(tags.at(MAIN).at(i).position,tags.at(j).at(k).position);
//
//         if(distance <= minDistRef[0]) {
//           minDistRef[1] = minDistRef[0];
//           minDistIndex2[0] = minDistIndex1[0];
//           minDistIndex2[1] = minDistIndex1[1];
//
//           minDistRef[0] = distance;
//           minDistIndex1[0] = j;
//           minDistIndex1[1] = k;
//           if(j==PINK)
//           robot.isOdd=true;
//
//         } else if(distance < minDistRef[1]) {
//           minDistRef[1] = distance;
//           minDistIndex2[0] = j;
//           minDistIndex2[1] = k;
//         }
//       }
//     }
//
//     // Posição do robô
//     robot.position = tags.at(MAIN).at(i).position;
//
//     // Secundárias do robô
//     try {
//       if (tags.at(minDistIndex1[0]).at(minDistIndex1[1]).area < tags.at(minDistIndex2[0]).at(minDistIndex2[1]).area) {
//         robot.secundary = tags.at(minDistIndex2[0]).at(minDistIndex2[1]).position;
//         robot.ternary = tags.at(minDistIndex1[0]).at(minDistIndex1[1]).position;
//       } else {
//         robot.secundary = tags.at(minDistIndex1[0]).at(minDistIndex1[1]).position;
//         robot.ternary = tags.at(minDistIndex2[0]).at(minDistIndex2[1]).position;
//       }
//     }
//     catch (const std::out_of_range& oor) {
//       continue; // Não calibrado
//     }
//
//     // Cálculo da orientação das tags secundária e ternária em relação a tag principal
//     robot.orientation = atan2((robot.secundary.y-robot.position.y)*1.3/height,(robot.secundary.x-robot.position.x)*1.5/width);
//     robot.orientation2 = atan2((robot.ternary.y-robot.position.y)*1.3/height,(robot.ternary.x-robot.position.x)*1.5/width);
//
//     // Cálculo do ângulo de orientação para diferenciar robôs de mesma cor
//     idAngle = atan2(sin(robot.orientation2-robot.orientation+3.1415),
//     cos(robot.orientation2-robot.orientation+3.1415));
//
//     // Dá nome aos bois (robôs)
//     if(robot.isOdd){
//       robot_list.at(2).position = robot.position; // colocar em um vetor
//       robot_list.at(2).secundary = robot.secundary; // colocar em um vetor
//       robot_list.at(2).orientation =  robot.orientation;
//     }else  if(idAngle>0) {
//       robot_list.at(0).position = robot.position; // colocar em um vetor
//       robot_list.at(0).secundary = robot.secundary; // colocar em um vetor
//       robot_list.at(0).orientation =  robot.orientation;
//     }else {
//       robot_list.at(1).position = robot.position; // colocar em um vetor
//       robot_list.at(1).secundary = robot.secundary; // colocar em um vetor
//       robot_list.at(1).orientation =  robot.orientation;
//     }
//   }
//
//   // ADV ROBOTS
//   for (int i = 0; i < tags.at(ADV).size() && i < MAX_ADV; i++) {
//     advRobots[i] = tags.at(ADV).at(i).position;
//   }
//
//   // BALL POSITION
//   if (!tags.at(BALL).empty())
//     ball = tags.at(BALL).at(0).position;
// }


/// <summary>
/// Seleciona um conjunto de tags para representar cada robô
/// </summary>
/// <description>
/// P.S.: Aqui eu uso a flag 'isOdd' para representar quando um robô tem as duas bolas laterais.
/// </description>
void Vision::pick_a_tag() {
    int dist, tmpSide;

    // OUR ROBOTS
    for (int i = 0; i < tags.at(MAIN).size() && i<3; i++) {
        // cria um robô temporário para armazenar nossas descobertas
        Robot robot;
        std::vector<Tag> tempTags;

        // Posição do robô
        robot.position = tags.at(MAIN).at(i).position;

        // Cálculo da orientação de acordo com os pontos rear e front
        robot.orientation = atan2((tags.at(MAIN).at(i).frontPoint.y-robot.position.y)*1.3/height,(tags.at(MAIN).at(i).frontPoint.x-robot.position.x)*1.5/width);

        // Armazena a tag
        tempTags.push_back(tags.at(MAIN).at(i));

        // Para cada tag principal, verifica quais são as secundárias correspondentes
        for(int j = 0; j < tags.at(GREEN).size(); j++) {
            // já faz a atribuição verificando se o valor retornado é 0 (falso); além disso, altera a orientação caso esteja errada
            if(tmpSide = inSphere(&robot, &tempTags, tags.at(GREEN).at(j).position)) {
                // identifica se já tem mais de uma tag
                if(tempTags.size() > 1) {
                    robot.isOdd = true;
                }
                tags.at(GREEN).at(j).left = (tmpSide > 0) ? true : false;
                // calculos feitos, joga tag no vetor
                tempTags.push_back(tags.at(GREEN).at(j));
            }
        }


        // Dá nome aos bois (robôs)
        if(robot.isOdd){ // isOdd representa que este tem as duas bolas
            robot_list.at(2).position = robot.position; // colocar em um vetor
            robot_list.at(2).secundary = tempTags.at(0).frontPoint; // colocar em um vetor
            robot_list.at(2).orientation =  robot.orientation;
            robot_list.at(2).rearPoint = tempTags.at(0).rearPoint;
        } else if(tempTags.size() > 1 && tempTags.at(1).left) {
            robot_list.at(0).position = robot.position; // colocar em um vetor
            robot_list.at(0).secundary = tempTags.at(0).frontPoint; // colocar em um vetor
            robot_list.at(0).orientation = robot.orientation;
            robot_list.at(0).rearPoint = tempTags.at(0).rearPoint;
        } else {
            robot_list.at(1).position = robot.position; // colocar em um vetor
            robot_list.at(1).secundary = tempTags.at(0).frontPoint; // colocar em um vetor
            robot_list.at(1).orientation =  robot.orientation;
            robot_list.at(1).rearPoint = tempTags.at(0).rearPoint;
        }
    } // OUR ROBOTS

    // ADV ROBOTS
    for (int i = 0; i < MAX_ADV; i++) {
        if(i < tags.at(ADV).size())
            advRobots[i] = tags.at(ADV).at(i).position;
        else
            advRobots[i] = cv::Point(-1, -1);
    }

    // BALL POSITION
    if (!tags[BALL].empty())
        ball = tags.at(BALL).at(0).position;
}

/// <summary>
/// Verifica se uma tag secundária pertence a esta pick-a e calcula seu delta.
/// </summary>
/// <param name="position">Posição central do robô</param>
/// <param name="secondary">O suposto ponto que marca uma bola da tag</param>
/// <param name="orientation">A orientação do robô</param>
/// <returns>
/// 0, se esta não é uma tag secundária;
/// -1, caso a secundária esteja à esquerda;
/// 1, caso a secundária esteja à direita
/// </returns>
int Vision::inSphere(Robot * robot, std::vector<Tag> * tempTags, cv::Point secondary) {
    // se esta secundária faz parte do robô
    if(calcDistance(robot->position, secondary) <= ROBOT_RADIUS) {
        if(calcDistance(tempTags->at(0).frontPoint, secondary) < calcDistance(tempTags->at(0).rearPoint, secondary)) {
            tempTags->at(0).switchPoints();
            // recalcula a orientação com os novos pontos (isso só é feito uma vez em cada robô, se necessário)
            robot->orientation = atan2((tempTags->at(0).frontPoint.y-robot->position.y)*1.3/height,(tempTags->at(0).frontPoint.x-robot->position.x)*1.5/width);
        }

        float secSide = atan2((secondary.y-robot->position.y)*1.3/height,(secondary.x-robot->position.x)*1.5/width);

        // Cálculo do ângulo de orientação para diferenciar robôs de mesma cor
        return (atan2(sin(secSide-robot->orientation+3.1415), cos(secSide-robot->orientation+3.1415))) > 0 ? 1 : -1;
    }
    return 0;
}

double Vision::calcDistance(cv::Point p1, cv::Point p2) {
  return sqrt(pow(p1.x-p2.x,2) + pow(p1.y-p2.y,2));
}

void Vision::switchMainWithAdv() {
  int tmp;

  for (int i = MIN; i <= MAX; i++) {
    tmp = hue[MAIN][i];
    hue[MAIN][i] = hue[ADV][i];
    hue[ADV][i] = tmp;

    tmp = saturation[MAIN][i];
    saturation[MAIN][i] = saturation[ADV][i];
    saturation[ADV][i] = tmp;

    tmp = value[MAIN][i];
    value[MAIN][i] = value[ADV][i];
    value[ADV][i] = tmp;
  }

  tmp = areaMin[MAIN];
  areaMin[MAIN] = areaMin[ADV];
  areaMin[ADV] = tmp;

  tmp = erode[MAIN];
  erode[MAIN] = erode[ADV];
  erode[ADV] = tmp;

  tmp = dilate[MAIN];
  dilate[MAIN] = dilate[ADV];
  dilate[ADV] = tmp;

  tmp = blur[MAIN];
  blur[MAIN] = blur[ADV];
  blur[ADV] = tmp;
}

cv::Mat Vision::getSplitFrame() {
  cv::Mat horizontal[2], vertical[2];

  for (int index = 0; index < 3; index++) {
    cv::cvtColor(threshold_frame.at(index), threshold_frame.at(index), cv::COLOR_GRAY2RGB);
  }

  cv::pyrDown(in_frame, vertical[0]);
  cv::pyrDown(threshold_frame.at(0), vertical[1]);

  cv::hconcat(vertical, 2, horizontal[0]);

  cv::pyrDown(threshold_frame.at(1), vertical[0]);
  cv::pyrDown(threshold_frame.at(2), vertical[1]);

  cv::hconcat(vertical, 2, horizontal[1]);

  cv::vconcat(horizontal, 2, splitFrame);

  // cv::imwrite("teste.png", splitFrame);

  return splitFrame;
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

void Vision::savePicture(std::string in_name) {
  cv::Mat frame = in_frame.clone();
  std::string picName = "media/pictures/" + in_name + ".png";

  cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
  cv::imwrite(picName, frame);
}

void Vision::startNewVideo(std::string in_name) {
  std::string videoName = "media/videos/" + in_name + ".avi";

  video.open(videoName, CV_FOURCC('M','J','P','G'), 30, cv::Size(width,height));
  std::cout << "Started a new video recording." << std::endl;
  bOnAir = true;
}

bool Vision::recordToVideo() {
  if (!video.isOpened()) return false;
  cv::Mat frame = in_frame.clone();
  cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);

  video.write(frame);
  return true;
}

bool Vision::finishVideo() {
  if (!video.isOpened()) return false;

  std::cout << "Finished video recording." << std::endl;
  video.release();
  bOnAir = false;
  return true;
}

bool Vision::isRecording() {
  return bOnAir;
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

int Vision::getHue(int index0, int index1) {
  return hue[index0][index1];
}

int Vision::getSaturation(int index0, int index1) {
  return saturation[index0][index1];
}

int Vision::getValue(int index0, int index1) {
  return value[index0][index1];
}

int Vision::getErode(int index) {
  return erode[index];
}

int Vision::getDilate(int index) {
  return dilate[index];
}

int Vision::getBlur(int index) {
  return blur[index];
}

int Vision::getAmin(int index) {
  return areaMin[index];
}

void Vision::setHue(int index0, int index1, int inValue) {
  if (index0 >= 0 && index0 < TOTAL_COLORS && (index1 == 0 || index1 == 1)) hue[index0][index1] = inValue;
  else std::cout << "Vision:setHue: could not set (invalid index)" << std::endl;
}

void Vision::setSaturation(int index0, int index1, int inValue) {
  if (index0 >= 0 && index0 < TOTAL_COLORS && (index1 == 0 || index1 == 1)) saturation[index0][index1] = inValue;
  else std::cout << "Vision:setSaturation: could not set (invalid index)" << std::endl;
}

void Vision::setValue(int index0, int index1, int inValue) {
  if (index0 >= 0 && index0 < TOTAL_COLORS && (index1 == 0 || index1 == 1)) value[index0][index1] = inValue;
  else std::cout << "Vision:setValue: could not set (invalid index)" << std::endl;
}

void Vision::setErode(int index, int inValue) {
  if (index >= 0 && index < TOTAL_COLORS) erode[index] = inValue;
  else std::cout << "Vision:setErode: could not set (invalid index)" << std::endl;
}

void Vision::setDilate(int index, int inValue) {
  if (index >= 0 && index < TOTAL_COLORS) dilate[index] = inValue;
  else std::cout << "Vision:setDilate: could not set (invalid index)" << std::endl;
}

void Vision::setBlur(int index, int inValue) {
  if (index >= 0 && index < TOTAL_COLORS) blur[index] = inValue;
  else std::cout << "Vision:setBlur: could not set (invalid index)" << std::endl;
}

void Vision::setAmin(int index, int inValue) {
  if (index >= 0 && index < TOTAL_COLORS) areaMin[index] = inValue;
  else std::cout << "Vision:setAmin: could not set (invalid index)" << std::endl;
}

void Vision::setFrameSize(int inWidth, int inHeight) {
  if (inWidth >= 0) width = inWidth;
  if (inHeight >= 0) height = inHeight;
}

int Vision::getFrameHeight() {
  return height;
}

int Vision::getFrameWidth() {
  return width;
}

cv::Point* Vision::getAllAdvRobots() {
  return advRobots;
}


Vision::Vision(int w, int h) : width(w), height(h), bOnAir(false)
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
}

Vision::~Vision()
{

}
