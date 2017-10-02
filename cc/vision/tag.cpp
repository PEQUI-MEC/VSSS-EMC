#include "tag.hpp"

Tag::Tag(cv::Point pos, double myarea) {
    Tag::position.x = pos.x;
    Tag::position.y = pos.y;
    Tag::area = myarea;
}

/// <summary>
/// Seta os pontos frontPoint e rearPoint aleatoriamente
/// </summary>
/// <param name="myLine">Linha encontrada pelo fitline</param>
void Tag::setLine(cv::Vec4f myLine) {
    float vx = myLine[0];
    float vy = myLine[1];
    float x = myLine[2];
    float y = myLine[3];
    Tag::frontPoint = cv::Point((int)(x + vx*ROBOT_RADIUS), (int)(y + vy*ROBOT_RADIUS));
    Tag::rearPoint = cv::Point((int)(x - vx*ROBOT_RADIUS), (int)(y - vy*ROBOT_RADIUS));
}

/// <summary>
/// Inverte os pontos frontPoint e rearPoint depois que a visão determinar a ordem correta
/// </summary>
void Tag::switchPoints() {
    cv::Point temp;
    temp = Tag::rearPoint;
    Tag::rearPoint = Tag::frontPoint;
    Tag::frontPoint = temp;
}
