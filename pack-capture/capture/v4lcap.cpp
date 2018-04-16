/*
 * v4l_device.cpp
 *
 *  Created on: Jan 27, 2014
 *      Author: gustavo
 */

#include "v4lcap.hpp"

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libv4l2.h>
#include <linux/videodev2.h>

#include <iostream>

using namespace std;

namespace capture {

	v4lcap::v4lcap() :
			fd(0), emulate_format(true), needs_convert(false), v4lconv(NULL), status(0) {
	}

	int v4lcap::xioctl(int fd, int request, void *arg) {
		int r;
		if (emulate_format) r = v4l2_ioctl(fd, request, arg);
		else r = ioctl(fd, request, arg);
		return r;
	}

	bool v4lcap::is_opened() {
		return fd > 0;
	}

	bool v4lcap::enum_device_name(char device[16], int ndev_max, bool init) {
		static int __dev_count = 0;
		bool not_found = true;
		struct stat st;
		if (init) __dev_count = 0;
		while (not_found && __dev_count < ndev_max) {
			snprintf(device, 15, "/dev/video%d", __dev_count);
			++__dev_count;
			if (stat(device, &st) != 0) continue;
			not_found = false;
		}
		return !not_found;
	}

	bool v4lcap::open_device(const char *devname, bool emulate_fmt) {
		fd = open(devname, O_RDWR);
		this->emulate_format = emulate_fmt;
		if (fd < 0) {
			cout << "Can not open " << devname << "device!" << endl;
			return false;
		}

		if (emulate_fmt) {
			int fd2 = v4l2_fd_open(fd, V4L2_ENABLE_ENUM_FMT_EMULATION);
			if (fd2 < 0) {
				cout << "Cannot use libv4l2 wrapper for " << devname << "!" << endl;
				emulate_fmt = false;
			}
		}

		struct v4l2_capability cap;
		if (!query_capability(&cap)) {
			cout << devname << " is not a V4L2 device!" << endl;
			if (emulate_fmt) v4l2_close(fd);
			else close(fd);
			return false;
		}

		v4lconv = v4lconvert_create(fd);
		if (!v4lconv) {
			close_device();
			return false;
		}

		status |= OPENED;
		return true;
	}

	bool v4lcap::query_buffer(struct v4l2_buffer *buf, enum v4l2_buf_type type, enum v4l2_memory memory,
							  unsigned int index) {
		buf->type = type;
		buf->memory = memory;
		buf->index = index;
		return query_buffer(buf);
	}

	bool v4lcap::query_buffer(struct v4l2_buffer *buf, unsigned int type, unsigned int memory, unsigned int index) {
		buf->type = type;
		buf->memory = memory;
		buf->index = index;
		return query_buffer(buf);
	}

	bool v4lcap::query_buffer(struct v4l2_buffer *buffer) {
		int r = xioctl(fd, VIDIOC_QUERYBUF, buffer);
		if (r >= 0) return true;
		perror("VIDIOC_QUERYBUF");
		return false;
	}

	bool v4lcap::enqueue_buff(struct v4l2_buffer *vbuf) {
		return xioctl(fd, VIDIOC_QBUF, vbuf) >= 0;
	}

	bool
	v4lcap::enqueue_buff(struct v4l2_buffer *vbuf, enum v4l2_buf_type buf_type, enum v4l2_memory memory, int index) {
		memset(vbuf, 0, sizeof(*vbuf));
		vbuf->type = buf_type;
		vbuf->memory = memory;
		vbuf->index = index;
		return enqueue_buff(vbuf);
	}

	bool v4lcap::enqueue_buff(struct v4l2_buffer *vbuf, unsigned int buf_type, unsigned int memory, int index) {
		memset(vbuf, 0, sizeof(*vbuf));
		vbuf->type = buf_type;
		vbuf->memory = memory;
		vbuf->index = index;
		return enqueue_buff(vbuf);
	}

	bool v4lcap::dequeue_buff(struct v4l2_buffer *vbuf) {
		return xioctl(fd, VIDIOC_DQBUF, vbuf) >= 0;
	}

	bool v4lcap::dequeue_buff(struct v4l2_buffer *vbuf, unsigned int buf_type, unsigned int memory, bool *again) {
		memset(vbuf, 0, sizeof(struct v4l2_buffer));
		vbuf->type = buf_type;
		vbuf->memory = memory;
		int r = dequeue_buff(vbuf);

		// if the buffer is not avaliable, try again.
		*again = (r < 0) && (errno == EAGAIN);
		return (r >= 0) || (*again);
	}

	bool
	v4lcap::dequeue_buff(struct v4l2_buffer *vbuf, enum v4l2_buf_type buf_type, enum v4l2_memory memory, bool *again) {
		memset(vbuf, 0, sizeof(struct v4l2_buffer));
		vbuf->type = buf_type;
		vbuf->memory = memory;
		int r = dequeue_buff(vbuf);

		// if the buffer is not avaliable, try again.
		*again = (r < 0) && (errno == EAGAIN);
		return (r >= 0) || (*again);
	}

	bool v4lcap::init_mmap(int nbuffers) {

		// = Get current format ====================================================
		if (!get_format(&format_src, V4L2_BUF_TYPE_VIDEO_CAPTURE)) return false;

		// = Request buffers by mmap ===============================================
		if (!request_buffers_mmap(&reqbuf, nbuffers)) {
			std::cout << "Can't request mmap buffers.\n";
			return false;
		}

		if (reqbuf.count < 2) {
			// = Request 0 buffers to clean memory driver ==========================
			request_buffers_mmap(&reqbuf, 0);
			std::cout << "Insufficient buffer memory.\n";
			return false;
		}

		// = Alloc buffer holders ==================================================
		buffers.qtd = reqbuf.count;
		buffers.buffs = (struct buffer *) calloc(reqbuf.count, sizeof(struct buffer));
		if (!buffers.buffs) return false;

		// = Memory map: mmap ======================================================
		for (unsigned int i = 0; i < reqbuf.count; ++i) {

			struct v4l2_buffer buffer = {0};

			if (!query_buffer(&buffer, V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_MEMORY_MMAP, i)) {
				std::cout << "Can't request buffer " << i << std::endl;
			}

			buffers.buffs[i].length = buffer.length; /* remember for munmap() */

			if (emulate_format) {

				buffers.buffs[i].start = v4l2_mmap(NULL, //
												   buffer.length, //
												   PROT_READ | PROT_WRITE, /* recommended */
												   MAP_SHARED, /* recommended */
												   fd, //
												   buffer.m.offset);
			} else {
				buffers.buffs[i].start = mmap(NULL, //
											  buffer.length, //
											  PROT_READ | PROT_WRITE, /* recommended */
											  MAP_SHARED, /* recommended */
											  fd, //
											  buffer.m.offset);
			}

			if (MAP_FAILED == buffers.buffs[i].start) {
				/* If you do not exit here you should unmap() and free() the buffers mapped so far. */
				perror("mmap");
				exit(EXIT_FAILURE);
			}

//			//////////////////////////////
//			if (!query_buffer(&buffer, reqbuf.type, reqbuf.memory, i)) {
//				std::cout << "Can't request buffer " << i << std::endl;
//			}
//			if (buffer.flags & V4L2_BUF_FLAG_MAPPED) {
//				std::cout << "Buffer " << i << " mapped!" << std::endl;
//			} else {
//				std::cout << "Buffer " << i << " NOT mapped!" << std::endl;
//			}

		}
		return true;
	}

	bool v4lcap::streamon(enum v4l2_buf_type type) {
		return xioctl(fd, VIDIOC_STREAMON, &type) >= 0;
	}

	bool v4lcap::streamon(int type) {
		return streamon((enum v4l2_buf_type) type);
	}

	bool v4lcap::start_capturing(enum memory_type mem_type, int nbuffers) {

		switch (mem_type) {
			case MEM_MMAP:
				if (!init_mmap(nbuffers)) return false;
				break;
			case MEM_USER:
			case MEM_READ:
			default:
				return false;
		}

		if (!init_mmap(nbuffers)) return false;

		// = Enqueue all buffers ===================================================
		for (unsigned int i = 0; i < reqbuf.count; ++i) {
			struct v4l2_buffer buffer = {0};
			if (!enqueue_buff(&buffer, reqbuf.type, reqbuf.memory, i)) {
				std::cout << "Can't enqueue buffer " << i << std::endl;
			}
			//////////////////////////////
			if (!query_buffer(&buffer, reqbuf.type, reqbuf.memory, i)) {
				std::cout << "Can't request buffer " << i << std::endl;
			}
			if (buffer.flags & V4L2_BUF_FLAG_QUEUED) {
				std::cout << "Buffer " << i << " queued!" << std::endl;
			} else {
				std::cout << "Buffer " << i << " NOT queued!" << std::endl;
			}
		}

		this->format_dest = this->format_src;
		format_dest.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;

		if (format_src.fmt.pix.pixelformat != V4L2_PIX_FMT_RGB24) needs_convert = true;
		else needs_convert = false;

		if (needs_convert) {
			v4l2_format copy = format_src;
			v4lconvert_try_format(v4lconv, &format_dest, &format_src);
			// v4lconvert_try_format sometimes modifies the source format if it thinks
			// that there is a better format available. Restore our selected source
			// format since we do not want that happening.
			format_src = copy;
		}

		return streamon(V4L2_BUF_TYPE_VIDEO_CAPTURE) >= 0;
	}

	bool v4lcap::start_capturing() {

		// = Enqueue all buffers ===================================================
		for (unsigned int i = 0; i < reqbuf.count; ++i) {
			struct v4l2_buffer buffer = {0};
			if (!enqueue_buff(&buffer, reqbuf.type, reqbuf.memory, i)) {
				std::cout << "Can't enqueue buffer " << i << std::endl;
			}
			//////////////////////////////
			if (!query_buffer(&buffer, reqbuf.type, reqbuf.memory, i)) {
				std::cout << "Can't request buffer " << i << std::endl;
			}
			if (buffer.flags & V4L2_BUF_FLAG_QUEUED) {
				std::cout << "Buffer " << i << " queued!" << std::endl;
			} else {
				std::cout << "Buffer " << i << " NOT queued!" << std::endl;
			}
		}

		this->format_dest = this->format_src;
		format_dest.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;

		if (format_src.fmt.pix.pixelformat != V4L2_PIX_FMT_RGB24) needs_convert = true;
		else needs_convert = false;

		if (needs_convert) {
			v4l2_format copy = format_src;
			v4lconvert_try_format(v4lconv, &format_dest, &format_src);
			// v4lconvert_try_format sometimes modifies the source format if it thinks
			// that there is a better format available. Restore our selected source
			// format since we do not want that happening.
			format_src = copy;
		}

		return streamon(V4L2_BUF_TYPE_VIDEO_CAPTURE) >= 0;
	}

	void *v4lcap::grab_raw(unsigned char *raw) {

		struct v4l2_buffer vbuf;
		memset(&vbuf, 0, sizeof(struct v4l2_buffer));

		/* dequeue buffer */
		// =========================================================================
		int count = 0;
		bool again = false;

		do {
			dequeue_buff(&vbuf, V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_MEMORY_MMAP, &again);
			if (count > 100) {
				std::cout << "Cannot dequeue buffer " << vbuf.index << std::endl;
				break;
			}
			count++;
		} while (again);
		// =========================================================================

		unsigned char *data = (unsigned char *) buffers.buffs[vbuf.index].start;
		for (unsigned int i = 0; i < vbuf.bytesused; ++i) {
			*raw = *data;
			++data;
			++raw;
		}

		// =========================================================================
		if (!enqueue_buff(&vbuf)) {
			std::cout << "Can't enqueue buffer " << vbuf.index << " !" << std::endl;
		}

		return data;
	}

	void *v4lcap::grab_rgb(unsigned char *rgb) {

		struct v4l2_buffer vbuf;
		memset(&vbuf, 0, sizeof(struct v4l2_buffer));

		/* dequeue buffer */
		// =========================================================================
		int count = 0;
		bool again = false;

		do {
			dequeue_buff(&vbuf, V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_MEMORY_MMAP, &again);
			if (count > 100) {
				std::cout << "Cannot dequeue buffer " << vbuf.index << std::endl;
				break;
			}
			count++;
		} while (again);
		// =========================================================================

		unsigned char *data = (unsigned char *) buffers.buffs[vbuf.index].start;

		if (needs_convert) {
			int err = 0;
			err = v4lconvert_convert(v4lconv, &format_src, &format_dest, (unsigned char *) data, vbuf.bytesused, rgb,
									 format_dest.fmt.pix.sizeimage);
			if (err < 0) std::cout << "OOpsss \n";
		}

//		for (int i = 0; i < format_dest.fmt.pix.sizeimage; ++i) {
//			*rgb = *data;
//			++rgb;
//			++data;
//		}

		// =========================================================================
		if (!enqueue_buff(&vbuf)) {
			std::cout << "Can't enqueue buffer " << vbuf.index << " !" << std::endl;
		}

		return NULL;
	}

	bool v4lcap::streamoff(enum v4l2_buf_type type) {
		return xioctl(fd, VIDIOC_STREAMOFF, &type) >= 0;
	}

	bool v4lcap::streamoff(int type) {
		return streamoff((enum v4l2_buf_type) type);
	}

	bool v4lcap::stop_capturing() {
		return streamoff(V4L2_BUF_TYPE_VIDEO_CAPTURE);
	}

	bool v4lcap::uninit_mmap() {
		for (unsigned int i = 0; i < reqbuf.count; i++) {
			if (emulate_format) {
				if (-1 == v4l2_munmap(buffers.buffs[i].start, buffers.buffs[i].length)) return false;
			} else {
				if (-1 == munmap(buffers.buffs[i].start, buffers.buffs[i].length)) return false;
			}
		}

		// = free all requested buffers ================================================
		if (!request_buffers_mmap(&reqbuf, 1)) std::cout << "Can't request 1 mmap buffers.\n";
		if (!request_buffers_mmap(&reqbuf, 0)) std::cout << "Can't request 0 mmap buffers.\n";

		free(buffers.buffs);
		buffers.buffs = 0;
		buffers.qtd = 0;

		return true;
	}

	bool v4lcap::close_device() {

		if (!(status & OPENED)) return true;
		if (status & STREAM_ON) streamoff(V4L2_BUF_TYPE_VIDEO_CAPTURE);
		if (status & MMAP_INIT) uninit_mmap();

		int r = 0;

		if (v4lconv) v4lconvert_destroy(v4lconv);

		if (emulate_format) r = v4l2_close(fd);
		else r = close(fd);

		if (r < 0) {
			cout << "Unable to close device!" << endl;
			return false;
		}

		fd = 0;
		return true;
	}

	/////////////////////////////////////
	bool v4lcap::query_capability(struct v4l2_capability *cap) {
		return xioctl(fd, VIDIOC_QUERYCAP, cap) >= 0;
	}

	bool v4lcap::get_format(struct v4l2_format *fmt) {
		fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		return xioctl(fd, VIDIOC_G_FMT, fmt) >= 0;
	}

	bool v4lcap::get_format(struct v4l2_format *fmt, unsigned int type) {
		fmt->type = type;
		return xioctl(fd, VIDIOC_G_FMT, fmt) >= 0;
	}

	bool v4lcap::get_input_index(unsigned int *input) {
		return xioctl(fd, VIDIOC_G_INPUT, input) >= 0;
	}

	bool v4lcap::get_input(struct v4l2_input *input) {
		unsigned int index;
		if (!get_input_index(&index)) return false;
		input->index = index;
		return xioctl(fd, VIDIOC_ENUMINPUT, input) >= 0;
	}

	bool v4lcap::get_standard_id(v4l2_std_id *std_id) {
		return xioctl(fd, VIDIOC_G_STD, std_id) >= 0;
	}

	bool v4lcap::get_standard(struct v4l2_standard *standard) {

		v4l2_std_id std_id;

		if (xioctl(fd, VIDIOC_G_STD, &std_id) < 0) return false;

		if (enum_video_standard(standard, 0, true)) {
			do {
				if (standard->id == std_id) return true;
			} while (enum_video_standard(standard));
		}
		return false;
	}

	bool v4lcap::get_stream_parameter(struct v4l2_streamparm *streamparm, unsigned int type) {
		streamparm->type = type;
		return xioctl(fd, VIDIOC_G_PARM, streamparm) >= 0;
	}

	bool v4lcap::get_control(struct v4l2_control *ctrl, unsigned int ctrl_id) {
		ctrl->id = ctrl_id;
		return xioctl(fd, VIDIOC_G_CTRL, ctrl) >= 0;
	}

	bool v4lcap::query_control(struct v4l2_queryctrl *qctrl) {
		return xioctl(fd, VIDIOC_QUERYCTRL, qctrl) >= 0;
	}

	bool v4lcap::query_control_menu(struct v4l2_querymenu *qmenu) {
		return xioctl(fd, VIDIOC_QUERYMENU, qmenu) >= 0;
	}

	bool v4lcap::request_buffers(struct v4l2_requestbuffers *req_buff) {
		int r = xioctl(fd, VIDIOC_REQBUFS, req_buff);
		if (r >= 0) return true;
		perror("VIDIOC_REQBUFS");
		return false;
	}

	bool v4lcap::request_buffers(struct v4l2_requestbuffers *req_buff, int nbuffers, enum v4l2_memory memory,
								 enum v4l2_buf_type type) {
		req_buff->reserved[0] = 0;
		req_buff->reserved[1] = 0;
		req_buff->count = nbuffers;
		req_buff->memory = memory;
		req_buff->type = type;
		return request_buffers(req_buff);
	}

	bool v4lcap::request_buffers_mmap(struct v4l2_requestbuffers *req_buff, int nbuffers) {
		return request_buffers(req_buff, nbuffers, V4L2_MEMORY_MMAP, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	}

	////////////////////////////////
	bool v4lcap::enum_audio_input(struct v4l2_audio *audio, int index, bool init) {
		if (init) audio->index = index;
		else ++audio->index;

		audio->reserved[0] = 0;
		audio->reserved[1] = 0;

		return xioctl(fd, VIDIOC_ENUMAUDIO, audio) >= 0;
	}

	bool v4lcap::enum_audio_output(struct v4l2_audioout *audiout, int index, bool init) {
		if (init) audiout->index = index;
		else ++audiout->index;

		audiout->reserved[0] = 0;
		audiout->reserved[1] = 0;

		return xioctl(fd, VIDIOC_ENUMAUDOUT, audiout) >= 0;
	}

	bool v4lcap::enum_image_format(int fd, struct v4l2_fmtdesc *fmtdesc) {
		return xioctl(fd, VIDIOC_ENUM_FMT, fmtdesc) >= 0;
//		int r = xioctl(fd, VIDIOC_ENUM_FMT, fmtdesc);
//		if (r >= 0) return true;
//		if( EINVAL == errno ) perror("VIDIOC_ENUM_FMT");
//		return false;
	}

	bool v4lcap::enum_image_format(struct v4l2_fmtdesc *fmtdesc, int index, bool init) {
		if (init) {
			memset(fmtdesc, 0, sizeof(struct v4l2_fmtdesc));
			fmtdesc->index = index;
		} else fmtdesc->index++;

		fmtdesc->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		return enum_image_format(fd, fmtdesc);
		//return xioctl(fd, VIDIOC_ENUM_FMT, fmtdesc) >= 0;
	}

	bool v4lcap::enum_frame_size(struct v4l2_frmsizeenum *frmsize, int pixel_format, int index, bool init) {

		if (init) frmsize->index = index;
		else ++frmsize->index;

		frmsize->pixel_format = pixel_format;
		return xioctl(fd, VIDIOC_ENUM_FRAMESIZES, frmsize) >= 0;
	}

	bool v4lcap::enum_frame_interval(struct v4l2_frmivalenum *frmi, int pixel_format, int width, int height, int index,
									 bool init) {

		if (init) frmi->index = 0;
		else ++frmi->index;

		frmi->pixel_format = pixel_format;
		frmi->width = width;
		frmi->height = height;
		return xioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, frmi) >= 0;
	}

	bool v4lcap::enum_video_input(struct v4l2_input *input, int index, bool init) {
		if (init) input->index = index;
		else ++input->index;

		return xioctl(fd, VIDIOC_ENUMINPUT, input) >= 0;
	}

	bool v4lcap::enum_video_output(struct v4l2_output *output, int index, bool init) {
		if (init) output->index = index;
		else ++output->index;

		return xioctl(fd, VIDIOC_ENUMOUTPUT, output) >= 0;
	}

	bool v4lcap::enum_video_standard(struct v4l2_standard *std, int index, bool init) {
		if (init) std->index = index;
		else ++std->index;

		return xioctl(fd, VIDIOC_ENUMSTD, std) >= 0;
	}

	bool v4lcap::enum_control_default(struct v4l2_queryctrl *qctrl, bool init) {
		if (init) qctrl->id = V4L2_CTRL_FLAG_NEXT_CTRL;
		else qctrl->id |= V4L2_CTRL_FLAG_NEXT_CTRL;
		return query_control(qctrl);
	}

	bool v4lcap::enum_control_user_base(struct v4l2_queryctrl *qctrl, bool init) {
		if (init) qctrl->id = V4L2_CID_BASE;
		else {
			++qctrl->id;
			if (qctrl->id >= V4L2_CID_LASTP1) return false;
		}

		while (!query_control(qctrl)) {
			++qctrl->id;
			if (qctrl->id > V4L2_CID_LASTP1) return false;
		}
		return true;
	}

	bool v4lcap::enum_control_private_base(struct v4l2_queryctrl *qctrl, bool init) {
		if (init) qctrl->id = V4L2_CID_PRIVATE_BASE;
		else ++qctrl->id;
		if (!query_control(qctrl)) return false;
		return true;
	}

	bool v4lcap::enum_control_menu(struct v4l2_querymenu *qmenu, struct v4l2_queryctrl &ctrl, int index, bool init) {

		if (init) {
			//memset(qmenu, 0, sizeof(struct v4l2_querymenu));
			qmenu->id = ctrl.id;
			qmenu->index = ctrl.minimum;
		} else qmenu->index++;

		if ((int) qmenu->index > ctrl.maximum) return false;

		while (!query_control_menu(qmenu)) {
			qmenu->index++;
		}

		return true;
	}

	///////////////////////////////
	bool v4lcap::set_input(int index) {
		return xioctl(fd, VIDIOC_S_INPUT, &index) >= 0;
	}

	bool v4lcap::set_standard(v4l2_std_id std_id) {
		return xioctl(fd, VIDIOC_S_STD, &std_id) >= 0;
	}

	bool v4lcap::set_format(struct v4l2_format *fmt) {
		return xioctl(fd, VIDIOC_S_FMT, fmt) >= 0;
	}
//	bool v4lcap::set_format(int pixel_format, int width, int height) {
//		struct v4l2_format fmt = { 0 };
//
//		if (!get_format(&fmt, V4L2_BUF_TYPE_VIDEO_CAPTURE)) return false;
//
//		switch (fmt.type) {
//			case V4L2_BUF_TYPE_VIDEO_CAPTURE:
//
//				memset(&fmt, 0, sizeof(struct v4l2_format));
//
//				fmt.fmt.pix.pixelformat = pixel_format;
//				fmt.fmt.pix.width = width;
//				fmt.fmt.pix.height = height;
//				fmt.fmt.pix.field = V4L2_FIELD_ANY;
//
//				break;
//			case V4L2_BUF_TYPE_VIDEO_OUTPUT:
//			case V4L2_BUF_TYPE_VIDEO_OVERLAY:
//			case V4L2_BUF_TYPE_VBI_CAPTURE:
//			case V4L2_BUF_TYPE_VBI_OUTPUT:
//			case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
//			case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
//				cout << "Not implemented!" << endl;
//				return false;
//		}
//
//		return set_format(&fmt);
//	}
//	bool v4lcap::set_pixel_format(unsigned int pixel_format, unsigned int type) {
//
//		struct v4l2_format fmt;
//
//		if (!get_format(&fmt, type)) return false;
//
//		fmt.fmt.pix.pixelformat = pixel_format;
//
//		return xioctl(fd, VIDIOC_S_FMT, &fmt) >= 0;
//	}

	bool v4lcap::set_pixel_format(unsigned int pixel_format) {

		struct v4l2_format fmt;

		if (!get_format(&fmt, V4L2_BUF_TYPE_VIDEO_CAPTURE)) return false;

		fmt.fmt.pix.pixelformat = pixel_format;

		return xioctl(fd, VIDIOC_S_FMT, &fmt) >= 0;
	}

	bool v4lcap::set_frame_size(int width, int height, unsigned int type) {

		struct v4l2_format fmt;

		if (!get_format(&fmt, type)) return false;

		fmt.fmt.pix.width = width;
		fmt.fmt.pix.height = height;
		return xioctl(fd, VIDIOC_S_FMT, &fmt) >= 0;
	}

	bool v4lcap::set_frame_interval(struct v4l2_fract &fract) {

		v4l2_streamparm parm;

		if (!get_stream_parameter(&parm, V4L2_BUF_TYPE_VIDEO_CAPTURE)) return false;

		if (!(parm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)) return false;

		parm.parm.capture.timeperframe = fract;

		return xioctl(fd, VIDIOC_S_PARM, &parm) >= 0;
	}

	bool v4lcap::set_control(int ctrl_id, int value) {
		struct v4l2_control ctrl;
		ctrl.id = ctrl_id;
		ctrl.value = value;
		return xioctl(fd, VIDIOC_S_CTRL, &ctrl) >= 0;
	}

	///////////////////////////////
	bool v4lcap::show_controls() {

		struct v4l2_queryctrl qctrl;

		cout << "=== Enumeratin controls via V4L2_CTRL_FLAG_NEXT_CTRL: ===" << endl;
		qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
		while (query_control(&qctrl)) {

			if ((qctrl.flags & V4L2_CTRL_FLAG_DISABLED) != 0) {
				cout << "[ Disabled ] " << qctrl.name;
			}
			cout << "Control: " << qctrl.name << endl;

			qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
		}

		cout << "=== Enumeratin controls via V4L2_CID_BASE until to V4L2_CID_LASTP1: ===" << endl;
		for (qctrl.id = V4L2_CID_BASE; qctrl.id <= V4L2_CID_LASTP1; ++qctrl.id) {

			if (!query_control(&qctrl)) continue;
			if ((qctrl.flags & V4L2_CTRL_FLAG_DISABLED) != 0) {
				cout << "[ Disabled ] " << qctrl.name;
			}
			cout << "Control: " << qctrl.name << endl;
		}

		cout << "=== Enumeratin controls via V4L2_CID_PRIVATE_BASE: ===" << endl;
		for (qctrl.id = V4L2_CID_PRIVATE_BASE; query_control(&qctrl); ++qctrl.id) {

			if ((qctrl.flags & V4L2_CTRL_FLAG_DISABLED) != 0) {
				cout << "[ Disabled ] " << qctrl.name;
			}
			cout << "Control: " << qctrl.name << endl;
		}

		return true;
	}
}

