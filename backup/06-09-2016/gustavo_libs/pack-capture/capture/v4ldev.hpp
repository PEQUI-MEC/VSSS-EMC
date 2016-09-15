/*
 * v4ldev.hpp
 *
 *  Created on: Feb 20, 2014
 *      Author: gustavo
 */

#ifndef V4LDEV_HPP_
#define V4LDEV_HPP_

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <libv4l2.h>
#include <linux/videodev2.h>

#include <string.h> /* menset */
#include <iostream> /* cout, endl, ... */

class v4ldev {

		int fd;
		bool emulate_format;

	public:
		v4ldev();
		~v4ldev();

	private:
		int xioctl(int fd, int request, void * arg) {
			int r;
			if (emulate_format) r = v4l2_ioctl(fd, request, arg);
			else r = ioctl(fd, request, arg);
			return r;
		}

	public:
		bool open_device(const char * devname, bool emulate_fmt) {
			fd = open(devname, O_RDWR);
			emulate_format = emulate_fmt;
			if (fd < 0) {
				std::cout << "Can not open " << devname << "device!" << std::endl;
				return false;
			}

			if (emulate_fmt) {
				int fd2 = v4l2_fd_open(fd, V4L2_ENABLE_ENUM_FMT_EMULATION);
				if (fd2 < 0) {
					std::cout << "Cannot use libv4l2 wrapper for " << devname << "!" << std::endl;
					emulate_fmt = false;
				}
			}

			struct v4l2_capability cap;
			if (!query_capability(&cap)) {
				std::cout << devname << " is not a V4L2 device!" << std::endl;
				if (emulate_fmt) v4l2_close(fd);
				else close(fd);
				return false;
			}
			return true;
		}

		/* IOCTLs */
		bool query_capability(struct v4l2_capability * cap) {
			return xioctl(fd, VIDIOC_QUERYCAP, cap) >= 0;
		}

		bool get_input_index(int * input) {
			return xioctl(fd, VIDIOC_G_INPUT, input) >= 0;
		}
		bool get_input(struct v4l2_input * input) {
			int index;
			if (!get_input_index(&index)) return false;
			input->index = index;
			return xioctl(fd, VIDIOC_ENUMINPUT, input) >= 0;
		}
		bool get_format(struct v4l2_format * fmt, enum v4l2_buf_type type) {
			fmt->type = type;
			return xioctl(fd, VIDIOC_G_FMT, fmt) >= 0;
		}
		bool get_standard_id(v4l2_std_id * std_id) {
			return xioctl(fd, VIDIOC_G_STD, std_id) >= 0;
		}
		bool get_standard(struct v4l2_standard * standard) {
			v4l2_std_id std_id;
			if (!get_standard_id(&std_id)) return false;
			standard->id = std_id;
			for (__u32 index = 0;; ++index) {
				standard->index = index;
				if (standard->id == std_id) return true;
				if (!xioctl(fd, VIDIOC_ENUMSTD, standard)) break;
			}
			return false;
		}
		bool get_stream_parameter(struct v4l2_streamparm * streamparm) {
			return xioctl(fd, VIDIOC_G_PARM, streamparm) >= 0;
		}
		bool get_stream_parameter(struct v4l2_streamparm * streamparm, __u32 type) {
			streamparm->type = type;
			return get_stream_parameter(streamparm);
		}
		bool get_control(struct v4l2_control * ctrl) {
			return xioctl(fd, VIDIOC_G_CTRL, ctrl) >= 0;
		}
		bool get_control(struct v4l2_control * ctrl, unsigned int ctrl_id) {
			ctrl->id = ctrl_id;
			return get_control(ctrl);
		}

		bool query_buffer(struct v4l2_buffer * buffer) {
			int r = xioctl(fd, VIDIOC_QUERYBUF, buffer);
			if (r >= 0) return true;
			perror("VIDIOC_QUERYBUF");
			return false;
		}
		bool query_buffer(struct v4l2_buffer * buf, enum v4l2_buf_type type, enum v4l2_memory memory, unsigned int index) {
			buf->type = type;
			buf->memory = memory;
			buf->index = index;
			return query_buffer(buf);
		}

		bool enqueue_buff(struct v4l2_buffer * vbuf) {
			return xioctl(fd, VIDIOC_QBUF, vbuf) >= 0;
		}
		bool enqueue_buff(struct v4l2_buffer * vbuf, enum v4l2_buf_type buf_type, enum v4l2_memory memory, int index) {
			memset(vbuf, 0, sizeof(*vbuf));
			vbuf->type = buf_type;
			vbuf->memory = memory;
			vbuf->index = index;
			return enqueue_buff(vbuf);
		}

		bool streamon(enum v4l2_buf_type type) {
			return xioctl(fd, VIDIOC_STREAMON, &type) >= 0;
		}
		bool streamoff(enum v4l2_buf_type type) {
			return xioctl(fd, VIDIOC_STREAMOFF, &type) >= 0;
		}

		bool dequeue_buff(struct v4l2_buffer * vbuf) {
			return xioctl(fd, VIDIOC_DQBUF, vbuf) >= 0;
		}
		bool dequeue_buff(struct v4l2_buffer * vbuf, unsigned int buf_type, unsigned int memory, bool * again) {
			memset(vbuf, 0, sizeof(struct v4l2_buffer));
			vbuf->type = buf_type;
			vbuf->memory = memory;
			int r = dequeue_buff(vbuf);

			// if the buffer is not avaliable, try again.
			*again = (r < 0) && (errno == EAGAIN);
			return (r >= 0) || (*again);
		}

		bool query_control(struct v4l2_queryctrl * qctrl) {
			return xioctl(fd, VIDIOC_QUERYCTRL, qctrl) >= 0;
		}
		bool query_control_menu(struct v4l2_querymenu * qmenu) {
			return xioctl(fd, VIDIOC_QUERYMENU, qmenu) >= 0;
		}

		bool request_buffers(struct v4l2_requestbuffers * req_buff) {
			return xioctl(fd, VIDIOC_REQBUFS, req_buff) >= 0;
		}
		bool request_buffers(struct v4l2_requestbuffers * req_buff, int nbuffers, enum v4l2_memory memory, enum v4l2_buf_type type) {
			req_buff->reserved[0] = 0;
			req_buff->reserved[1] = 0;
			req_buff->count = nbuffers;
			req_buff->memory = memory;
			req_buff->type = type;
			return request_buffers(req_buff);

		}
};

#endif /* V4LDEV_HPP_ */
