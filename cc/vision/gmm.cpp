#include "gmm.hpp"

void GMM::setFrame(cv::Mat frame) {
  inFrame = frame.clone();
}

cv::Mat GMM::crop(cv::Point p1, cv::Point p2) {
  cv::Mat frame = inFrame.clone();
  cv::Rect roi;

  if ( p1.x < 0 || p2.x < 0 || p1.y < 0 || p2.y < 0 || p1.x > frame.cols ||
  p2.x > frame.cols || p1.y > frame.rows || p2.y > frame.rows)
    return cv::Mat::zeros(1, 1, CV_32F);

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

  roi.width  = sqrt(pow((p1.x - p2.x),2));
  roi.height = sqrt(pow((p1.y - p2.y),2));

  cv::Mat out = frame(roi);

  return out;
}

cv::Mat GMM::classify() {
  cv::Mat input = predict();
	cv::Mat outputFrame = cv::Mat::zeros(input.rows, input.cols, CV_8UC3);
	for (int x = 0; x < input.rows; x++) {
		for (int y = 0; y < input.cols; y++) {
			int label = input.at<float>(x,y);
			outputFrame.at<cv::Vec3b>(x, y)[0] = colors[label][0];
			outputFrame.at<cv::Vec3b>(x, y)[1] = colors[label][1];
			outputFrame.at<cv::Vec3b>(x, y)[2] = colors[label][2];
		}
	}
  return outputFrame;
}

void GMM::run(cv::Mat frame) {
  inFrame = frame.clone();
  // cv::pyrDown(inFrame, inFrame);
  // cv::pyrDown(inFrame, inFrame);
  gaussiansFrame = classify();
  finalFrame = gaussiansFrame.clone();
}

cv::Mat GMM::predict() {
  cv::Mat input = formatFrameForEM().clone();
  input.convertTo(input,CV_32FC1);
  cv::Mat output = cv::Mat(inFrame.rows, inFrame.cols, CV_32F);

  int index = 0;
  // std::cout << "------ SAMPLES" << std::endl;
  // std::cout << input << std::endl;
  cv::Mat results;
  // std::cout << "----- PREDICT" << std::endl;
  for (int i = 0; i < inFrame.rows; i++) {
    for (int j = 0; j < inFrame.cols; j++) {
      double result = em->predict2(input.row(index), results)[1];
      // std::cout << input.row(index) << std::endl;
      // std::cout << results << std::endl;
      output.at<float>(i, j) = result;
      index++;
    }
  }

  // std::cout << "------ OUTPUT" << std::endl;
  // std::cout << output << std::endl;

  return output;
}

cv::Mat GMM::formatFrameForEM() {
  if (inFrame.empty()) return cv::Mat::zeros(1, 1, CV_32F);

  cv::Mat output(inFrame.cols*inFrame.rows, 3, CV_32FC1);

  cv::Mat dst = inFrame.clone();
  cv::cvtColor(dst, dst, cv::COLOR_BGR2HSV);

  cv::Mat float_image;
  dst.convertTo(float_image,CV_32F);

  int counter = 0;
  for (int j = 0; j < dst.rows; j++)
  {
      cv::Vec3f* row = float_image.ptr<cv::Vec3f > (j);

      for (int i = 0; i < dst.cols; i++)
      {
          output.at<cv::Vec3f> (counter++, 0) = row[i];
      }
  }

  return output;
}

int GMM::train() {
  cv::FileStorage fs("gmm.json", cv::FileStorage::WRITE );
  cv::Mat probs, loglikelihoods, labels;
  std::cout << "Training..." << std::endl;
  cv::Mat input = formatSamplesForEM().clone();
  if (input.total() <= 1) return -1;
  // em = cv::ml::EM::create();
  em->setClustersNumber(clusters);
  em->setCovarianceMatrixType(cv::ml::EM::COV_MAT_DIAGONAL);
  em->setTermCriteria(cv::TermCriteria(cv::TermCriteria::EPS, 10000, 0.000000001));
  // em->trainE(input, initMeans);
  em->trainEM(input, loglikelihoods, labels, probs);
  em->write(fs);
  fs.release();
  std::cout << "Training: Finished" << std::endl;
  // std::cout << "------ LOGLIKELIHOODS" << std::endl;
  // std::cout << loglikelihoods << std::endl;
  // std::cout << "------ LABELS" << std::endl;
  // std::cout << labels << std::endl;
  // std::cout << "------ PROBS" << std::endl;
  // std::cout << probs << std::endl;
  means = em->getMeans();
  em->getCovs(covs);
  weights = em->getWeights();
  // cv::Mat results;
  // em->predict(input, results);
  // std::cout << "----- Resultados" << std::endl;
  // std::cout << results << std::endl;
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

  return 0;
}

cv::Mat GMM::formatSamplesForEM()
{
  if (samples.empty()) {
    std:: cout << "GMM aborted: no samples provided." << std::endl;
    return cv::Mat::zeros(1, 1, CV_32F);
  }
  int totalRows = 0;
  for (int i = 0; i < samples.size(); i++) {
    totalRows += samples.at(i).cols*samples.at(i).rows;
  }
  cv::Mat output(totalRows, 3, CV_32FC1);
  int counter = 0;
  for (int k = 0; k < samples.size(); k++) {
    cv::Mat dst = samples.at(k).clone();
    cv::cvtColor(dst, dst, cv::COLOR_BGR2HSV);
    cv::Mat float_image;
    dst.convertTo(float_image,CV_32F);

    //Converting from Float image to Column vector
    for (int j = 0; j < dst.rows; j++)
    {
        cv::Vec3f* row = float_image.ptr<cv::Vec3f > (j);

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

  // cv::imwrite("sample.png", sample);

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

int GMM::getSamplesSize() {
  return samples.size();
}

std::vector<cv::Point> GMM::getSamplePoints() {
  return samplePoints;
}

void GMM::setClusters(int k) {
  if (k > 0) clusters = k;
  else std::cout << "GMM::setClusters: invalid value = " << k << std::endl;
}

int GMM::getClusters() {
  return clusters;
}

cv::Mat GMM::getGaussiansFrame() {
  return gaussiansFrame;
}

cv::Mat GMM::getFinalFrame() {
  return finalFrame;
}

bool GMM::getIsTrained() {
  return isTrained;
}

GMM::GMM() : clusters(1), isTrained(false) {
  em = cv::ml::EM::create();
}

GMM::~GMM() {

}
