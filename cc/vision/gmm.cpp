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

void GMM::classify(int index) {
  // std::cout << "CLASSIFY 1" << std::endl;
  cv::Mat input = predict(index);
  // std::cout << "CLASSIFY 2" << std::endl;
  int rows = input.rows;
  int cols = input.cols;
  // std::cout << "input rows = " << input.rows << ", input cols = " << input.cols << std::endl;
	partialFrames[index] = cv::Mat::zeros(rows, cols, CV_8UC3);
  // std::cout << "CLASSIFY 3" << std::endl;
  // std::cout << "index = " << index << std::endl;
	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < cols; y++) {
      // std::cout << "CLASSIFY 3.1 ("  << x << ", " << y << ")" << std::endl;
			int label = input.at<float>(x,y);
      // std::cout << "CLASSIFY 3.2 ("  << x << ", " << y << ")" << std::endl;
			partialFrames[index].at<cv::Vec3b>(x, y)[0] = colors[label][0];
      // std::cout << "CLASSIFY 3.3 ("  << x << ", " << y << ")" << std::endl;
			partialFrames[index].at<cv::Vec3b>(x, y)[1] = colors[label][1];
      // std::cout << "CLASSIFY 3.4 ("  << x << ", " << y << ")" << std::endl;
			partialFrames[index].at<cv::Vec3b>(x, y)[2] = colors[label][2];
      // std::cout << "CLASSIFY 3.5 ("  << x << ", " << y << ")" << std::endl;
		}
	}

  // std::cout << "CLASSIFY 4" << std::endl;
}

void GMM::run(cv::Mat frame) {
  inFrame = frame.clone();
  // cv::pyrDown(inFrame, inFrame);
  // cv::pyrDown(inFrame, inFrame);
  for (int i = 0; i < TOTAL_THREADS; i++) {
    threads.add_thread(new boost::thread(&GMM::classify, this, i));
    // classify(i);
  }
  threads.join_all();

  // gaussiansFrame = cv::Mat(partialFrames.at(0).rows, partialFrames.at(0).cols, CV_8UC3, partialFrames.at(0).data);
  cv::Mat emptyMat;
  gaussiansFrame = emptyMat;
  cv::vconcat(partialFrames, TOTAL_THREADS, gaussiansFrame);

  // std::cout << "GAUSSIANS: rows = " << gaussiansFrame.rows << ", cols = " << gaussiansFrame.cols << std::endl;
  // std::cout << "ORIGINAL: rows = " << inFrame.rows << ", cols = " << inFrame.cols << std::endl;
  finalFrame = gaussiansFrame.clone();
}

cv::Mat GMM::predict(int threadIndex) {
  // std::cout << "PREDICT 1" << std::endl;
  cv::Mat input = formatFrameForEM(threadIndex);
  // std::cout << "input rows = " << input.rows << ", input cols = " << input.cols << std::endl;
  cv::Mat output = cv::Mat(inFrame.rows/TOTAL_THREADS, inFrame.cols, CV_32F);
  // std::cout << "output rows = " << output.rows << ", output cols = " << output.cols << std::endl;
  // std::cout << "PREDICT 2" << std::endl;
  int pixelIndex = 0;
  // std::cout << "------ SAMPLES" << std::endl;
  // std::cout << input << std::endl;
  cv::Mat results;
  // std::cout << "----- PREDICT" << std::endl;
  for (int i = 0; i < inFrame.rows/TOTAL_THREADS; i++) {
    for (int j = 0; j < inFrame.cols; j++) {
      // std::cout << "PREDICT FOR (" << i << ", " << j << ")" << std::endl;
      // std::cout <<  "pixelIndex = " << pixelIndex << std::endl;
      double result = em->predict2(input.row(pixelIndex), results)[1];
      // std::cout << input.row(pixelIndex) << std::endl;
      // std::cout << results << std::endl;
      // std::cout << "INSERT TRY" << std::endl;
      output.at<float>(i, j) = result;
      // std::cout << "INSERT DONE" << std::endl;
      pixelIndex++;
    }
  }
  // std::cout << "PREDICT 3" << std::endl;
  // std::cout << "------ OUTPUT" << std::endl;
  // std::cout << output << std::endl;

  return output;
}

cv::Mat GMM::formatFrameForEM(int index) {
  if (inFrame.empty()) return cv::Mat::zeros(1, 1, CV_32F);

  int cols = inFrame.cols;
  int rows = inFrame.rows/TOTAL_THREADS;

  // std::cout << "rows = " << rows << ", cols = " << cols << std::endl;



  cv::Rect roi;
  roi.x = 0;
  roi.y = index*rows;
  roi.width = cols;
  roi.height = rows;

  // std::cout << "[1]: " << roi.x << ", " << roi.y << std::endl;
  // std::cout << "[2]: " <<  roi.width << ", " << roi.height << std::endl;

  cv::Mat dst = inFrame(roi);

  // std::cout << "dst rows = " << dst.rows << ", dst cols = " << dst.cols << std::endl;

  cv::cvtColor(dst, dst, cv::COLOR_BGR2HSV);

  // std::cout << "FORMAT FRAME 1" << std::endl;

  cv::Mat float_image;
  dst.convertTo(float_image,CV_32F);

  // std::cout << "FORMAT FRAME 2" << std::endl;
  cv::Mat output(dst.rows*dst.cols, 3, CV_32FC1);

  int counter = 0;
  for (int j = 0; j < dst.rows; j++)
  {
      cv::Vec3f* row = float_image.ptr<cv::Vec3f > (j);

      for (int i = 0; i < dst.cols; i++)
      {
          output.at<cv::Vec3f> (counter++, 0) = row[i];
      }
  }

  // std::cout << "FORMAT FRAME 3" << std::endl;

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
