#include "VideoRecorder.hpp"

using namespace rec;

void VideoRecorder::run(cv::Mat in_frame) {
	frame = in_frame.clone();
	if (is_on_air) record_to_video();
}

void VideoRecorder::save_picture(std::string in_name) {
	std::string picName = "media/pictures/" + in_name + ".png";

	cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
	cv::imwrite(picName, frame);
}

void VideoRecorder::start_new_video(std::string in_name) {
	std::string videoName = "media/videos/" + in_name + ".avi";

	video.open(videoName, CV_FOURCC('M', 'J', 'P', 'G'), 30, cv::Size(width, height));
	std::cout << "Started a new video recording." << std::endl;
	is_on_air = true;
}

bool VideoRecorder::record_to_video() {
	if (!video.isOpened()) return false;
	cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);

	video.write(frame);
	return true;
}

bool VideoRecorder::finish_video() {
	if (!video.isOpened()) return false;

	std::cout << "Finished video recording." << std::endl;
	video.release();
	is_on_air = false;
	return true;
}

VideoRecorder::~VideoRecorder() {
	if (is_recording())
		finish_video();
}
