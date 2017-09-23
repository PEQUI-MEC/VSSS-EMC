#ifndef TAG_HPP_
#define TAG_HPP_

#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

class Tag {
public:
    cv::Point position;
    cv::Vec4f line;
    cv::Point frontPoint;
    cv::Point rearPoint;

    // armazena se esta é uma tag secundária da esquerda
    bool left;

    double area;

    // eu tentei implementar num cpp mas deu muita merda
    /// <summary>
    /// Constroi a tag
    /// </summary>
    /// <param name="pos">Posição da tag</param>
    /// <param name="myarea">Corresponde ao tagArea</param>
    Tag(cv::Point pos, double myarea) {
        position.x = pos.x;
        position.y = pos.y;
        area = myarea;
    }

    /// <summary>
    /// Seta os pontos frontPoint e rearPoint aleatoriamente
    /// </summary>
    /// <param name="myLine">Linha encontrada pelo fitline</param>
    void setLine(cv::Vec4f myLine) {
        float vx = myLine[0];
        float vy = myLine[1];
        float x = myLine[2];
        float y = myLine[3];

        line = myLine;

        frontPoint = cv::Point((int)(x + vx*15), (int)(y + vy*15));
        rearPoint = cv::Point((int)(x - vx*15), (int)(y - vy*15));
    }

    /// <summary>
    /// Inverte os pontos frontPoint e rearPoint depois que a visão determinar a ordem correta
    /// </summary>
    void switchPoints() {
        cv::Point temp;
        temp = rearPoint;
        rearPoint = frontPoint;
        frontPoint = temp;
    }

};

#endif /* TAG_HPP_ */
