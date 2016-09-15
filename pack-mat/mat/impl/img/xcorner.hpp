/*
 * xcorner.hpp
 *
 *  Created on: May 4, 2013
 *      Author: gustavo
 */

#ifndef IMPL_X_CORNER_HPP_
#define IMPL_X_CORNER_HPP_

#include <iostream>
using namespace std;

#include <math.h>

#include "../util/mem.h"
#include "../math/math_macros.h"

#include "image_integral.hpp"

class Ring {

	private:
		int calc_size(int r, int std) {
			int sz = 0;
			int x = -ray, y = 0, err = 2 - 2 * ray; /* II. Quadrant */
			do {
				sz++;
				ray = err;
				if (ray > x) err += ++x * 2 + 1; /* e_xy+e_x > 0 */
				if (ray <= y) err += ++y * 2 + 1; /* e_xy+e_y < 0 */
			} while (x < 0);

			return (sz * 4);
		}

	public:

		int stride;
		int ray;
		int sz;
		int * idx;

		Ring() :
				stride(-1), ray(0), sz(0), idx(0) {

		}

		void update(int r, int std) {
			if (ray == r && stride == std) return;

			ray = r;
			stride = std;

			sz = calc_size(r, std);

			delete[] idx;
			idx = new int[sz];

			int x = -ray, y = 0, err = 2 - 2 * ray;
			int c = 0;

			x = -ray;
			y = 0;
			err = 2 - 2 * ray;

			do {

				idx[c + sz * 0] = (0 - x) + (0 + y) * stride;  // 0
				idx[c + sz * 1] = (0 - y) + (0 - x) * stride;  // - 90
				idx[c + sz * 2] = (0 + x) + (0 - y) * stride;  // 180
				idx[c + sz * 3] = (0 + y) + (0 + x) * stride;  // 90

				c++;

				ray = err;
				if (ray > x) err += ++x * 2 + 1; /* e_xy+e_x > 0 */
				if (ray <= y) err += ++y * 2 + 1; /* e_xy+e_y < 0 */
			} while (x < 0);

		}

		~Ring() {
			delete[] idx;
		}

};

namespace impl {
	namespace img {

		void create_circle_index_trace(int xm, int ym, int ray, int stride, int ** ring, int * size) {
			int * idx;
			int sz = 0;
			int c = 0;
			int R = ray;

			int x = -ray, y = 0, err = 2 - 2 * ray; /* II. Quadrant */
			do {
				(sz)++;
				ray = err;
				if (ray > x) err += ++x * 2 + 1; /* e_xy+e_x > 0 */
				if (ray <= y) err += ++y * 2 + 1; /* e_xy+e_y < 0 */
			} while (x < 0);

			(*size) = sz * 4;

			idx = new int[(*size)];
			c = 0;

			ray = R;
			x = -ray;
			y = 0;
			err = 2 - 2 * ray;

			do {
				idx[c + sz * 0] = (xm - x) + (ym + y) * stride;  // 0
				idx[c + sz * 1] = (xm - y) + (ym - x) * stride;  // 270
				idx[c + sz * 2] = (xm + x) + (ym - y) * stride;  // 180
				idx[c + sz * 3] = (xm + y) + (ym + x) * stride;  // 90

				ray = err;
				if (ray > x) err += ++x * 2 + 1; /* e_xy+e_x > 0 */
				if (ray <= y) err += ++y * 2 + 1; /* e_xy+e_y < 0 */

				c++;
			} while (x < 0);

			(*ring) = idx;
		}

		void __get_circle_idx_trace(int xm, int ym, int ray, int stride, int ** ring, int * size) {

			int * idx;
			int sz = 0;
			int c = 0;
			int R = ray;

			int x = -ray, y = 0, err = 2 - 2 * ray; /* II. Quadrant */
			do {
				(sz)++;
				ray = err;
				if (ray > x) err += ++x * 2 + 1; /* e_xy+e_x > 0 */
				if (ray <= y) err += ++y * 2 + 1; /* e_xy+e_y < 0 */
			} while (x < 0);

			(*size) = sz * 4;

//			idx = mem_allocn((*size), int);
			idx = new int[(*size)];
			c = 0;

			ray = R;
			x = -ray;
			y = 0;
			err = 2 - 2 * ray;

			//sz = sz - 1;

			do {

				idx[c + sz * 0] = (xm - x) + (ym + y) * stride;  // 0
				idx[c + sz * 1] = (xm - y) + (ym - x) * stride;  // 270
				idx[c + sz * 2] = (xm + x) + (ym - y) * stride;  // 180
				idx[c + sz * 3] = (xm + y) + (ym + x) * stride;  // 90

				ray = err;
				if (ray > x) err += ++x * 2 + 1; /* e_xy+e_x > 0 */
				if (ray <= y) err += ++y * 2 + 1; /* e_xy+e_y < 0 */

				c++;
			} while (x < 0);

			(*ring) = idx;
		}

		template<typename T> T __x_corner_symmetry(T * mat, int * ring, int sz) {

			int s = sz / 2;
			int hs = sz / 4;

			int sum_response = 0;  // X-corner response

			for (int i = 0; i < hs; ++i) {

				T a, b;

				a = *(mat + ring[i]) + *(mat + ring[i + s]);
				b = *(mat + ring[i + hs]) + *(mat + ring[i + hs + s]);
				sum_response += __abs(a - b);

			}

			return sum_response;
		}

		template<typename T> double __symmetry(T * mat, double mean, int * ring, int sz) {

			int s = sz / 2;
			int hs = sz / 4;

			double sim = 0;
			double r_mean = 0;
			double edge = 0;

			for (int i = 0; i < sz; ++i) {
				r_mean += *(mat + ring[i]);
			}
			r_mean = r_mean / (double) sz;

			for (int i = 0; i < s; ++i) {
				T a = *(mat + ring[i]) - *(mat + ring[i + s]);
				edge += __abs(a);
			}

			for (int i = 0; i < hs; ++i) {

				T a, b;

				a = *(mat + ring[i]) + *(mat + ring[i + s]);
				b = *(mat + ring[i + hs]) + *(mat + ring[i + hs + s]);

				a = __abs(a-b);
				sim += __abs(a);

			}

			r_mean = __abs( r_mean - mean );
			sim = sim - edge - r_mean;

			if (sim < 0) sim = 0;
			return sim;
		}

		template<typename T, typename Tc> void symmetry(int rows, int columns, T * mat, int stm, Tc * corners, int stc, int ray) {

			int r, c, _rm, im, _rc, ic, _ri, ii;
			int * ring;
			int sz;

			double * intimg = new double[rows * columns];

			image_integral(rows, columns, mat, stm, intimg, columns);

			__get_circle_idx_trace(0, 0, ray, stm, &ring, &sz);

			for (r = ray + 1, _rm = r * stm, _rc = r * stc, _ri = r * columns;  //
					r < rows - ray;  //
					r++, _rm += stm, _rc += stc, _ri += columns) {

				for (c = ray + 1; c < columns - ray; c++) {

					im = _rm + c;
					ic = _rc + c;
					ii = _ri + c;

					double mean;

					mean = intimg[ii + columns + 1] - intimg[ii + columns - 2] - intimg[ii - (2 * columns) + 1] + intimg[ii - (2 * columns) - 2];
					mean = mean / 9.0;

					corners[ic] = __symmetry(&mat[im], mean, ring, sz);

				}
			}

			delete[] ring;
			delete[] intimg;
		}

		template<typename T> double __is_xcorner(T * mat, double gate, int * ring, int sz) {

			// Computes mean
			double mean = 0;
			for (int i = 0; i < sz; ++i) {
				mean += *(mat + ring[i]);
			}
			mean /= sz;

			// Computes high and low threshold
			double highTh = mean + gate;
			double lowTh = mean - gate;

			// Verifies the central pixel
			if (*mat <= lowTh || *mat >= highTh) return 0;

			// Computes the number of high contrast alternances
			bool state = *(mat + ring[sz - 1]) > mean;
			int nswaps = 0;
			for (int i = 0; i < sz; i++) {

				int val = *(mat + ring[i]);

				if (state) {
					if (val < lowTh) {
						state = false;
						nswaps++;
					}
				} else if (val > highTh) {
					state = true;
					nswaps++;
				};
			}
			if (nswaps != 0) {
				nswaps = nswaps + 1;
				nswaps--;
			}

			// Verifies the number of alternances
			if (nswaps != 4) {
				/* Is not a X-corner */
				return 0;
			}

			/* Is a X-corner */

			// Compute the corner score
			int sbright = 0;
			int sdark = 0;

			int ndark = 0;
			int nbright = 0;

			for (int i = 0; i < sz; i++) {

				T val = *(mat + ring[i]);

				if (val > *mat) {
					sbright += val - *mat;
					nbright++;
				} else {
					sdark += *mat - val;
					ndark++;
				}
			}
			//score = MIN(sbright, sdark);
			//score = MAX(sbright, sdark);
			//score = 1.0 - ABS(ndark-nbright)/(ndark+nbright);

//	 score = nbright*sbright - ndark*sdark;
//	 score = score / ((nbright + ndark)*255.0);
//	 score = 1 - ABS(score);

			double score = 1 - abs(nbright - ndark) / (double) sz;

			return score;

		}

		template<typename T, typename Tc> int xcorner(  //
				int rows, int columns,  //
				T * mat, int stm,  //
				Tc * corners, int stc,  //
				int ray, double gate) {

			int count = 0;
			int r, c, _rm, im, _rc, ic;

			int * __ring;
			int sz;

			create_circle_index_trace(0, 0, ray, stm, &__ring, &sz);

			for (r = ray, _rm = r * stm, _rc = r * stc;  //
					r < rows - ray; r++, _rm += stm, _rc += stc) {
				for (c = ray; c < columns - ray; c++) {

					im = _rm + c;
					ic = _rc + c;

					double score = __is_xcorner(&mat[im], gate, __ring, sz);

					corners[ic] = score;

					if (score) {
						count++;
					}
				}
			}

			delete[] __ring;

			return count;
		}

		template<typename T, typename Tg, typename Tc> int xcorner(  //
				int rows, int columns,  //
				T * mat, int stm,  //
				Tc * corners, int stc,  //
				Tg * gate, int stg,  //
				int ray) {

			int count = 0;
			int r, c, _rm, im, _rc, ic, _rg, ig;

			int * __ring;
			int sz;

			__get_circle_idx_trace(0, 0, ray, stm, &__ring, &sz);

			for (r = ray, _rm = r * stm, _rc = r * stc, _rg = r * stg;  //
					r < rows - ray; r++, _rm += stm, _rc += stc, _rg += stg) {
				for (c = ray; c < columns - ray; c++) {

					im = _rm + c;
					ic = _rc + c;
					ig = _rg + c;

					Tc score = __is_xcorner(&mat[im], gate[ig], __ring, sz);

					if (score) {
						corners[ic] = score;
						count++;
					}
				}
			}

			delete[] __ring;

			return count;
		}

		/**
		 * This function verifies if a central pixel (*mat) is a x-corner counting the number of
		 * high contrast alternances around its neighborhood defined by a Bresenhan circle (ring).
		 *
		 * This function is very similar to the PTAM detector except for the gate is computed
		 * adaptively.
		 *
		 * The low and high thresholds are defined by a factor k times the standard deviation of
		 * the pixels inside the circle neighborhood.
		 *
		 * low = u - k*std
		 * high = u + k*std
		 *
		 */
		template<typename T> double __is_xcorner_adaptive_ring(T * mat, double k, int * ring, int sz, double min_std) {

			double gate;
			double area = 0;
			double area2 = 0;
			double mean = 0;
			double std;

			// Computes area and area2
			for (int i = 0; i < sz; ++i) {
				T val = *(mat + ring[i]);
				area += val;
				area2 += (val * val);
			}
			mean = area / (double) sz;
			std = (area2 - sz * mean * mean) / (double) (sz - 1);
			std = sqrt(std);

			if (std <= min_std) return 0;

			gate = std * k;

			// Computes high and low threshold
			T highTh = mean + gate;
			T lowTh = mean - gate;

			// Verifies the central pixel
			if (*mat <= lowTh || *mat >= highTh) return 0;

			// Computes the number of high contrast alternances
			bool state;
			state = *(mat + ring[sz - 1]) > mean;  // Original PTAM initial verification

			// My initial verification
//			if (*(mat + ring[sz - 1]) > highTh) state = true;
//			else if (*(mat + ring[sz - 1]) < lowTh) state = false;
//			else return 0;

			int nswaps = 0;
			for (int i = 0; i < sz; i++) {

				int val = *(mat + ring[i]);

				if (state) {
					if (val < lowTh) {
						state = false;
						nswaps++;
					}
				} else if (val > highTh) {
					state = true;
					nswaps++;
				};
			}
			if (nswaps != 0) {
				nswaps = nswaps + 1;
				nswaps--;
			}

			// Verifies the number of alternances
			if (nswaps != 4) {
//			if (nswaps < 3 || nswaps > 4) {
				/* Is not a X-corner */
				return 0;
			}

//			// Computes area and area2
//			for (int i = 0; i < sz; ++i) {
//				T val = *(mat + ring[i]);
//				cout << val << " ";
//			}
//			cout << endl;

			/* Is a X-corner */

			// Compute the corner score
			int sbright = 0;
			int sdark = 0;

			int ndark = 0;
			int nbright = 0;

			for (int i = 0; i < sz; i++) {

				T val = *(mat + ring[i]);

				if (val > *mat) {
					sbright += val - *mat;
					nbright++;
				} else {
					sdark += *mat - val;
					ndark++;
				}
			}

			double score = 1 - abs(nbright - ndark) / (double) sz;

			return score;
		}

		template<typename T, typename Tc> int xcorner_adaptive_ring(  //
				int rows, int columns,  //
				T * mat, int stm,  //
				Tc * corners, int stc,  //
				double k, int ray, double min_std) {

			int count = 0;
			int r, c, _rm, im, _rc, ic;

			int * __ring;
			int sz;

			create_circle_index_trace(0, 0, ray, stm, &__ring, &sz);

			for (r = ray, _rm = r * stm, _rc = r * stc;  //
					r < rows - ray; r++, _rm += stm, _rc += stc) {
				for (c = ray; c < columns - ray; c++) {

					im = _rm + c;
					ic = _rc + c;

					double score = __is_xcorner_adaptive_ring(&mat[im], k, __ring, sz, min_std);

					corners[ic] = score;

					if (score) {
						count++;
					}
				}
			}

			delete[] __ring;

			return count;
		}

		//-------------------------------------------------------------------------------------------
		//- CheSS -----------------------------------------------------------------------------------
		/**
		 * This function implements the operator presented in:
		 * S. Bennett and J. Lasenby, “ChESS-Quick and Robust Detection of Chess-board Features,” CoRR, vol. abs/1301.5, pp. 1–13, 2013.
		 */
		template<typename T> double __xcorner_CheSS_response(T * mat, double local_mean, int * ring, int sz) {

			int s = sz / 2;
			int hs = sz / 4;

			double R;  // CheSS response
			double sum_response = 0;  // X-corner response
			double diff_response = 0;  // Edge response
			double mean_response = 0;
			double ring_mean = 0;

			for (int i = 0; i < sz; ++i) {
				ring_mean += *(mat + ring[i]);
			}
			ring_mean = ring_mean / (double) sz;

			for (int i = 0; i < s; ++i) {
				T a = *(mat + ring[i]) - *(mat + ring[i + s]);
				diff_response += __abs(a);
			}

			for (int i = 0; i < hs; ++i) {

				T a, b;

				a = *(mat + ring[i]) + *(mat + ring[i + s]);
				b = *(mat + ring[i + hs]) + *(mat + ring[i + hs + s]);
				sum_response += __abs(a - b);

			}

			mean_response = __abs( ring_mean - local_mean );
			R = sum_response - diff_response - mean_response;

			if (R < 0) R = 0;
			return R;
		}

		template<typename T, typename Tc> void xcorner_CheSS(int rows, int columns, T * mat, int stm, Tc * corners, int stc, int ray) {

			int r, c, _rm, im, _rc, ic, _ri, ii;
			int * ring;
			int sz;

			double * intimg = new double[rows * columns];

			image_integral(rows, columns, mat, stm, intimg, columns);

			create_circle_index_trace(0, 0, ray, stm, &ring, &sz);

			for (r = ray + 1, _rm = r * stm, _rc = r * stc, _ri = r * columns;  //
					r < rows - ray;  //
					r++, _rm += stm, _rc += stc, _ri += columns) {

				for (c = ray + 1; c < columns - ray; c++) {

					im = _rm + c;
					ic = _rc + c;
					ii = _ri + c;

					double local_mean;

					local_mean = intimg[ii + columns + 1] - intimg[ii + columns - 2] - intimg[ii - (2 * columns) + 1] + intimg[ii - (2 * columns) - 2];
					local_mean = local_mean / 9.0;

					corners[ic] = __xcorner_CheSS_response(&mat[im], local_mean, ring, sz);
				}
			}

			delete[] ring;
			delete[] intimg;
		}

		//-------------------------------------------------------------------------------------------
		//- PTAM ------------------------------------------------------------------------------------
		/**
		 * This function implements the x-corner detector in PTAM source.
		 * http://www.robots.ox.ac.uk/~gk/PTAM/
		 */
		template<typename T> bool __is_xcorner_ptam(T * mat, double gate, int * ring, int sz) {

			// Computes mean
			double mean = 0;
			for (int i = 0; i < sz; ++i) {
				mean += *(mat + ring[i]);
			}
			mean /= sz;

			// Computes high and low threshold
			double highTh = mean + gate;
			double lowTh = mean - gate;

			// Verifies the central pixel
			if (*mat <= lowTh || *mat >= highTh) return 0;

			// Computes the number of high contrast alternances
			bool state;
			state = *(mat + ring[sz - 1]) > mean;  // Original PTAM initial verification

			int nswaps = 0;
			for (int i = 0; i < sz; i++) {

				int val = *(mat + ring[i]);

				if (state) {
					if (val < lowTh) {
						state = false;
						nswaps++;
					}
				} else if (val > highTh) {
					state = true;
					nswaps++;
				};
			}

			/**
			 * Verifies the number of alternances.
			 * If nswaps == 4 the central pixel is a x-corner
			 * else it is not a x-corner
			 */
			if (nswaps != 4) return false;
			return true;
		}
		/**
		 * This function verifies if each pixel of image mat is a xcorner_ptam and store the results in corners
		 */
		template<typename T, typename Tc> int xcorner_ptam(  //
				int rows, int columns,  //
				T * mat, int stm,  //
				double gate, int ray,  //
				Tc * corners, int stc) {

			int count = 0;
			int r, c, _rm, im, _rc, ic;

			int * __ring;
			int sz;
			create_circle_index_trace(0, 0, ray, stm, &__ring, &sz);

			for (r = ray, _rm = r * stm, _rc = r * stc;  //
					r < rows - ray; r++, _rm += stm, _rc += stc) {
				for (c = ray; c < columns - ray; c++) {

					im = _rm + c;
					ic = _rc + c;

					Tc score = __is_xcorner_ptam(&mat[im], gate, __ring, sz);
					corners[ic] = score;
					if (score) count++;

				}
			}
			delete[] __ring;
			return count;
		}

		/**
		 * The original PTAM x-corner detector returns a boolean response for each pixel.
		 * In this function, if the point is a x-corner, a score is computed and returned,
		 * otherwise 0 is returned.
		 */
		template<typename T> double __is_xcorner_ptam_scored(T * mat, double gate, int * ring, int sz) {

			// Computes the mean
			double mean = 0;
			for (int i = 0; i < sz; ++i) {
				mean += *(mat + ring[i]);
			}
			mean /= sz;

			// Computes high and low threshold
			double highTh = mean + gate;
			double lowTh = mean - gate;

			// Verifies the central pixel
			if (*mat <= lowTh || *mat >= highTh) return 0;

			// Computes the number of high contrast alternances
			bool state;
			state = *(mat + ring[sz - 1]) > mean;  // Original PTAM initial verification

			int nswaps = 0;
			for (int i = 0; i < sz; i++) {

				int val = *(mat + ring[i]);

				if (state) {
					if (val < lowTh) {
						state = false;
						nswaps++;
					}
				} else if (val > highTh) {
					state = true;
					nswaps++;
				};
			}

			/**
			 * Verifies the number of alternances.
			 * If nswaps == 4 the central pixel is a x-corner
			 * else it is not a x-corner
			 */
			if (nswaps != 4) return 0;

			/* The point is a x-corner */
			// Computes the corner score
			int sbright = 0;
			int sdark = 0;
			int ndark = 0;
			int nbright = 0;

			for (int i = 0; i < sz; i++) {

				T val = *(mat + ring[i]);

				if (val > *mat) {
					sbright += val - *mat;
					nbright++;
				} else {
					sdark += *mat - val;
					ndark++;
				}
			}
			//score = MIN(sbright, sdark);
			//score = MAX(sbright, sdark);
			//score = 1.0 - ABS(ndark-nbright)/(ndark+nbright);
			//score = nbright*sbright - ndark*sdark;
			//score = score / ((nbright + ndark)*255.0);
			//score = 1 - ABS(score);

//			double diff = nbright - ndark;
//			double score = 1 - __abs(diff) / (double) sz;
//			return score;
//			int m = __min(nbright, ndark);
//			return m;

			double score = 1 - abs(nbright - ndark) / (double) sz;

			return score;
		}

		/**
		 * This function verifies if each pixel of image mat is a xcorner_ptam and store the results in corners
		 */
		template<typename T, typename Tc> int xcorner_ptam_scored(  //
				int rows, int columns,  //
				T * mat, int stm,  //
				double gate, int ray,  //
				Tc * corners, int stc) {

			int count = 0;
			int r, c, _rm, im, _rc, ic;

			int * __ring;
			int sz;
			create_circle_index_trace(0, 0, ray, stm, &__ring, &sz);

			for (r = ray, _rm = r * stm, _rc = r * stc;  //
					r < rows - ray; r++, _rm += stm, _rc += stc) {
				for (c = ray; c < columns - ray; c++) {

					im = _rm + c;
					ic = _rc + c;

					Tc score = __is_xcorner_ptam_scored(&mat[im], gate, __ring, sz);
					corners[ic] = score;
					if (score) count++;

				}
			}
			delete[] __ring;
			return count;
		}

		/**
		 * This function implements the detection of x-corners points based
		 */
		template<typename T> int __is_xcorner_adaptive_by_region(T * mat, int * ring, int sz, T lowTh, T highTh) {

			int acc[sz];
			for (int i = 0; i < sz; ++i)
				acc[i] = 0;

			// Verifies the central pixel
			if (*mat <= lowTh || *mat >= highTh) return 0;

			// Computes the number of high contrast alternances
			bool state = *(mat + ring[sz - 1]) > ((highTh + lowTh) / 2.0);
			int nswaps = 0;
			for (int i = 0; i < sz; i++) {

				int val = *(mat + ring[i]);

				acc[nswaps] = acc[nswaps] + 1;

				if (state) {
					if (val < lowTh) {
						state = false;
						nswaps++;
					}
				} else if (val > highTh) {
					state = true;
					nswaps++;
				};
			}

			// Verifies the number of alternances
			if (nswaps < 3 || nswaps > 4) {
				/* Is not a X-corner */
				return 0;
			}

			/* Is a X-corner */
			int sc;
			sc = __x_corner_symmetry(mat, ring, sz);
			return sc;
		}

		template<typename T, typename Tc> int xcorner_adaptive_by_region(  //
				int rows, int columns,  //
				T * mat, int stm,  //
				double k,  //
				int ray, double min_std,  //
				Tc * corners, int stc) {

			int count = 0;

			int * __ring;
			int sz;
			create_circle_index_trace(0, 0, ray, stm, &__ring, &sz);

			double * imint = new double[rows * columns];
			double * imint2 = new double[rows * columns];
			double * u = new double[rows * columns];
			double * std = new double[rows * columns];

			impl::img::image_integral(rows, columns, mat, stm, imint, columns);
			impl::img::image_integral2(rows, columns, mat, stm, imint2, columns);
			impl::img::image_integral_to_mean_and_std(rows, columns, imint, columns, imint2, columns, u, columns, std, columns, ray + 1);

			int stu = columns;

			for (int r = ray, _rm = r * stm, _rc = r * stc, _ru = r * stu;  //
			r < rows - ray;  //
					r++, _rm += stm, _rc += stc, _ru += stu) {
				for (int c = ray; c < columns - ray; c++) {

					// avoid regions with low contrast
					if (std[_ru + c] < min_std) continue;

					double high = u[_ru + c] + k * std[_ru + c];
					double low = u[_ru + c] - k * std[_ru + c];

					double score = __is_xcorner_adaptive_by_region(&mat[_rm + c], __ring, sz, low, high);

					corners[_rc + c] = score;
					if (score) count++;

				}
			}

			delete[] __ring;

			delete[] imint;
			delete[] imint2;
			delete[] u;
			delete[] std;

			return count;
		}

		///////////////////////////////////////////////
		//////////////////  OK  ///////////////////////
		///////////////////////////////////////////////

		template<typename T> T __is_xcorner(T * mat, int * ring, int sz, double lowTh, double highTh) {

			int acc[sz];
			for (int i = 0; i < sz; ++i)
				acc[i] = 0;

			// Evaluates the central pixel
			if (*mat <= lowTh || *mat >= highTh) return 0;

			// Computes the number of high contrast alternances
			bool state = *(mat + ring[sz - 1]) > ((highTh + lowTh) / 2.0);
			int nswaps = 0;
			for (int i = 0; i < sz; i++) {

				int val = *(mat + ring[i]);

				acc[nswaps] = acc[nswaps] + 1;

				if (state) {
					if (val < lowTh) {
						state = false;
						nswaps++;
					}
				} else if (val > highTh) {
					state = true;
					nswaps++;
				};
			}

			// Verifies the number of alternances
			if (nswaps < 3 || nswaps > 4) {
				/* Is not a X-corner */
				return 0;
			}

			/* Is a X-corner */
			T sc;
			sc = __x_corner_symmetry(mat, ring, sz);
			return sc;
		}

		template<typename T, typename Tc> int xcorner_region_adapt(  //
				int rows, int columns,  //
				T * mat,  //
				double * mean,  //
				double * std,  //
				double k, int ray, double min_std,  //
				Tc * corners) {

			int count = 0;

			int * __ring;
			int sz;
			int st = columns;
			create_circle_index_trace(0, 0, ray, st, &__ring, &sz);

			for (int r = ray, _r = r * st; r < rows - ray; r++, _r += st) {
				for (int c = ray; c < columns - ray; c++) {

					int i = _r + c;

					// avoid regions with low contrast
					if (std[i] < min_std) continue;

					// computes the thresholds
					double high = mean[i] + k * std[i];
					double low = mean[i] - k * std[i];

					// is x-corner?
					T score = __is_xcorner(&mat[i], __ring, sz, low, high);
					corners[i] = score;
					if (score) count++;

				}
			}

			delete[] __ring;
			return count;
		}

	}
}

#endif /* IMPL_X_CORNER_HPP_ */
