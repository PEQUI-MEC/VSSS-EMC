#include "vision.hpp"

#ifdef CUDA_FOUND
void Vision::run(const cv::cuda::GpuMat &raw_frame) {
	in_frame = raw_frame.clone();

	if (bOnAir) recordToVideo();

	preProcessing();
	findTags();
	pick_a_tag();
}

void Vision::recordVideo(const cv::cuda::GpuMat &frame) {
	in_frame = frame.clone();
	if (bOnAir) recordToVideo();
}
#else
void Vision::run(cv::Mat raw_frame) {
	in_frame = raw_frame.clone();

	if (bOnAir) recordToVideo();

	preProcessing();
	findTags();
	pick_a_tag();
}

void Vision::recordVideo(cv::Mat frame) {
	in_frame = frame.clone();
	if (bOnAir) recordToVideo();
}
#endif

void Vision::runGMM(std::vector<cv::Mat> thresholds, std::vector<VisionROI> *windowsList) {
	searchGMMTags(std::move(thresholds));
	pick_a_tag(windowsList);
}

void Vision::preProcessing() {
	#ifdef CUDA_FOUND
	if (convertType == HSV) cv::cuda::cvtColor(in_frame, hsv_frame, cv::COLOR_RGB2HSV);
	else cv::cuda::cvtColor(in_frame, hsv_frame, cv::COLOR_RGB2Lab);
	#else
	if (convertType == HSV) cv::cvtColor(in_frame, hsv_frame, cv::COLOR_RGB2HSV);
	else cv::cvtColor(in_frame, hsv_frame, cv::COLOR_RGB2Lab);
	#endif
}

void Vision::findTags() {
	#ifdef CUDA_FOUNDs
	for (unsigned long i = 0; i < TOTAL_COLORS; i++) {
		segmentAndSearch(i);
	}
	#else
	for (int i = 0; i < TOTAL_COLORS; i++) {
		threshold_threads.add_thread(new boost::thread(&Vision::segmentAndSearch, this, i));
	}
	threshold_threads.join_all();
	#endif
}

void Vision::segmentAndSearch(unsigned long color) {
	#ifdef CUDA_FOUND
	cv::Mat frame;
	cv::Mat threshold;

	hsv_frame.download(frame);

	if (convertType == HSV) {
		inRange(frame, cv::Scalar(hue[color][MIN], saturation[color][MIN], value[color][MIN]),
				cv::Scalar(hue[color][MAX], saturation[color][MAX], value[color][MAX]), threshold);
	} else {
		inRange(frame, cv::Scalar(cieL[color][MIN], cieA[color][MIN], cieB[color][MIN]),
				cv::Scalar(cieL[color][MAX], cieA[color][MAX], cieB[color][MAX]), threshold);
	}

	threshold_frame.at(color).upload(threshold);
	#else
	cv::Mat frame = hsv_frame.clone();

	if (convertType == HSV) {
		inRange(frame, cv::Scalar(hue[color][MIN], saturation[color][MIN], value[color][MIN]),
				cv::Scalar(hue[color][MAX], saturation[color][MAX], value[color][MAX]), threshold_frame.at(color));
	} else {
		inRange(frame, cv::Scalar(cieL[color][MIN], cieA[color][MIN], cieB[color][MIN]),
				cv::Scalar(cieL[color][MAX], cieA[color][MAX], cieB[color][MAX]), threshold_frame.at(color));
	}
	#endif

//	cv::cuda::GpuMat frame = hsv_frame.clone();
//
//	cv::cuda::GpuMat splitFrame[3];
//	cv::cuda::GpuMat thresholdChannel[3];
//	cv::cuda::GpuMat temp;
//
//	//Split HSV 3 channels
//	cv::cuda::split(frame, splitFrame);
//
//	//Threshold HSV channels
//	if (convertType == HSV) {
//		cv::cuda::threshold(splitFrame[0], thresholdChannel[0], hue[color][MIN], hue[color][MAX], cv::THRESH_BINARY);
//		cv::cuda::threshold(splitFrame[1], thresholdChannel[1], saturation[color][MIN], saturation[color][MAX], cv::THRESH_BINARY);
//		cv::cuda::threshold(splitFrame[2], thresholdChannel[2], value[color][MIN], value[color][MAX], cv::THRESH_BINARY);
//	} else {
//		cv::cuda::threshold(splitFrame[0], thresholdChannel[0], cieL[color][MIN], cieL[color][MAX], cv::THRESH_BINARY);
//		cv::cuda::threshold(splitFrame[1], thresholdChannel[1], cieA[color][MIN], cieA[color][MAX], cv::THRESH_BINARY);
//		cv::cuda::threshold(splitFrame[2], thresholdChannel[2], cieB[color][MIN], cieB[color][MAX], cv::THRESH_BINARY);
//	}
//
//	//Bitwise AND the channels
//	cv::cuda::bitwise_and(thresholdChannel[0], thresholdChannel[1],temp);
//	cv::cuda::bitwise_and(temp, thresholdChannel[2], threshold_frame.at(color));

	posProcessing(color);

	searchTags(color);
}

void Vision::posProcessing(unsigned long color) {
	cv::Mat erodeElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

	#ifdef CUDA_FOUND
	cv::Mat mat;
	threshold_frame.at(color).download(mat);

	cv::medianBlur(mat, mat, blur[convertType][color]);
	cv::erode(mat, mat, erodeElement, cv::Point(-1, -1),
			  erode[convertType][color]);
	cv::dilate(mat, mat, dilateElement, cv::Point(-1, -1),
			   dilate[convertType][color]);
//	int type = threshold_frame.at(color).type();
//
//	cv::Ptr<cv::cuda::Filter> blurFilter = cv::cuda::createMedianFilter(type, blur[convertType][color]);
//	blurFilter->apply(threshold_frame.at(color), threshold_frame.at(color));
//
//	cv::Ptr<cv::cuda::Filter> erodeFilter = cv::cuda::createMorphologyFilter(cv::MORPH_ERODE, type, erodeElement, cv::Point(-1, -1), erode[convertType][color]);
//	erodeFilter->apply(threshold_frame.at(color), threshold_frame.at(color));
//
//	cv::Ptr<cv::cuda::Filter> dilateFilter = cv::cuda::createMorphologyFilter(cv::MORPH_DILATE, type, dilateElement, cv::Point(-1, -1), dilate[convertType][color]);
//	dilateFilter->apply(threshold_frame.at(color), threshold_frame.at(color));
	#else
	cv::medianBlur(threshold_frame.at(color), threshold_frame.at(color), blur[convertType][color]);
	cv::erode(threshold_frame.at(color), threshold_frame.at(color), erodeElement, cv::Point(-1, -1),
			  erode[convertType][color]);
	cv::dilate(threshold_frame.at(color), threshold_frame.at(color), dilateElement, cv::Point(-1, -1),
			   dilate[convertType][color]);
	#endif
}

void Vision::searchGMMTags(std::vector<cv::Mat> thresholds) {
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	for (unsigned long color = 0; color < TOTAL_COLORS; color++) {
		tags.at(color).clear();

		cv::Mat tmp;
		cv::cvtColor(thresholds.at(color), tmp, cv::COLOR_RGB2GRAY);

		cv::findContours(tmp, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);

		for (const auto &contour : contours) {
			double area = contourArea(contour);
			if (area >= 10) {
				cv::Moments moment = moments((cv::Mat) contour);
				tags.at(color).emplace_back(cv::Point(static_cast<int>(moment.m10 / area),
													  static_cast<int>(moment.m01 / area)), area);

				// seta as linhas para as tags principais do pick-a-tag
				if (color == MAIN) {
					cv::Vec4f line;
					cv::fitLine(cv::Mat(contour), line, 2, 0, 0.01, 0.01);
					unsigned long tagsInVec = tags.at(color).size() - 1;
					tags.at(color).at(tagsInVec).setLine(line);
				}
			}
		}
	}
}

void Vision::searchTags(unsigned long color) {
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	tags.at(color).clear();

	#ifdef CUDA_FOUND
	cv::Mat frame;
	threshold_frame.at(color).download(frame);
	cv::findContours(frame, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);
	#else
	cv::findContours(threshold_frame.at(color), contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);
	#endif

	for (const auto &contour : contours) {
		double area = contourArea(contour);
		if (area >= areaMin[convertType][color]) {
			cv::Moments moment = moments((cv::Mat) contour);

			// seta as linhas para as tags principais do pick-a-tag
			if (color == MAIN) {
				tags.at(color).emplace_back(cv::Point(static_cast<int>(moment.m10 / area),
													  static_cast<int>(moment.m01 / area)), area);

				// tem que ter jogado a tag no vetor antes de mexer nos valores dela
				cv::Vec4f line;
				cv::fitLine(cv::Mat(contour), line, 2, 0, 0.01, 0.01);
				unsigned long tagsInVec = tags.at(color).size() - 1;
				tags.at(color).at(tagsInVec).setLine(line);
			} else if (color == ADV) {
				if (tags.at(color).size() >= 3) {
					// pega o menor índice
					unsigned long minValue = 0;
					for (unsigned long j = 1; j < 3; j++) {
						if (tags.at(color).at(j).area < tags.at(color).at(minValue).area)
							minValue = j;
					}
					if (minValue < tags.at(color).size() && area > tags.at(color).at(minValue).area)
						tags.at(color).at(minValue) = Tag(cv::Point(static_cast<int>(moment.m10 / area),
																	static_cast<int>(moment.m01 / area)), area);
				} else {
					tags.at(color).emplace_back(cv::Point(static_cast<int>(moment.m10 / area),
														  static_cast<int>(moment.m01 / area)), area);
				}
			} else {
				tags.at(color).emplace_back(cv::Point(static_cast<int>(moment.m10 / area),
													  static_cast<int>(moment.m01 / area)), area);
			}
		}
	}
}

/// <summary>
/// Seleciona um conjunto de tags para representar cada robô. Adaptado aos ROIs utilizados pelo GMM
/// </summary>
/// <description>
/// P.S.: Aqui eu uso a flag 'isOdd' para representar quando um robô tem as duas bolas laterais.
/// </description>
void Vision::pick_a_tag(std::vector<VisionROI> *windowsList) {
	int dist, tmpSide;

	// Define inicialmente que o objeto de cada janela não foi encontrado
	for (auto &i : *windowsList) {
		i.setIsLost(true);
	}

	// OUR ROBOTS
	for (unsigned long i = 0; i < tags.at(MAIN).size() && i < 3; i++) {
		// cria um robô temporário para armazenar nossas descobertas
		Robot robot;
		std::vector<Tag> tempTags;

		// Posição do robô
		robot.position = tags.at(MAIN).at(i).position;

		// Cálculo da orientação de acordo com os pontos rear e front
		robot.orientation = atan2((tags.at(MAIN).at(i).frontPoint.y - robot.position.y) * 1.3 / height,
								  (tags.at(MAIN).at(i).frontPoint.x - robot.position.x) * 1.5 / width);

		// Armazena a tag
		tempTags.push_back(tags.at(MAIN).at(i));

		// Para cada tag principal, verifica quais são as secundárias correspondentes
		for (auto &greenTag : tags.at(GREEN)) {
			// já faz a atribuição verificando se o valor retornado é 0 (falso); além disso, altera a orientação caso esteja errada
			tmpSide = inSphere(&robot, &tempTags, greenTag.position);
			if (tmpSide) {
				// identifica se já tem mais de uma tag
				if (tempTags.size() > 1) {
					robot.isOdd = true;
				}
				greenTag.left = 0 < tmpSide;
				// calculos feitos, joga tag no vetor
				tempTags.push_back(greenTag);
			}
		}


		// Dá nome aos bois (robôs)
		if (robot.isOdd) { // isOdd representa que este tem as duas bolas
			robot_list.at(2).position = robot.position; // colocar em um vetor
			robot_list.at(2).secundary = tempTags.at(0).frontPoint; // colocar em um vetor
			robot_list.at(2).orientation = robot.orientation;
			robot_list.at(2).rearPoint = tempTags.at(0).rearPoint;
			windowsList->at(2).setIsLost(false);
			windowsList->at(2).setCenter(robot.position);
		} else if (tempTags.size() > 1 && tempTags.at(1).left) {
			robot_list.at(0).position = robot.position; // colocar em um vetor
			robot_list.at(0).secundary = tempTags.at(0).frontPoint; // colocar em um vetor
			robot_list.at(0).orientation = robot.orientation;
			robot_list.at(0).rearPoint = tempTags.at(0).rearPoint;
			windowsList->at(0).setIsLost(false);
			windowsList->at(0).setCenter(robot.position);
		} else {
			robot_list.at(1).position = robot.position; // colocar em um vetor
			robot_list.at(1).secundary = tempTags.at(0).frontPoint; // colocar em um vetor
			robot_list.at(1).orientation = robot.orientation;
			robot_list.at(1).rearPoint = tempTags.at(0).rearPoint;
			windowsList->at(1).setIsLost(false);
			windowsList->at(1).setCenter(robot.position);
		}
	} // OUR ROBOTS

	// ADV ROBOTS
	for (unsigned long i = 0; i < tags.at(ADV).size() && i < MAX_ADV; i++) {
		advRobots[i] = tags.at(ADV).at(i).position;
		windowsList->at(4 + i).setIsLost(false);
		windowsList->at(4 + i).setCenter(advRobots[i]);
	}

	// BALL POSITION
	if (!tags[BALL].empty()) {
		ball = tags.at(BALL).at(0).position;
		windowsList->at(3).setIsLost(false);
		windowsList->at(3).setCenter(ball);
	}
}

/// <summary>
/// Seleciona um conjunto de tags para representar cada robô
/// </summary>
/// <description>
/// P.S.: Aqui eu uso a flag 'isOdd' para representar quando um robô tem as duas bolas laterais.
/// </description>
void Vision::pick_a_tag() {
	int tmpSide;

	// OUR ROBOTS
	for (unsigned long i = 0; i < tags.at(MAIN).size() && i < 3; i++) {
		// cria um robô temporário para armazenar nossas descobertas
		Robot robot;
		std::vector<Tag> tempTags;

		// Posição do robô
		robot.position = tags.at(MAIN).at(i).position;

		// Cálculo da orientação de acordo com os pontos rear e front
		robot.orientation = atan2((tags.at(MAIN).at(i).frontPoint.y - robot.position.y) * 1.3 / height,
								  (tags.at(MAIN).at(i).frontPoint.x - robot.position.x) * 1.5 / width);

		// Armazena a tag
		tempTags.push_back(tags.at(MAIN).at(i));

		// Para cada tag principal, verifica quais são as secundárias correspondentes
		for (auto &greenTag : tags.at(GREEN)) {
			// já faz a atribuição verificando se o valor retornado é 0 (falso); além disso, altera a orientação caso esteja errada
			tmpSide = inSphere(&robot, &tempTags, greenTag.position);
			if (tmpSide) {
				// identifica se já tem mais de uma tag
				if (tempTags.size() > 1) {
					robot.isOdd = true;
				}
				greenTag.left = tmpSide > 0;
				// calculos feitos, joga tag no vetor
				tempTags.push_back(greenTag);
			}
		}


		// Dá nome aos bois (robôs)
		if (robot.isOdd) { // isOdd representa que este tem as duas bolas
			robot_list.at(2).position = robot.position; // colocar em um vetor
			robot_list.at(2).secundary = tempTags.at(0).frontPoint; // colocar em um vetor
			robot_list.at(2).orientation = robot.orientation;
			robot_list.at(2).rearPoint = tempTags.at(0).rearPoint;
		} else if (tempTags.size() > 1 && tempTags.at(1).left) {
			robot_list.at(0).position = robot.position; // colocar em um vetor
			robot_list.at(0).secundary = tempTags.at(0).frontPoint; // colocar em um vetor
			robot_list.at(0).orientation = robot.orientation;
			robot_list.at(0).rearPoint = tempTags.at(0).rearPoint;
		} else {
			robot_list.at(1).position = robot.position; // colocar em um vetor
			robot_list.at(1).secundary = tempTags.at(0).frontPoint; // colocar em um vetor
			robot_list.at(1).orientation = robot.orientation;
			robot_list.at(1).rearPoint = tempTags.at(0).rearPoint;
		}
	} // OUR ROBOTS

	// ADV ROBOTS
	for (unsigned long i = 0; i < MAX_ADV; i++) {
		if (i < tags.at(ADV).size())
			advRobots[i] = tags.at(ADV).at(i).position;
		else
			advRobots[i] = cv::Point(-1, -1);
	}

	// BALL POSITION
	if (!tags[BALL].empty()) {
		ball = tags.at(BALL).at(0).position;
	}
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
int Vision::inSphere(Robot *robot, std::vector<Tag> *tempTags, cv::Point secondary) {
	// se esta secundária faz parte do robô
	if (calcDistance(robot->position, secondary) <= ROBOT_RADIUS) {
		if (calcDistance(tempTags->at(0).frontPoint, secondary) < calcDistance(tempTags->at(0).rearPoint, secondary)) {
			tempTags->at(0).switchPoints();
			// recalcula a orientação com os novos pontos (isso só é feito uma vez em cada robô, se necessário)
			robot->orientation = atan2((tempTags->at(0).frontPoint.y - robot->position.y) * 1.3 / height,
									   (tempTags->at(0).frontPoint.x - robot->position.x) * 1.5 / width);
		}

		double secSide = atan2((secondary.y - robot->position.y) * 1.3 / height,
							  (secondary.x - robot->position.x) * 1.5 / width);

		// Cálculo do ângulo de orientação para diferenciar robôs de mesma cor
		return (atan2(sin(secSide - robot->orientation + 3.1415), cos(secSide - robot->orientation + 3.1415))) > 0 ? 1
																												   : -1;
	}
	return 0;
}

double Vision::calcDistance(cv::Point p1, cv::Point p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void Vision::switchMainWithAdv() {
	int tmp;

	if (convertType == HSV) {
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
	} else {
		for (int i = MIN; i <= MAX; i++) {
			tmp = cieL[MAIN][i];
			cieL[MAIN][i] = cieL[ADV][i];
			cieL[ADV][i] = tmp;

			tmp = cieA[MAIN][i];
			cieA[MAIN][i] = cieA[ADV][i];
			cieA[ADV][i] = tmp;

			tmp = cieB[MAIN][i];
			cieB[MAIN][i] = cieB[ADV][i];
			cieB[ADV][i] = tmp;
		}
	}

	tmp = areaMin[convertType][MAIN];
	areaMin[convertType][MAIN] = areaMin[convertType][ADV];
	areaMin[convertType][ADV] = tmp;

	tmp = erode[convertType][MAIN];
	erode[convertType][MAIN] = erode[convertType][ADV];
	erode[convertType][ADV] = tmp;

	tmp = dilate[convertType][MAIN];
	dilate[convertType][MAIN] = dilate[convertType][ADV];
	dilate[convertType][ADV] = tmp;

	tmp = blur[convertType][MAIN];
	blur[convertType][MAIN] = blur[convertType][ADV];
	blur[convertType][ADV] = tmp;
}

#ifdef CUDA_FOUND
cv::Mat Vision::getSplitFrame() {
	cv::cuda::GpuMat frameMap[2][2];

	for (unsigned long index = 0; index < 3; index++) {
		cv::cuda::cvtColor(threshold_frame.at(index), threshold_frame.at(index), cv::COLOR_GRAY2RGB);
	}

	cv::cuda::pyrDown(in_frame, frameMap[0][0]);
	cv::cuda::pyrDown(threshold_frame.at(0), frameMap[0][1]);
	cv::cuda::pyrDown(threshold_frame.at(1), frameMap[1][0]);
	cv::cuda::pyrDown(threshold_frame.at(2), frameMap[1][1]);

	splitFrame = cv::cuda::GpuMat(in_frame.rows, in_frame.cols, in_frame.type());

	frameMap[0][0].copyTo(splitFrame(cv::Rect(0, 0, frameMap[0][0].cols, frameMap[0][0].rows)));
	frameMap[0][1].copyTo(splitFrame(cv::Rect(in_frame.cols/2, 0, frameMap[0][1].cols, frameMap[0][1].rows)));
	frameMap[1][0].copyTo(splitFrame(cv::Rect(0, in_frame.rows/2, frameMap[1][0].cols, frameMap[1][0].rows)));
	frameMap[1][1].copyTo(splitFrame(cv::Rect(in_frame.cols/2, in_frame.rows/2, frameMap[1][1].cols, frameMap[1][1].rows)));

	cv::Mat mat;
	splitFrame.download(mat);

	return mat;
}
#else
cv::Mat Vision::getSplitFrame() {
	cv::Mat horizontal[2], vertical[2];

	for (unsigned long index = 0; index < 3; index++) {
		cv::cvtColor(threshold_frame.at(index), threshold_frame.at(index), cv::COLOR_GRAY2RGB);
	}

	cv::pyrDown(in_frame, vertical[0]);
	cv::pyrDown(threshold_frame.at(0), vertical[1]);

	cv::hconcat(vertical, 2, horizontal[0]);

	cv::pyrDown(threshold_frame.at(1), vertical[0]);
	cv::pyrDown(threshold_frame.at(2), vertical[1]);

	cv::hconcat(vertical, 2, horizontal[1]);

	cv::vconcat(horizontal, 2, splitFrame);

	return splitFrame;
}
#endif

void
Vision::setCalibParams(int type, int H[4][2], int S[4][2], int V[4][2], int Amin[4], int E[4], int D[4], int B[4]) {
	if (type == HSV) {
		for (int i = 0; i < TOTAL_COLORS; i++) {
			areaMin[type][i] = Amin[i];
			erode[type][i] = E[i];
			dilate[type][i] = D[i];
			blur[type][i] = B[i];
			for (int j = 0; j < 2; j++) {
				hue[i][j] = H[i][j];
				saturation[i][j] = S[i][j];
				value[i][j] = V[i][j];
			}
		}
	} else { // CIELAB
		for (int i = 0; i < TOTAL_COLORS; i++) {
			areaMin[type][i] = Amin[i];
			erode[type][i] = E[i];
			dilate[type][i] = D[i];
			blur[type][i] = B[i];
			for (int j = 0; j < 2; j++) {
				cieL[i][j] = H[i][j];
				cieA[i][j] = S[i][j];
				cieB[i][j] = V[i][j];
			}
		}
	}
}

void Vision::saveCamCalibFrame() {
	#ifdef CUDA_FOUND
	cv::cuda::GpuMat temp = in_frame.clone();
	cv::Mat frame;
	temp.download(frame);
	savedCamCalibFrames.push_back(frame);
	#else
	cv::Mat temp = in_frame.clone();
	savedCamCalibFrames.push_back(temp);
	#endif

	std::string text = "CamCalib_" + std::to_string(getCamCalibFrames().size());
	saveCameraCalibPicture(text, "media/pictures/camCalib/");
	std::cout << "Saving picture " << std::endl;
}

void Vision::collectImagesForCalibration() {
	std::cout << "Collecting pictures " << std::endl;
	cv::String path("media/pictures/camCalib/*.png"); //select only png
	std::vector<cv::String> fn;
	std::vector<cv::Mat> data;
	cv::glob(path, fn, true); // recurse
	for (size_t k = 0; k < fn.size(); ++k) {
		cv::Mat im = cv::imread(fn[k]);
		if (im.empty()) continue; //only proceed if sucsessful
		// you probably want to do some preprocessing
		savedCamCalibFrames.push_back(im);
	}
	std::cout << "Pictures collected: " << savedCamCalibFrames.size() << std::endl;
	cameraCalibration();
}

void Vision::cameraCalibration() {

	std::vector<std::vector<cv::Point2f>> checkerBoardImageSpacePoints;
	checkerBoardImageSpacePoints = getChessBoardCorners(savedCamCalibFrames);
	std::cout << "Image Space Points " << checkerBoardImageSpacePoints.size() << std::endl;
	std::vector<std::vector<cv::Point3f>> worldSpaceCornersPoints(1);

	worldSpaceCornersPoints[0] = createKnownBoardPosition(CHESSBOARD_DIMENSION, CALIBRATION_SQUARE_DIMENSION);
	worldSpaceCornersPoints.resize(checkerBoardImageSpacePoints.size(), worldSpaceCornersPoints[0]);
	std::cout << "world SpaceCorners Points " << worldSpaceCornersPoints.size() << std::endl;
	std::vector<cv::Mat> rVectors, tVectors;
	distanceCoeficents = cv::Mat::zeros(8, 1, CV_64F);

	//root mean square (RMS) reprojection error and should be between 0.1 and 1.0 pixels in a good calibration.
	double rms = cv::calibrateCamera(worldSpaceCornersPoints, checkerBoardImageSpacePoints, CHESSBOARD_DIMENSION,
									 cameraMatrix, distanceCoeficents, rVectors, tVectors);
	savedCamCalibFrames.clear();

	flag_cam_calibrated = true;

	std::cout << "Camera parameters matrix." << std::endl;
	std::cout << cameraMatrix << std::endl;
	std::cout << "Camera distortion coefficients" << std::endl;
	std::cout << distanceCoeficents << std::endl;
	std::cout << "RMS" << std::endl;
	std::cout << rms << std::endl;
	std::cout << "End of calibration" << std::endl;
}

// criando um vetor com a posiçap de todos os pontos que pertencem ao padrao em milimetros desconsiderando Z para ficar
// computacionalmente mais barato
std::vector<cv::Point3f> Vision::createKnownBoardPosition(cv::Size boardSize, float squareEdgeLenght) {
	std::vector<cv::Point3f> corners;
	for (int i = 0; i < boardSize.height; i++) {
		for (int j = 0; j < boardSize.width; ++j) {
			corners.emplace_back(j * squareEdgeLenght, i * squareEdgeLenght, 0.0f);
		}
	}

	return corners;
}

std::vector<std::vector<cv::Point2f>> Vision::getChessBoardCorners(std::vector<cv::Mat> images) {
	cv::TermCriteria termCriteria = cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001);
	cv::Mat grayFrame;
	std::vector<std::vector<cv::Point2f>> allFoundCorners;
	for (auto &image : images) {
		std::vector<cv::Point2f> pointBuf;
		bool found = cv::findChessboardCorners(image, CHESSBOARD_DIMENSION, pointBuf,
											   cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);

		if (found) {
			cv::cvtColor(image, grayFrame, cv::COLOR_RGB2GRAY);
			cv::cornerSubPix(grayFrame, pointBuf, cv::Size(11, 11), cv::Size(-1, -1), termCriteria);
			allFoundCorners.push_back(pointBuf);
		}
	}
	return allFoundCorners;
}

bool Vision::foundChessBoardCorners() {
	std::vector<cv::Vec2f> foundPoints;
	#ifdef CUDA_FOUND
	cv::cuda::GpuMat temp = in_frame.clone();
	cv::Mat frame;
	temp.download(frame);

	return cv::findChessboardCorners(frame, CHESSBOARD_DIMENSION, foundPoints,
									 cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
	#else
	cv::Mat temp;
	temp = in_frame.clone();

	return cv::findChessboardCorners(temp, CHESSBOARD_DIMENSION, foundPoints,
									 cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
	#endif
}

void Vision::saveCameraCalibPicture(std::string in_name, std::string directory) {
	#ifdef CUDA_FOUND
	cv::cuda::GpuMat temp = in_frame.clone();
	cv::Mat frame;
	temp.download(frame);
	#else
	cv::Mat frame = in_frame.clone();
	#endif

	std::string picName = directory + in_name + ".png";

	cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
	cv::imwrite(picName, frame);
}

void Vision::savePicture(std::string in_name) {
	#ifdef CUDA_FOUND
	cv::cuda::GpuMat temp = in_frame.clone();
	cv::Mat frame;
	temp.download(frame);
	#else
	cv::Mat frame = in_frame.clone();
	#endif

	std::string picName = "media/pictures/" + in_name + ".png";

	cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
	cv::imwrite(picName, frame);
}

void Vision::startNewVideo(std::string in_name) {
	std::string videoName = "media/videos/" + in_name + ".avi";

	video.open(videoName, CV_FOURCC('M', 'J', 'P', 'G'), 30, cv::Size(width, height));
	std::cout << "Started a new video recording." << std::endl;
	bOnAir = true;
}

bool Vision::recordToVideo() {
	if (!video.isOpened()) return false;
	#ifdef CUDA_FOUND
	cv::cuda::GpuMat temp;
	cv::cuda::cvtColor(in_frame, temp, cv::COLOR_RGB2BGR);
	cv::Mat frame;
	temp.download(frame);
	#else
	cv::Mat frame = in_frame.clone();
	cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
	#endif

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

Robot Vision::getRobot(unsigned long index) {
	return robot_list.at(index);
}

cv::Point Vision::getRobotPos(unsigned long index) {
	return robot_list.at(index).position;
}

cv::Point Vision::getAdvRobot(int index) {
	if (index < 0 || index >= MAX_ADV) {
		std::cout << "Vision::getAdvRobot: index argument is invalid." << std::endl;
		return cv::Point(-1, -1);
	} else {
		return advRobots[index];
	}
}

int Vision::getRobotListSize() {
	return static_cast<int>(robot_list.size());
}

int Vision::getAdvListSize() {
	return MAX_ADV;
}

cv::Mat Vision::getThreshold(unsigned long index) {
	#ifdef CUDA_FOUND
	cv::cuda::cvtColor(threshold_frame.at(index), threshold_frame.at(index), cv::COLOR_GRAY2RGB);
	cv::Mat mat;
	threshold_frame.at(index).download(mat);
	return mat;
	#else
	cv::cvtColor(threshold_frame.at(index), threshold_frame.at(index), cv::COLOR_GRAY2RGB);
	return threshold_frame.at(index);
	#endif
}

int Vision::getHue(int index0, int index1) {
	return hue[index0][index1];
}

int Vision::getSaturation(int index0, int index1) {
	return saturation[index0][index1];
}

std::vector<cv::Mat> Vision::getCamCalibFrames() {
	return savedCamCalibFrames;
}

cv::Mat Vision::getcameraMatrix() {
	return cameraMatrix;
}

cv::Mat Vision::getdistanceCoeficents() {
	return distanceCoeficents;
}

void Vision::setFlagCamCalibrated(bool value) {
	this->flag_cam_calibrated = value;
}

void Vision::popCamCalibFrames() {
	savedCamCalibFrames.pop_back();
}

int Vision::getValue(int index0, int index1) {
	return value[index0][index1];
}

int Vision::getCIE_L(int index0, int index1) {
	return cieL[index0][index1];
}

int Vision::getCIE_A(int index0, int index1) {
	return cieA[index0][index1];
}

int Vision::getCIE_B(int index0, int index1) {
	return cieB[index0][index1];
}

int Vision::getErode(int convertType, int index) {
	return erode[convertType][index];
}

int Vision::getDilate(int convertType, int index) {
	return dilate[convertType][index];
}

int Vision::getBlur(int convertType, int index) {
	return blur[convertType][index];
}

int Vision::getAmin(int convertType, int index) {
	return areaMin[convertType][index];
}

int Vision::getConvertType() {
	return convertType;
}

void Vision::setHue(int index0, int index1, int inValue) {
	if (index0 >= 0 && index0 < TOTAL_COLORS && (index1 == 0 || index1 == 1)) hue[index0][index1] = inValue;
	else std::cout << "Vision:setHue: could not set (invalid index)" << std::endl;
}

void Vision::setCIE_L(int index0, int index1, int inValue) {
	if (index0 >= 0 && index0 < TOTAL_COLORS && (index1 == 0 || index1 == 1)) cieL[index0][index1] = inValue;
	else std::cout << "Vision:setCIE_L: could not set (invalid index)" << std::endl;
}

void Vision::setCIE_A(int index0, int index1, int inValue) {
	if (index0 >= 0 && index0 < TOTAL_COLORS && (index1 == 0 || index1 == 1)) cieA[index0][index1] = inValue;
	else std::cout << "Vision:setCIE_A: could not set (invalid index)" << std::endl;
}

void Vision::setCIE_B(int index0, int index1, int inValue) {
	if (index0 >= 0 && index0 < TOTAL_COLORS && (index1 == 0 || index1 == 1)) cieB[index0][index1] = inValue;
	else std::cout << "Vision:setCIE_B: could not set (invalid index)" << std::endl;
}

void Vision::setSaturation(int index0, int index1, int inValue) {
	if (index0 >= 0 && index0 < TOTAL_COLORS && (index1 == 0 || index1 == 1)) saturation[index0][index1] = inValue;
	else std::cout << "Vision:setSaturation: could not set (invalid index)" << std::endl;
}

void Vision::setValue(int index0, int index1, int inValue) {
	if (index0 >= 0 && index0 < TOTAL_COLORS && (index1 == 0 || index1 == 1)) value[index0][index1] = inValue;
	else std::cout << "Vision:setValue: could not set (invalid index)" << std::endl;
}

void Vision::setErode(int convertType, int index, int inValue) {
	if (index >= 0 && index < TOTAL_COLORS && (convertType == HSV || convertType == CIELAB))
		erode[convertType][index] = inValue;
	else std::cout << "Vision:setErode: could not set (invalid index or convert type)" << std::endl;
}

void Vision::setDilate(int convertType, int index, int inValue) {
	if (index >= 0 && index < TOTAL_COLORS && (convertType == HSV || convertType == CIELAB))
		dilate[convertType][index] = inValue;
	else std::cout << "Vision:setDilate: could not set (invalid index or convert type)" << std::endl;
}

void Vision::setBlur(int convertType, int index, int inValue) {
	if (index >= 0 && index < TOTAL_COLORS && (convertType == HSV || convertType == CIELAB))
		blur[convertType][index] = inValue;
	else std::cout << "Vision:setBlur: could not set (invalid index or convert type)" << std::endl;
}

void Vision::setAmin(int convertType, int index, int inValue) {
	if (index >= 0 && index < TOTAL_COLORS && (convertType == HSV || convertType == CIELAB))
		areaMin[convertType][index] = inValue;
	else std::cout << "Vision:setAmin: could not set (invalid index or convert type)" << std::endl;
}

void Vision::setConvertType(int type) {
	if (type == HSV || type == CIELAB) convertType = type;
	else std::cout << "Vision:setConvertType: could not set type (invalid type)" << std::endl;
}

void Vision::setFrameSize(int inWidth, int inHeight) {
	if (inWidth >= 0) width = inWidth;
	if (inHeight >= 0) height = inHeight;
}

cv::Point *Vision::getAllAdvRobots() {
	return advRobots;
}

Vision::Vision(int w, int h) : width(w), height(h), bOnAir(false), convertType(HSV),
							   robot_list(3), threshold_frame(TOTAL_COLORS), tags(TOTAL_COLORS) {
}

Vision::~Vision() = default;


