#include "gmm.hpp"

void GMM::run(cv::Mat frame) {

  inFrame = frame.clone();

  if (isDone && checkROIs()) {
    for (int i = 0; i < TOTAL_WINDOWS; i++) {
      threads.add_thread(new boost::thread(&GMM::classifyWindows, this, i));
    }
    threads.join_all();

    paintWindows();
    joinWindowsToFrames();
    setAllThresholds();
  } else {
    for (int i = 0; i < TOTAL_THREADS; i++) {
      threads.add_thread(new boost::thread(&GMM::classify, this, i));
    }
    threads.join_all();

    cv::Mat emptyMat;
    gaussiansFrame = emptyMat;
    cv::vconcat(partialFrames, TOTAL_THREADS, gaussiansFrame);
    paint();
    setAllThresholds();
  }

  for (int i = 0; i < TOTAL_COLORS; i++) {
    threads.add_thread(new boost::thread(&GMM::posProcessing, this, i));
  }
  threads.join_all();
}

void GMM::joinWindowsToFrames() {
  gaussiansFrame = cv::Mat::zeros(inFrame.rows, inFrame.cols, CV_8UC3);
  finalFrame = cv::Mat::zeros(inFrame.rows, inFrame.cols, CV_8UC3);
  preThreshold = cv::Mat::zeros(inFrame.rows, inFrame.cols, CV_8UC3);

  for (int i = 0; i < TOTAL_WINDOWS; i++) {
    int x = windowsList.at(i).getX();
    int y = windowsList.at(i).getY();
    int size = windowsList.at(i).getSize();
    partialGaussians[i].copyTo(gaussiansFrame(cv::Rect(x, y, size, size)));
    partialFinals[i].copyTo(finalFrame(cv::Rect(x, y, size, size)));
    partialPreThresholds[i].copyTo(preThreshold(cv::Rect(x, y, size, size)));
  }
}

void GMM::posProcessing(int index) {

    cv::Mat erodeElement = cv::getStructuringElement( cv::MORPH_RECT,cv::Size(3,3));
    cv::Mat dilateElement = cv::getStructuringElement( cv::MORPH_RECT,cv::Size(3,3));

    cv::cvtColor(threshold_frame.at(index), threshold_frame.at(index), cv::COLOR_RGB2GRAY);

    if (blur[index] > 0)
      cv::medianBlur(threshold_frame.at(index), threshold_frame.at(index), blur[index]);

    cv::erode(threshold_frame.at(index),threshold_frame.at(index),erodeElement,cv::Point(-1,-1),erode[index]);
    cv::dilate(threshold_frame.at(index),threshold_frame.at(index),dilateElement,cv::Point(-1,-1),dilate[index]);

    cv::cvtColor(threshold_frame.at(index), threshold_frame.at(index), cv::COLOR_GRAY2RGB);
}

void GMM::setFrame(cv::Mat frame) {
  inFrame = frame.clone();
}

// Recorta um retângulo do Frame Original
cv::Mat GMM::crop(cv::Point p1, cv::Point p2) {
  cv::Mat frame = inFrame.clone();
  cv::Rect roi;

  // Ter certeza que clicou na imagem e não fora dela
  if ( p1.x < 0 || p2.x < 0 || p1.y < 0 || p2.y < 0 || p1.x > frame.cols ||
  p2.x > frame.cols || p1.y > frame.rows || p2.y > frame.rows)
    return cv::Mat::zeros(1, 1, CV_32F);

  // Adaptar os pontos para o ROI funcionar
  if (p1.x <= p2.x && p1.y <= p2.y) {
    roi.x = p1.x;
    roi.y = p1.y;
  } else if (p1.x > p2.x && p1.y > p2.y){
    roi.x = p2.x;
    roi.y = p2.y;
  } else if (p1.x <= p2.x && p1.y > p2.y) {
    roi.x = p1.x;
    roi.y = p2.y;
  } else {
    roi.x = p2.x;
    roi.y = p1.y;
  }

  roi.width  = static_cast<int>(sqrt(pow((p1.x - p2.x), 2)));
  roi.height = static_cast<int>(sqrt(pow((p1.y - p2.y), 2)));

  cv::Mat out = frame(roi);

  return out;
}


void GMM::classifyWindows(int index) {
  cv::Mat input = predictWindows(index);
  int rows = input.rows;
  int cols = input.cols;

  partialGaussians[index] = cv::Mat::zeros(rows, cols, CV_8UC3);

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      int label = static_cast<int>(input.at<float>(i, j));
      partialGaussians[index].at<cv::Vec3b>(i, j)[0] = colors[label][0];
      partialGaussians[index].at<cv::Vec3b>(i, j)[1] = colors[label][1];
      partialGaussians[index].at<cv::Vec3b>(i, j)[2] = colors[label][2];
    }
  }
}

// Classifica cada pixel quanto a cor de sua gaussiana
void GMM::classify(int index) {
  cv::Mat input = predict(index);

  int rows = input.rows;
  int cols = input.cols;
  partialFrames[index] = cv::Mat::zeros(rows, cols, CV_8UC3);
  for (int x = 0; x < rows; x++) {
    for (int y = 0; y < cols; y++) {
      int label = static_cast<int>(input.at<float>(x, y));
      partialFrames[index].at<cv::Vec3b>(x, y)[0] = colors[label][0];
      partialFrames[index].at<cv::Vec3b>(x, y)[1] = colors[label][1];
      partialFrames[index].at<cv::Vec3b>(x, y)[2] = colors[label][2];
    }
  }
}

void GMM::paintWindows() {
  for (int k = 0; k < windowsList.size(); k++) {
    partialFinals[k] = cv::Mat::zeros(partialGaussians[k].rows, partialGaussians[k].cols, CV_8UC3);
    partialPreThresholds[k] = cv::Mat::zeros(partialGaussians[k].rows, partialGaussians[k].cols, CV_8UC3);
    for (int i = 0; i < windowsList.at(k).getSize(); i++) {
      for (int j = 0; j < windowsList.at(k).getSize(); j++) {
        int label = static_cast<int>(partialPredicts[k].at<float>(i, j));
        partialFinals[k].at<cv::Vec3b>(i, j)[0] = colors[matchColor.at(label)][0];
        partialFinals[k].at<cv::Vec3b>(i, j)[1] = colors[matchColor.at(label)][1];
        partialFinals[k].at<cv::Vec3b>(i, j)[2] = colors[matchColor.at(label)][2];
        partialPreThresholds[k].at<cv::Vec3b>(i, j)[0] = matchColor.at(label);
        partialPreThresholds[k].at<cv::Vec3b>(i, j)[1] = matchColor.at(label);
        partialPreThresholds[k].at<cv::Vec3b>(i, j)[2] = matchColor.at(label);
      }
    }
  }
}

// Pinta os pixels das gaussianas em suas cores reais
// Prepara o preThreshold para a futura separação dos thresholds de cada cor
void GMM::paint() {
  cv::vconcat(partialPredicts, TOTAL_THREADS, predictFrame);

  finalFrame = cv::Mat::zeros(predictFrame.rows, predictFrame.cols, CV_8UC3);
  preThreshold = cv::Mat::zeros(predictFrame.rows, predictFrame.cols, CV_8UC3);

  for (int x = 0; x < predictFrame.rows; x++) {
    for (int y = 0; y < predictFrame.cols; y++) {
      int label = static_cast<int>(predictFrame.at<float>(x, y));
      finalFrame.at<cv::Vec3b>(x, y)[0] = colors[matchColor.at(label)][0];
      finalFrame.at<cv::Vec3b>(x, y)[1] = colors[matchColor.at(label)][1];
      finalFrame.at<cv::Vec3b>(x, y)[2] = colors[matchColor.at(label)][2];
      preThreshold.at<cv::Vec3b>(x, y)[0] = matchColor.at(label);
      preThreshold.at<cv::Vec3b>(x, y)[1] = matchColor.at(label);
      preThreshold.at<cv::Vec3b>(x, y)[2] = matchColor.at(label);
    }
  }
}

cv::Mat GMM::predictWindows(int index) {
  cv::Mat input = formatFrameForEM(index);

  int windowSize = windowsList.at(index).getSize();

  partialPredicts[index] = cv::Mat(windowSize, windowSize, CV_32F);
  int pixelIndex = 0;

  cv::Mat results;

  for (int i = 0; i < windowSize; i++) {
    for (int j = 0; j < windowSize; j++) {

      double result = em->predict2(input.row(pixelIndex), results)[1];

      partialPredicts[index].at<float>(i, j) = static_cast<float>(result);

      pixelIndex++;
    }
  }
  return partialPredicts[index];
}

// Associa cada pixel com uma gaussiana do modelo
cv::Mat GMM::predict(int index) {
  cv::Mat input = formatFrameForEM(index);

  partialPredicts[index] = cv::Mat(inFrame.rows/TOTAL_THREADS, inFrame.cols, CV_32F);
  int pixelIndex = 0;

  cv::Mat results;

  for (int i = 0; i < inFrame.rows/TOTAL_THREADS; i++) {
    for (int j = 0; j < inFrame.cols; j++) {

      double result = em->predict2(input.row(pixelIndex), results)[1];

      partialPredicts[index].at<float>(i, j) = static_cast<float>(result);

      pixelIndex++;
    }
  }
  return partialPredicts[index];

}

// Formata o Frame para o tipo de variável requerido pelo EM
cv::Mat GMM::formatFrameForEM(int index) {
  if (inFrame.empty()) return cv::Mat::zeros(1, 1, CV_32F);

  cv::Rect roi;

  cv::Mat frame = inFrame.clone();

  if (isDone && checkROIs()) {
    int cols = windowsList.at(index).getSize();
    int rows = windowsList.at(index).getSize();

    roi.x = windowsList.at(index).getX();
    roi.y = windowsList.at(index).getY();
    roi.width = cols;
    roi.height = rows;
  } else {
    int cols = inFrame.cols;
    int rows = inFrame.rows/TOTAL_THREADS;

    roi.x = 0;
    roi.y = index*rows;
    roi.width = cols;
    roi.height = rows;
  }

  cv::Mat dst = frame(roi);

  cv::cvtColor(dst, dst, cv::COLOR_BGR2Lab);

  cv::Mat float_image;
  dst.convertTo(float_image,CV_32F);

  cv::Mat output(dst.rows*dst.cols, 3, CV_32FC1);

  int counter = 0;
  for (int j = 0; j < dst.rows; j++)
  {
	  auto * row = float_image.ptr<cv::Vec3f > (j);

      for (int i = 0; i < dst.cols; i++)
      {
          output.at<cv::Vec3f> (counter++, 0) = row[i];
      }
  }

  return output;
}

// Treina e gera o modelo GMM (com base no EM)
int GMM::train() {

  std::cout << "Training..." << std::endl;
  cv::Mat input = formatSamplesForEM().clone();
  if (input.total() <= 1) return -1;
  em->setClustersNumber(clusters);
  em->setCovarianceMatrixType(cv::ml::EM::COV_MAT_DIAGONAL);
  em->setTermCriteria(cv::TermCriteria(cv::TermCriteria::EPS, 10000, 0.000000001));

  em->trainEM(input);

  std::cout << "Training: Finished" << std::endl;
  means = em->getMeans();
  em->getCovs(covs);
  weights = em->getWeights();
  isTrained = true;

  std::cout << "-------- MEANS" << std::endl;
  std::cout << means << std::endl;

  std::cout << "-------- COVS" << std::endl;
  for (int i = 0; i < covs.size(); i++) {
    std::cout << ">> " << i << std::endl;
    std::cout << covs.at(i) << std::endl;
  }

  std::cout << "-------- WEIGHTS" << std::endl;
  std::cout << weights << std::endl;

  write("autoGMM.json");

  return 0;
}

// Formata as samples para o tipo de variável requerido pelo EM.
cv::Mat GMM::formatSamplesForEM()
{
  if (samples.empty()) {
    std:: cout << "GMM aborted: no samples provided." << std::endl;
    return cv::Mat::zeros(1, 1, CV_32F);
  }
  int totalRows = 0;
  for (auto &sample : samples) {
    totalRows += sample.cols* sample.rows;
  }
  cv::Mat output(totalRows, 3, CV_32FC1);
  int counter = 0;
  for (auto &sample : samples) {
    cv::Mat dst = sample.clone();
    cv::cvtColor(dst, dst, cv::COLOR_BGR2Lab);
    cv::Mat float_image;
    dst.convertTo(float_image,CV_32F);

    //Converting from Float image to Column vector
    for (int j = 0; j < dst.rows; j++)
    {
		auto * row = float_image.ptr<cv::Vec3f > (j);

        for (int i = 0; i < dst.cols; i++)
        {
            output.at<cv::Vec3f> (counter++, 0) = row[i];
        }
    }
  }
  return output;
}

void GMM::pushSample(int points[2][2]) {
  cv::Point p1(points[0][0], points[0][1]);
  cv::Point p2(points[1][0], points[1][1]);

  cv::Mat sample = crop(p1, p2);
  if (sample.cols <= 1 && sample.rows <= 1) {
    std::cout << "Invalid sampled provided: not added to samples vector." << std::endl;
    return;
  }

  samplePoints.push_back(p1);
  samplePoints.push_back(p2);

  samples.push_back(sample);
}

void GMM::popSample() {
  if (!samples.empty()) {
    samplePoints.pop_back();
    samplePoints.pop_back();
    samples.pop_back();
  }
}

void GMM::clearSamples() {
  samples.clear();
  samplePoints.clear();
}

// Carrega a GMM de um arquivo
bool GMM::read(std::string fileName) {

  std::string txtFileName = fileName;
  txtFileName.replace(txtFileName.size()-5, txtFileName.size(), ".txt");

  std::ifstream file;
  file.open(txtFileName);

  if (file.is_open()) {
    std::string line;
    getline(file, line);
    clusters = atoi(line.c_str());
    for (int i = 0; i < TOTAL_COLORS; i++) {
      getline(file, line);
      blur[i] = atoi(line.c_str());
      getline(file, line);
      erode[i] = atoi(line.c_str());
      getline(file, line);
      dilate[i] = atoi(line.c_str());
    }
    for (int &color : matchColor) {
      getline(file, line);
		color = atoi(line.c_str());
    }
    file.close();
  } else {
    std::cout << "GMM::read: Could not open " << txtFileName << ". Maybe it does not exist." << std::endl;
    return false;
  }

  cv::FileStorage fs(fileName, cv::FileStorage::READ);

  if (fs.isOpened()) {
    const cv::FileNode& fn = fs["StatModel.EM"];
    em->read(fn);
    means = em->getMeans();
    em->getCovs(covs);
    weights = em->getWeights();
    isTrained = true;
    std::cout << "-------- MEANS" << std::endl;
    std::cout << means << std::endl;

    std::cout << "-------- COVS" << std::endl;
    for (int i = 0; i < covs.size(); i++) {
      std::cout << ">> " << i << std::endl;
      std::cout << covs.at(i) << std::endl;
    }

    std::cout << "-------- WEIGHTS" << std::endl;
    std::cout << weights << std::endl;
    return true;
  } else {
    std::cout << "GMM::read: Could not open " << fileName << ". Maybe it does not exist." << std::endl;
    return false;
  }
}

// Salva a GMM em um arquivo
bool GMM::write(std::string fileName) {

  std::string txtFileName = fileName;
  txtFileName.replace(txtFileName.size()-5, txtFileName.size(), ".txt");

  std::ofstream file;
  file.open(txtFileName);

  if (file.is_open()) {
    file << clusters <<std::endl;
    for (int i = 0; i < TOTAL_COLORS; i++) {
      file << blur[i] <<std::endl;
      file << erode[i] <<std::endl;
      file << dilate[i] <<std::endl;
    }
    for (int color : matchColor) {
      file << color <<std::endl;
    }
    file.close();
  } else {
    std::cout << "GMM::write: Could not open " << txtFileName << ". Maybe it does not exist." << std::endl;
    return false;
  }

  cv::FileStorage fs(fileName, cv::FileStorage::WRITE);

  if (fileName.empty()) cv::FileStorage fs("autoGMM.json", cv::FileStorage::WRITE);

  if (fs.isOpened()) {
    em->write(fs);
    fs.release();
    return true;
  } else {
    std::cout << "GMM::write: Could not open " << fileName << ". Maybe it does not exist." << std::endl;
    return false;
  }

}

unsigned long GMM::getSamplesSize() {
  return samples.size();
}

std::vector<cv::Point> GMM::getSamplePoints() {
  return samplePoints;
}

// Define o número de gaussianas do modelo
void GMM::setClusters(int k) {
  if (k > 0) clusters = k;
  else std::cout << "GMM::setClusters: invalid value = " << k << std::endl;
}

int GMM::getClusters() {
  return clusters;
}

cv::Mat GMM::getGaussiansFrame() {
  cv::Mat output = gaussiansFrame.clone();
  // cv::pyrUp(output, output);
  // cv::pyrUp(output, output);
  return output;
}

cv::Mat GMM::getFinalFrame() {
  cv::Mat output = finalFrame.clone();
  // cv::pyrUp(output, output);
  // cv::pyrUp(output, output);
  return output;
}

bool GMM::getIsTrained() {
  return isTrained;
}

void GMM::setMatchColor(int gaussian, int color) {
  matchColor.at(gaussian) = color;
}

bool GMM::getDoneFlag() {
  return isDone;
}

std::vector<VisionROI>* GMM::getWindowsList() {
  return &windowsList;
}

void GMM::setDone(bool flag) {
  if (flag) write("autoGMM.json");
  isDone = flag;
}

// Usa o preThreshold para fazer um threshold para cada cor
void GMM::setAllThresholds() {

  for (int i = 0; i < TOTAL_COLORS; i++) {
    threshold_frame.at(i) = cv::Mat::zeros(inFrame.rows, inFrame.cols, CV_8UC3);
  }

  for (int i = 0; i < preThreshold.rows; i++) {
    for (int j = 0; j < preThreshold.cols; j++) {
      int label = preThreshold.at<cv::Vec3b>(i, j)[0];
      if (label > 0 && label < TOTAL_COLORS+1) {
        threshold_frame.at(label-1).at<cv::Vec3b>(i, j)[0] = 255;
        threshold_frame.at(label-1).at<cv::Vec3b>(i, j)[1] = 255;
        threshold_frame.at(label-1).at<cv::Vec3b>(i, j)[2] = 255;
      }
    }
  }
}

cv::Mat GMM::getThresholdFrame(int color) {
  return threshold_frame.at(color);
}

void GMM::setBlur(int index, int value) {
  blur[index] = value;
}

void GMM::setErode(int index, int value) {
  erode[index] = value;
}

void GMM::setDilate(int index, int value) {
  dilate[index] = value;
}

int GMM::getBlur(int index) {
  return blur[index];
}

int GMM::getErode(int index) {
  return erode[index];
}
int GMM::getDilate(int index) {
  return dilate[index];
}

std::vector<cv::Mat> GMM::getAllThresholds() {
  return threshold_frame;
}

bool GMM::checkROIs() {
  for (auto &window : windowsList) {
    if (window.getIsLost()) return false;
  }
  return true;
}

GMM::GMM(int width, int height) : clusters(1), isTrained(false), isDone(false) {
  em = cv::ml::EM::create();

  cv::Mat mat;
  for (int i = 0; i < TOTAL_COLORS; i++) {
    threshold_frame.push_back(mat);
    blur[i] = 0;
    erode[i] = 0;
    dilate[i] = 0;
  }

  for (int i = 0; i < TOTAL_WINDOWS; i++) {
    VisionROI roi(width, height, 60, i);
    roi.setPosition(0,0);
    windowsList.push_back(roi);
  }
}

GMM::~GMM() = default;
