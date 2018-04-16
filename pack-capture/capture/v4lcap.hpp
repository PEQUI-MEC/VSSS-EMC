/*
 * v4l_device.hpp
 *
 *  Created on: Jan 27, 2014
 *      Author: gustavo
 */

#ifndef V4LCAP_HPP_
#define V4LCAP_HPP_

#include <linux/videodev2.h>
#include <libv4lconvert.h>

namespace capture {

	class v4lcap {
		public:

			enum memory_type {
				MEM_READ, MEM_MMAP, MEM_USER
			};
		public:

			int fd;
			bool emulate_format;
			bool needs_convert;
			struct v4l2_format format_src, format_dest;
			struct v4lconvert_data *v4lconv;
			struct v4l2_requestbuffers reqbuf;

			enum device_status {
				OPENED = 1 << 0, MMAP_INIT = 1 << 1, STREAM_ON = 1 << 2
			};
			unsigned int status;

			struct buffer {
				void *start;
				long int length;
			};
			struct buffer_holder {
				int qtd;
				struct buffer *buffs;
			};
			struct buffer_holder buffers;

			int xioctl(int fd, int request, void *arg);

		public:
			v4lcap();

			~v4lcap() {
				close_device();
			}

		public:
			/* 			 */
			static bool enum_device_name(char device[16], int ndev_max, bool init = false);
			bool open_device(const char *devname, bool emulate_fmt);
			bool start_capturing(enum memory_type mem_type, int nbuffers);
			bool start_capturing();

			bool init_mmap(int nbuffers);

		public:
			/* configuration */
			bool set_input(int index);
			bool set_standard(v4l2_std_id std_id);
			bool set_pixel_format(unsigned int pixel_format);
			bool set_frame_size(int width, int height, unsigned int type);
			bool set_frame_interval(struct v4l2_fract &fract);
			bool set_control(int ctrl_id, int value);

		public:

			bool streamon(enum v4l2_buf_type type);
			bool streamon(int type);

			void *grab_raw(unsigned char *raw);
			void *grab_rgb(unsigned char *rgb);

			bool streamoff(enum v4l2_buf_type type);
			bool streamoff(int type);
			bool stop_capturing();
			bool uninit_mmap();
			bool close_device();

			bool is_opened();

			bool query_capability(struct v4l2_capability *cap);
			bool get_format(struct v4l2_format *fmt, unsigned int type);
			bool get_format(struct v4l2_format *fmt);
			bool get_input_index(unsigned int *input);
			bool get_input(struct v4l2_input *input);
			bool get_standard_id(v4l2_std_id *std_id);
			bool get_standard(struct v4l2_standard *standard);
			bool get_stream_parameter(struct v4l2_streamparm *streamparm, unsigned int type);
			bool get_control(struct v4l2_control *ctrl, unsigned int ctrl_id);
			bool query_control(struct v4l2_queryctrl *qctrl);
			bool query_control_menu(struct v4l2_querymenu *qmenu);

			bool request_buffers(struct v4l2_requestbuffers *req_buff, int nbuffers, enum v4l2_memory memory,
								 enum v4l2_buf_type type);
			bool request_buffers(struct v4l2_requestbuffers *req_buff);
			bool request_buffers_mmap(struct v4l2_requestbuffers *req_buff, int nbuffers);
			bool query_buffer(struct v4l2_buffer *buffer);
			bool query_buffer(struct v4l2_buffer *buf, unsigned int type, unsigned int memory, unsigned int index);
			bool
			query_buffer(struct v4l2_buffer *buf, enum v4l2_buf_type type, enum v4l2_memory memory, unsigned int index);

			bool enqueue_buff(struct v4l2_buffer *vbuf);
			bool
			enqueue_buff(struct v4l2_buffer *vbuf, enum v4l2_buf_type buf_type, enum v4l2_memory memory, int index);
			bool enqueue_buff(struct v4l2_buffer *vbuf, unsigned int buf_type, unsigned int memory, int index);

			bool dequeue_buff(struct v4l2_buffer *vbuf);
			bool
			dequeue_buff(struct v4l2_buffer *vbuf, enum v4l2_buf_type buf_type, enum v4l2_memory memory, bool *again);
			bool dequeue_buff(struct v4l2_buffer *vbuf, unsigned int buf_type, unsigned int memory, bool *again);

			bool init_mmap(struct v4l2_requestbuffers *req_buff);

			/**
			 * ioctl VIDIOC_ENUMAUDIO — Enumerate audio inputs
			 */
			bool enum_audio_input(struct v4l2_audio *audio, int index = 0, bool init = false);
			/**
			 * ioctl VIDIOC_ENUMAUDOUT — Enumerate audio outputs
			 */
			bool enum_audio_output(struct v4l2_audioout *audiout, int index = 0, bool init = false);
			/**
			 * ioctl VIDIOC_ENUM_DV_TIMINGS — Enumerate supported Digital Video timings
			 */
			bool enum_dv_timings(...);
			/**
			 * 	ioctl VIDIOC_ENUM_FMT — Enumerate image formats
			 */
			bool enum_image_format(int fd, struct v4l2_fmtdesc *fmtdesc);
			bool enum_image_format(struct v4l2_fmtdesc *fmtdesc, int index = 0, bool init = false);
			/**
			 * ioctl VIDIOC_ENUM_FRAMESIZES — Enumerate frame sizes
			 */
			bool enum_frame_size(struct v4l2_frmsizeenum *frmsize, int pixel_format, int index = 0, bool init = false);
			/**
			 * ioctl VIDIOC_ENUM_FRAMEINTERVALS — Enumerate frame intervals
			 */
			bool
			enum_frame_interval(struct v4l2_frmivalenum *frmi, int pixel_format, int width, int height, int index = 0,
								bool init = false);
			/**
			 * ioctl VIDIOC_ENUM_FREQ_BANDS — Enumerate supported frequency bands
			 */
			bool enum_video_freq_bands(...);
			/**
			 * ioctl VIDIOC_ENUMINPUT — Enumerate video inputs
			 */
			bool enum_video_input(struct v4l2_input *input, int index = 0, bool init = false);
			/**
			 * ioctl VIDIOC_ENUMOUTPUT — Enumerate video outputs
			 */
			bool enum_video_output(struct v4l2_output *output, int index = 0, bool init = false);
			/**
			 * ioctl VIDIOC_ENUMSTD — Enumerate supported vid
			 */
			bool enum_video_standard(struct v4l2_standard *std, int index = 0, bool init = false);

			bool enum_control_user_base(struct v4l2_queryctrl *qctrl, bool init = false);
			bool enum_control_private_base(struct v4l2_queryctrl *qctrl, bool init = false);
			bool enum_control_default(struct v4l2_queryctrl *qctrl, bool init = false);
			bool show_controls();
			bool
			enum_control_menu(struct v4l2_querymenu *qmenu, v4l2_queryctrl &ctrl, int index = 0, bool init = false);

			bool set_format(struct v4l2_format *fmt);
			//bool set_format(int pixel_format, int width, int height);

	};
}

#endif /* V4LCAP_HPP_ */
