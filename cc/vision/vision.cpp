#include <Geometry/Geometry.h>
#include <Strategy2/Field.h>
#include "vision.hpp"

using namespace vision;

// Todas as cores que serão encontradas pela visão:
// Qualquer alteração nessa variável precisa ser refletida no enum Color no arquivo hpp.
const char * const Vision::Color_Name[] = {
	"Yellow",
	"Blue",
	"Red",
	"Green",
	"Cyan",
	"Magenta",
	"Ball"
};

Tags Vision::run(cv::Mat raw_frame, bool yellow_pick_at_tag, bool blue_pick_at_tag) {
	in_frame = raw_frame.clone();

	preProcessing();
	findTags();
	//findElements();
	return find_all_tags(yellow_pick_at_tag, blue_pick_at_tag);
}

void Vision::preProcessing() {
	cv::cvtColor(in_frame, lab_frame, cv::COLOR_RGB2Lab);
}

void Vision::findTags() {
	for (unsigned int color = 0; color < MAX_COLORS; color++) {
		threshold_threads.add_thread(new boost::thread(&Vision::segmentAndSearch, this, color));
	}
	threshold_threads.join_all();
}

void Vision::segmentAndSearch(const unsigned long color) {
	cv::Mat frame = lab_frame.clone();

	inRange(frame, cv::Scalar(cieL[color][Limit::Min], cieA[color][Limit::Min], cieB[color][Limit::Min]),
			cv::Scalar(cieL[color][Limit::Max], cieA[color][Limit::Max], cieB[color][Limit::Max]), threshold_frame.at(color));

	posProcessing(color);
	searchTags(color);
}

void Vision::posProcessing(const unsigned long color) {
	int  morphShape;
	cv::Size size;

	if (color == Color::Ball) {
		morphShape = cv::MORPH_ELLIPSE;
		size = cv::Size(15, 15);
	} else {
		morphShape = cv::MORPH_RECT;
		size = cv::Size(3, 3);
	}

	cv::Mat erodeElement = cv::getStructuringElement(morphShape, size);
	cv::Mat dilateElement = cv::getStructuringElement(morphShape, size);

	if (blur[color] > 0) {
		cv::medianBlur(threshold_frame.at(color), threshold_frame.at(color), blur[color]);
	}
	cv::erode(threshold_frame.at(color), threshold_frame.at(color), erodeElement, cv::Point(-1, -1),
			  erode[color]);
	cv::dilate(threshold_frame.at(color), threshold_frame.at(color), dilateElement, cv::Point(-1, -1),
			   dilate[color]);
}

void Vision::searchTags(const unsigned long color) {
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	tags.at(color).clear();

	cv::findContours(threshold_frame.at(color), contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);

	for (const auto &contour : contours) {
		double area = contourArea(contour);
		if (area >= areaMin[color]) {
            // Calcula o centro do contour
            cv::Point_<double> sum(0,0);
            for(cv::Point point : contour)
            {
                sum += static_cast<cv::Point_<double>>(point);
            }
            cv::Point_<double> center = sum/(float)contour.size();

            tags.at(color).emplace_back(center, area);
        }
    }
}

// Retorna o id da tag de acordo com as
// Regras IEEE Very Small Size Soccer (VSSS) - Série A; Anexo 1 - Padrão de Cores
int Vision::get_tag_id(Color left, Color right) {
	if ( left == Color::Red     && right == Color::Green )   return 0;
	if ( left == Color::Red     && right == Color::Cyan )    return 1;
	if ( left == Color::Green   && right == Color::Red )     return 2;
	if ( left == Color::Green   && right == Color::Cyan )    return 3;
	if ( left == Color::Green   && right == Color::Magenta ) return 4;
	if ( left == Color::Cyan    && right == Color::Red )     return 5;
	if ( left == Color::Cyan    && right == Color::Green )   return 6;
	if ( left == Color::Cyan    && right == Color::Magenta ) return 7;
	if ( left == Color::Magenta && right == Color::Green )   return 8;
	if ( left == Color::Magenta && right == Color::Cyan )    return 9;
	return -1;
}

std::vector<Tag> Vision::pick_a_tag(Color color) {
	std::vector<Tag> found_tags;
	std::vector<std::reference_wrapper<Tag>> main_tags;

	// Adiciona todas as tags principais em um vetor
	// de referências para facilidade de iteração.
	const Color Main_Colors[] = { Color::Yellow, Color::Blue };
	for (Color main_color : Main_Colors) {
		for (Tag & main_tag : tags.at(main_color)) {
			main_tag.color = main_color;
			main_tags.push_back(main_tag);
		}
	}

	// While loop enquanto existe uma tag principal associada à exatamente duas tags secundárias.
	// A cada iteração, quando existe alguma ambiguidade com uma tag principal próxima de
	// mais de duas tags secundárias, ela é ignorada e testada de novo na próxima iteração, esperando
	// que a ambiguidade já tenha sido resolvida usando a variável already_assigned_to_robot.
	while (true) {
		bool found_a_new_tag = false;
		for (Tag & main_tag : main_tags) {
			if (main_tag.already_assigned_to_robot) continue;

			// Tags secundárias dentro do ROBOT_RADIUS.
			std::vector<std::reference_wrapper<Tag>> secondary_tags;

			// Para cada tag principal, verifica quais são as tags de cores secundárias correspondentes.
			const Color Secondary_Colors[] = { Color::Red, Color::Green, Color::Cyan, Color::Magenta };
			for (Color secondary_color : Secondary_Colors) {
				for (Tag & secondary_tag : tags.at(secondary_color)) {
					if (secondary_tag.already_assigned_to_robot) continue;
                    if (calcDistance(main_tag.position, secondary_tag.position) <= ROBOT_RADIUS) {
							secondary_tag.color = secondary_color;
							secondary_tags.push_back(secondary_tag);
					}
				}
			}

			if (secondary_tags.size() == 2) {
				// Ajusta o centro do robô.
				// O centro da main_tag é o centro da cor principal, desse modo é necessário
				// mover o centro em direção às tags secundárias.
				cv::Point_<double> center_correction = secondary_tags[0].get().position/4 + secondary_tags[1].get().position/4 - main_tag.position/2;
				main_tag.position += center_correction;

				// Coloca a tag à esquerda na primeira posição do array.
                // Compara se as tags estão no sentido horário, assim a primeira estaria à esquerda.
                if (are_secondary_tags_clockwise(main_tag, secondary_tags[0].get(), secondary_tags[1].get()))
                {
                    std::cout << "Before swap: " << secondary_tags[0].get().id << " " << secondary_tags[1].get().id << std::endl;
					std::swap(secondary_tags[0], secondary_tags[1]);
                    std::cout << "after swap: " << secondary_tags[0].get().id << " " << secondary_tags[1].get().id << std::endl;
				}

				main_tag.id = get_tag_id((Color) secondary_tags[0].get().color, 
				                         (Color) secondary_tags[1].get().color);

                // Verifica se a tag é válida
				if (main_tag.id >= 0) {
					found_a_new_tag = true;
					main_tag.already_assigned_to_robot = true;
					for ( Tag & secondary_tag : secondary_tags ){
						secondary_tag.already_assigned_to_robot = true;
					}

                    calculate_orientation_and_front_and_rear(main_tag, secondary_tags[0].get(), secondary_tags[1].get());
				}
			}
		}
		if (!found_a_new_tag) break;
	}

	// Adiciona tags da cor selecionada para o vetor de retorno.
	for (Tag & main_tag : main_tags) {
		if ( main_tag.color == color && 
		     main_tag.already_assigned_to_robot ) {
			found_tags.push_back(main_tag);
		}
	}

	// Ordena as tags de acordo com o seu id.
	std::sort(found_tags.begin(), found_tags.end(), [](Tag& tag1, Tag& tag2) {return tag1.id < tag2.id;});

	// Limita a 3 tags.
	if (found_tags.size() <= 3)  {
		return found_tags;
	} else {
		return {found_tags.begin(), found_tags.begin() + 3};
	}
}

Tags Vision::find_all_tags(bool yellow_pick_at_tag, bool blue_pick_at_tag) {
	Tags found_tags;

	found_tags.yellow_has_orientation = yellow_pick_at_tag;
	found_tags.yellow = pick_a_tag(Color::Yellow);

	found_tags.blue_has_orientation = blue_pick_at_tag;
	found_tags.blue = pick_a_tag(Color::Blue);

	// BALL POSITION
	if (!tags[Color::Ball].empty()) {
		found_tags.ball = tags.at(Color::Ball).at(0);
		found_tags.found_ball = true;
	} else {
		// É importante que a posição da bola permaneça sendo a última encontrada
		// para que os robôs funcionem corretamente em caso de oclusão da bola na imagem
		// portanto, a posição da bola não deve ser alterada aqui
		found_tags.found_ball = false;
	}

	return found_tags;
};

/// <summary>
/// Seleciona um conjunto de tags para representar cada robô
/// </summary>
/// <description>
/// P.S.: Aqui eu uso a flag 'isOdd' para representar quando um robô tem as duas bolas laterais.
/// </description>
/**
std::map<unsigned int, RecognizedTag> Vision::pick_a_tag() {
	std::map<unsigned int, RecognizedTag> found_tags;

	advRobots.clear();

	// OUR ROBOTS
	for (unsigned int i = 0; i < tags.at(Color::Yellow).size() && i < 3; i++) {
		std::vector<Tag> secondary_tags;

		Tag main_tag = tags.at(Color::Yellow).at(i);

		// Posição do robô
		cv::Point position = main_tag.position;

		// Cálculo da orientação de acordo com os pontos rear e front
		double orientation = atan2((main_tag.frontPoint.y - position.y) * field::field_height / height,
								  (main_tag.frontPoint.x - position.x) * field::field_width / width);

		// Para cada tag principal, verifica quais são as secundárias correspondentes
		for (Tag &secondary_tag : tags.at(Color::Green)) {

			// Altera a orientação caso esteja errada
			int tag_side = in_sphere(secondary_tag.position, main_tag, orientation);
			if (tag_side != 0) {
				secondary_tag.left = tag_side > 0;
				// calculos feitos, joga tag no vetor
				secondary_tags.push_back(secondary_tag);
			}
		}

		if (secondary_tags.size() > 1) {
			// tag 3 tem duas tags secundárias
			RecognizedTag tag = {position, orientation,
								 main_tag.frontPoint, main_tag.rearPoint};
			found_tags.insert(std::make_pair(2, tag));
		} else if (!secondary_tags.empty()) {
			RecognizedTag tag = {position, orientation,
								 main_tag.frontPoint, main_tag.rearPoint};
			if (secondary_tags[0].left) {
				found_tags.insert(std::make_pair(0, tag));
			} else {
				found_tags.insert(std::make_pair(1, tag));
			}
		}
	} // OUR ROBOTS

	// ADV ROBOTS
	for (unsigned long i = 0; i < MAX_ADV; i++) {
		if (i < tags.at(Color::Blue).size())
			advRobots.push_back(tags.at(Color::Blue).at(i).position);
	}

	// BALL POSITION
	if (!tags[Color::Ball].empty()) {
		ball.position = tags.at(Color::Ball).at(0).position;
		ball.isFound = true;
	} else {
		// É importante que a posição da bola permaneça sendo a última encontrada
		// para que os robôs funcionem corretamente em caso de oclusão da bola na imagem
		// portanto, a posição da bola não deve ser alterada aqui
		ball.isFound = false;
	}

	return found_tags;
}
 **/

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

bool Vision::are_secondary_tags_clockwise(Tag main_tag, Tag tag0, Tag tag1)
{
    // Vetores com origem em main_tag
    cv::Point_<double> vec0 = tag0.position - main_tag.position;
    cv::Point_<double> vec1 = tag1.position - main_tag.position;

    // Ajusta o aspect ratio dos vetores
    vec0 = cv::Point_<double>(vec0.x * field::field_width / width,
                              vec0.y * field::field_height / height);
    vec0 = cv::Point_<double>(vec1.x * field::field_width / width,
                              vec1.y * field::field_height / height);
    // https://stackoverflow.com/questions/14066933/direct-way-of-computing-the-clockwise-angle-between-two-vectors
    double dot_product = vec0.x*vec1.x + vec0.y*vec1.y;
    double determinant = vec0.x*vec1.y - vec1.x*vec0.y;
    double signed_angle = atan2(determinant, dot_product);
    bool is_clockwise = signed_angle < 0;
    return is_clockwise;
}

void Vision::calculate_orientation_and_front_and_rear(Tag & main_tag, Tag left_tag, Tag right_tag)
{
    cv::Point_<double> front = main_tag.position;
    cv::Point_<double> rear = 0.5*left_tag.position + 0.5*right_tag.position;
    cv::Point_<double> direction_vector = front-rear;

    // Calcula o ângulo relativo ao eixo x.
    main_tag.orientation = atan2(direction_vector.y * field::field_height / height,
                                 direction_vector.x * field::field_width / width);

    // Usado apenas para desenhar a linha
    // TODO:: Remover a dependência de front_point e rear_point
    main_tag.front_point = front+1.5 *2.5*direction_vector;
    main_tag.rear_point = rear-1.5 *1.5*direction_vector;
}

double Vision::calcDistance(const cv::Point p1, const cv::Point p2) const {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void Vision::switchMainWithAdv() {
	int tmp;

	for (int i = Limit::Min; i <= Limit::Max; i++) {
		tmp = cieL[Color::Yellow][i];
		cieL[Color::Yellow][i] = cieL[Color::Blue][i];
		cieL[Color::Blue][i] = tmp;

		tmp = cieA[Color::Yellow][i];
		cieA[Color::Yellow][i] = cieA[Color::Blue][i];
		cieA[Color::Blue][i] = tmp;

		tmp = cieB[Color::Yellow][i];
		cieB[Color::Yellow][i] = cieB[Color::Blue][i];
		cieB[Color::Blue][i] = tmp;
	}

	tmp = areaMin[Color::Yellow];
	areaMin[Color::Yellow] = areaMin[Color::Blue];
	areaMin[Color::Blue] = tmp;

	tmp = erode[Color::Yellow];
	erode[Color::Yellow] = erode[Color::Blue];
	erode[Color::Blue] = tmp;

	tmp = dilate[Color::Yellow];
	dilate[Color::Yellow] = dilate[Color::Blue];
	dilate[Color::Blue] = tmp;

	tmp = blur[Color::Yellow];
	blur[Color::Yellow] = blur[Color::Blue];
	blur[Color::Blue] = tmp;
}

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

void Vision::saveCamCalibFrame() {
//    cv::Mat temp = rawFrameCamcalib.clone();
	cv::Mat temp = in_frame.clone();
	savedCamCalibFrames.push_back(temp);
	std::string text = "CamCalib_" + std::to_string(getCamCalibFrames().size());
	saveCameraCalibPicture(text, "media/pictures/camCalib/");
	std::cout << "Saving picture " << std::endl;
}

void Vision::collectImagesForCalibration() {
	std::cout << "Collecting pictures " << std::endl;
	cv::String path("media/pictures/camCalib/*.png"); //select only png
	std::vector<cv::String> fn;
	std::vector<cv::Mat> data;
    try{
        cv::glob(path, fn, true); // recurse
        for (auto &index : fn) {
            cv::Mat im = cv::imread(index);
            if (im.empty()) continue; //only proceed if sucsessful
            // you probably want to do some preprocessing
            savedCamCalibFrames.push_back(im);
        }
        std::cout << "Pictures collected: " << savedCamCalibFrames.size() << std::endl;
        cameraCalibration();

    }catch (...){
        std::cout << "An exception occurred. No images for calibration. \n";
    }

}

void Vision::cameraCalibration() {

	std::vector<std::vector<cv::Point2f>> checkerBoardImageSpacePoints;
    std::vector<std::vector<cv::Point3f>> worldSpaceCornersPoints;

	getChessBoardCorners(savedCamCalibFrames, worldSpaceCornersPoints, checkerBoardImageSpacePoints);
	std::cout << "Image Space Points " << checkerBoardImageSpacePoints.size() << std::endl;

	std::cout << "world SpaceCorners Points " << worldSpaceCornersPoints.size() << std::endl;
	std::vector<cv::Mat> rVectors, tVectors;
	distanceCoeficents = cv::Mat::zeros(8, 1, CV_64F);

    int flag = 0;
    flag |= cv::CALIB_FIX_K4;
    flag |= cv::CALIB_FIX_K5;

	//root mean square (RMS) reprojection error and should be between 0.1 and 1.0 pixels in a good calibration.
    double rms = cv::calibrateCamera(worldSpaceCornersPoints, checkerBoardImageSpacePoints, in_frame.size(),
                                     cameraMatrix, distanceCoeficents, rVectors, tVectors, flag);

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


void Vision::getChessBoardCorners(std::vector<cv::Mat> images, std::vector<std::vector<cv::Point3f>>& pts3d,std::vector<std::vector<cv::Point2f>>& pts2d) const {
	cv::TermCriteria termCriteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 40, 0.001);
	cv::Mat grayFrame;
	//std::vector<std::vector<cv::Point2f>> allFoundCorners;
	for (auto &image : images) {
		std::vector<cv::Point2f> pointBuf;
        std::vector<cv::Point3f> corners;

		bool found = cv::findChessboardCorners(image, CHESSBOARD_DIMENSION, pointBuf,
											   cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
        for (int i = 0; i < CHESSBOARD_DIMENSION.height; i++) {
            for (int j = 0; j < CHESSBOARD_DIMENSION.width; ++j) {
                corners.emplace_back(j * CALIBRATION_SQUARE_DIMENSION, i * CALIBRATION_SQUARE_DIMENSION, 0.0f);
            }
        }

		if (found) {
			cv::cvtColor(image, grayFrame, cv::COLOR_RGB2GRAY);
			cv::cornerSubPix(grayFrame, pointBuf, cv::Size(5, 5), cv::Size(-1, -1), termCriteria);
			pts2d.push_back(pointBuf);
            pts3d.push_back(corners);
		}
	}
}

bool Vision::foundChessBoardCorners() const {
	std::vector<cv::Vec2f> foundPoints;
	cv::Mat temp;
	temp = in_frame.clone();

	return cv::findChessboardCorners(temp, CHESSBOARD_DIMENSION, foundPoints,
									 cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
}

void Vision::saveCameraCalibPicture(const std::string in_name, const std::string directory) {
	cv::Mat frame = in_frame.clone();
	std::string picName = directory + in_name + ".png";

	cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
	cv::imwrite(picName, frame);
}

cv::Mat Vision::getThreshold(const unsigned long index) {
	cv::cvtColor(threshold_frame.at(index), threshold_frame.at(index), cv::COLOR_GRAY2RGB);
	return threshold_frame.at(index);
}

void Vision::setFlagCamCalibrated(const bool value) {
	this->flag_cam_calibrated = value;
}

void Vision::popCamCalibFrames() {
	savedCamCalibFrames.pop_back();
}

void Vision::setCIE_L(const unsigned long index0, const int index1, const int inValue) {
	if (index0 < MAX_COLORS && (index1 == Limit::Min || index1 == Limit::Max)) cieL[index0][index1] = inValue;
	else std::cout << "Vision:setCIE_L: could not set (invalid index)" << std::endl;
}

void Vision::setCIE_A(const unsigned long index0, const int index1, const int inValue) {
	if (index0 < MAX_COLORS && (index1 == Limit::Min || index1 == Limit::Max)) cieA[index0][index1] = inValue;
	else std::cout << "Vision:setCIE_A: could not set (invalid index)" << std::endl;
}

void Vision::setCIE_B(const unsigned long index0, const int index1, const int inValue) {
	if (index0 < MAX_COLORS && (index1 == Limit::Min || index1 == Limit::Max)) cieB[index0][index1] = inValue;
	else std::cout << "Vision:setCIE_B: could not set (invalid index)" << std::endl;
}

void Vision::setErode(const unsigned long index, const int inValue) {
	if (index < MAX_COLORS)
		erode[index] = inValue;
	else std::cout << "Vision:setErode: could not set (invalid index or convert type)" << std::endl;
}

void Vision::setDilate(const unsigned long index, const int inValue) {
	if (index < MAX_COLORS)
		dilate[index] = inValue;
	else std::cout << "Vision:setDilate: could not set (invalid index or convert type)" << std::endl;
}

void Vision::setBlur(const unsigned long index, int inValue) {
	if (index < MAX_COLORS) {
		if (inValue == 0 || inValue % 2 == 1)
			blur[index] = inValue;
		else
			blur[index] = inValue+1;
	} else {
		std::cout << "Vision:setBlur: could not set (invalid index or convert type)" << std::endl;
	}
}

void Vision::setAmin(const unsigned long index, const int inValue) {
	if (index < MAX_COLORS)
		areaMin[index] = inValue;
	else std::cout << "Vision:setAmin: could not set (invalid index or convert type)" << std::endl;
}

void Vision::setFrameSize(const int inWidth, const int inHeight) {
	if (inWidth >= 0) width = inWidth;
	if (inHeight >= 0) height = inHeight;
}

Vision::Vision(int w, int h)
		:
		width(w),
		height(h),
		threshold_frame(MAX_COLORS),
		tags(MAX_COLORS)
{
		for( unsigned int i = 0; i < MAX_COLORS; i++ )
		{
			cieL[i][0] = 0;
			cieL[i][1] = 255;
			cieA[i][0] = 0;
			cieA[i][1] = 255;
			cieB[i][0] = 0;
			cieB[i][1] = 255;
			dilate[i] = 0;
			erode[i] = 0;
			blur[i] = 3;
			areaMin[i] = 30; //Valores antigos: Yellow=50, Green=20, Ball=30, Blue=30
		}
}

Vision::~Vision() = default;




