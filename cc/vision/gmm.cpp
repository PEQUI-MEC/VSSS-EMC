#include "gmm.hpp"

void GMM::setFrame(cv::Mat frame) {
  inFrame = frame.clone();
}

cv::Mat GMM::crop(cv::Point p1, cv::Point p2) {
  cv::Mat frame = inFrame.clone();
  cv::Rect roi;

  if ( p1.x < 0 || p2.x < 0 || p1.y < 0 || p2.y < 0 || p1.x > frame.rows ||
  p2.x > frame.rows || p1.y > frame.cols || p2.y > frame.cols)
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

void GMM::pushSample(int points[2][2]) {
  cv::Point p1(points[0][0], points[0][1]);
  cv::Point p2(points[1][0], points[1][1]);

  cv::Mat sample = crop(p1, p2);
  if (sample.cols <= 1 && sample.rows <= 1) {
    std::cout << "Invalid sampled provided: not added to samples vector." << std::endl;
    return;
  }

  // cv::imwrite("sample.png", sample);

  samples.push_back(sample);
}

void GMM::popSample() {
  if (!samples.empty()) samples.pop_back();
}

void GMM::clearSamples() {
  samples.clear();
}

int GMM::getSamplesSize() {
  return samples.size();
}

GMM::GMM() {

}

GMM::~GMM() {

}
