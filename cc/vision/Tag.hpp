#ifndef TAG_HPP_
#define TAG_HPP_

#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

#define ROBOT_RADIUS 20 // valor encontrado empiricamente (quanto menor, mais próximos os robôs podem ficar sem dar errado)

class Tag {
	public:
	cv::Point_<double> position = {ROBOT_RADIUS/2.0, 0.0};

    // Usado apenas para desenhar a linha
    // TODO:: Remover a dependência de front_point e rear_point
	cv::Point_<double> front_point = {ROBOT_RADIUS, 0};
	cv::Point_<double> rear_point = {0, 0};

	double orientation = 0;

	double area = 0;

	int id = -1;

	// TODO Alterar o tipo de int para enum Color.
	// No momento seria uma mudança grande já que haveria a necessidade de
	// mover vision::Color e vision::Vision::Color_Name para Tag.hpp
	// TODO Alterar o Constructor para incluir a variável color.
	// Atualmente ela não é confiável, já que só é inicializada na função Vision::pick_a_tag()
	int color = -1;
    bool already_assigned_to_robot = false;

	Tag() = default;

	/// Constroi a tag
	/// pos: Posição da tag
	/// myarea: Corresponde ao tagArea
	Tag(cv::Point_<double> pos, double myarea);

	/// Seta os pontos frontPoint e rearPoint aleatoriamente
	/// myLine: Linha encontrada pelo fitline</param>
	void setLine(cv::Vec4f myLine);

	/// Inverte os pontos frontPoint e rearPoint depois que a visão determinar a ordem correta
	void switchPoints();
};

#endif /* TAG_HPP_ */
