#ifndef TAG_HPP_
#define TAG_HPP_

#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

#define ROBOT_RADIUS 15 // valor encontrado empiricamente (quanto menor, mais próximos os robôs podem ficar sem dar errado)

class Tag {
	public:
	cv::Point position = {ROBOT_RADIUS/2, 0};
	cv::Point front_point = {ROBOT_RADIUS, 0};
	cv::Point rear_point = {0, 0};
	double orientation = 0;

	double area = 0;

	bool left = false;

	Tag() = default;

	/// Constroi a tag
	/// pos: Posição da tag
	/// myarea: Corresponde ao tagArea
	Tag(cv::Point pos, double myarea);

	/// Seta os pontos frontPoint e rearPoint aleatoriamente
	/// myLine: Linha encontrada pelo fitline</param>
	void setLine(cv::Vec4f myLine);

	/// Inverte os pontos frontPoint e rearPoint depois que a visão determinar a ordem correta
	void switchPoints();
};

#endif /* TAG_HPP_ */
