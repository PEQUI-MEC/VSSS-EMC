/*
 * xcorner.hpp
 *
 *  Created on: Sep 21, 2012
 *      Author: gustavo
 */

#ifndef IMPL_X_CORNER_PTAM_HPP
#define IMPL_X_CORNER_PTAM_HPP

#include "../util/mem.h"
#include "../math/math_macros.h"



static int __row_stride = 0;
static int __fast_ring_16[16];

static void __init_ring_offsets(int stride) {

	int w;
	__row_stride = stride;
	w = __row_stride;

	__fast_ring_16[0] = -(w + w);
	__fast_ring_16[4] = +(2);
	__fast_ring_16[8] = +(w + w);
	__fast_ring_16[12] = -(2);

	__fast_ring_16[1] = __fast_ring_16[0] + 1;
	__fast_ring_16[2] = __fast_ring_16[1] + 1 + w;
	__fast_ring_16[3] = __fast_ring_16[4] - w;
	__fast_ring_16[5] = __fast_ring_16[4] + w;
	__fast_ring_16[6] = __fast_ring_16[5] - 1 + w;
	__fast_ring_16[7] = __fast_ring_16[8] + 1;
	__fast_ring_16[9] = __fast_ring_16[8] - 1;
	__fast_ring_16[10] = __fast_ring_16[9] - 1 - w;
	__fast_ring_16[11] = __fast_ring_16[12] + w;
	__fast_ring_16[13] = __fast_ring_16[12] - w;
	__fast_ring_16[14] = __fast_ring_16[13] + 1 - w;
	__fast_ring_16[15] = __fast_ring_16[0] - 1;

}

/**
 *
 * @param Mat
 * @param x
 * @param y
 * @param gate
 * @return
 */
template<typename T> bool __is_Xcorner_ring16(T * Mat, int st_mat, int mat_idx, int gate, T * score) {

	int pixels[16];
	int i;
	int _i;
	T sum, mean;

	if (__row_stride != st_mat) __init_ring_offsets(st_mat);

	sum = 0.0;
	for (i = 0; i < 16; i++) {
		_i = mat_idx + __fast_ring_16[i];
		pixels[i] = Mat[_i];
		sum += pixels[i];
	}

	mean = sum / 16.0;

	int highTh = mean + gate;
	int lowTh = mean - gate;

	if (Mat[mat_idx] <= lowTh || Mat[mat_idx] >= highTh) return false;

	bool state = (pixels[15] > mean);
	int nswaps = 0;
	for (i = 0; i < 16; i++) {
		int val = pixels[i];

		if (state) {
			if (val < lowTh) {
				state = false;
				nswaps++;
			}
		} else
		if (val > highTh) {
			state = true;
			nswaps++;
		};
	}

	if (nswaps != 4) {
		/* Is not a X-corner */
		return false;
	}

	/* Compute corner score */
	int sbright = 0;
	int sdark = 0;
	int ndark = 0;
	int nbright = 0;

	for (i = 0; i < 16; i++) {
		if (pixels[i] > Mat[mat_idx]) {
			sbright += pixels[i] - Mat[mat_idx];
			nbright++;
		} else {
			sdark += Mat[mat_idx] - pixels[i];
			ndark++;
		}
	}

	if (score) *score = __max(sbright, sdark);

	return true;
}

template<typename T> bool __is_Xcorner_ring16_safe(int rows, int columns, T * Mat, int st_mat, int x, int y, int gate, T * score) {

	//gate = 20;

	int mat_idx;
	T sum, mean;
	//T score;

	/* avoid invalid indexing */
	if (x < 2 || x > columns - 3) return false;
	if (y < 2 || y > rows - 3) return false;

	mat_idx = mem2d_idx( st_mat, x, y);

	return __is_Xcorner_ring16(Mat, st_mat, mat_idx, gate, score);
}

namespace impl {
	namespace img {

		template<typename T> int xcorner16(int rows, int columns, T * data_in, int st_in, T * data_score, int st_score, T gate) {

			int r, _r, _rs, c;
			long int qtd = 0;

			for (r = 2, _r = r * st_in, _rs = r * st_score; r < rows - 2;
					++r, _r += st_in, _rs += st_score) {
				for (c = 2; c < columns - 2; ++c) {

					T score = 0;

					__is_Xcorner_ring16(data_in, st_in, c + _r, gate, &score);

					data_score[_rs + c] = score;

					if (score != 0) ++qtd;

				}
			}
			return qtd;
		}

	}
}
#endif /* IMPL_XCORNER_HPP */
