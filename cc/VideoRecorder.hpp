#ifndef VSSS_VIDEORECORDER_HPP
#define VSSS_VIDEORECORDER_HPP

#include <opencv2/opencv.hpp>

namespace rec {
	class VideoRecorder {
		private:
			cv::Mat frame;
			// video
			cv::VideoWriter video;

			int width, height;

			// record video flag
			bool is_on_air;

		public:
			VideoRecorder(int w, int h) : width(w), height(h), is_on_air(false) {};
			~VideoRecorder();
			bool record_to_video();
			void run(cv::Mat frame);
			bool finish_video();

			bool is_recording() const { return is_on_air; };

			void start_new_video(const std::string &in_name);
			void save_picture(const std::string &in_name);
	};
}

#endif //VSSS_VIDEORECORDER_HPP
