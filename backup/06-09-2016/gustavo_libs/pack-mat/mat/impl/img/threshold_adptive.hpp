/*
 * threshold_adptive.hpp
 *
 *  Created on: Sep 23, 2012
 *      Author: gustavo
 */

#ifndef IMPL_THRESHOLD_ADPTIVE_HPP
#define IMPL_THRESHOLD_ADPTIVE_HPP

#include "image_integral.hpp"

#include "../util/mem.h"
#include "../math/math_macros.h"

#include "../img/io.hpp"

#ifndef CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

namespace impl {
	namespace img {

		/**
		 * \brief Extended Wellner's Adaptive Thresholding.
		 *
		 * This function implements the Extended Wellner's Adaptive Thresholding.
		 *
		 * \sa Bradley, G., Roth, G. (2007), "Adaptive Thresholding Using Integral Image", Journal of Graphics Tools, Vol.12, No.2, p.13-21.
		 *
		 * @param rows
		 * @param columns
		 * @param img
		 * @param thresh
		 * @param width ~ 1/8 * columns
		 * @param t ~ 0.15
		 */
		template<typename T, typename Tbw> void threshold_wellner_extended(  //
				int rows, int columns, T * img,  //
				Tbw * thresh, int width, double t) {

			/**
			 * TODO: adaptar para usar stride
			 */

			int r, c, _r, i;

			double * imint = mem_allocn(rows*columns, double);
			double * AREA = mem_allocn(rows*columns, double);
			T area;

			int ray = width / 2;
			int w = (ray << 1) + 1;
			int n_pixels = w * w;

			img::image_integral(rows, columns, img, columns, imint, columns);
			img::image_integral_to_area(rows, columns, imint, columns, AREA, columns, ray);

			// Check Image Center
			for (r = ray, _r = r * columns; r < rows - ray; r++, _r += columns) {
				for (c = ray; c < columns - ray; c++) {
					i = _r + c;

					area = AREA[i];
					if (img[i] * n_pixels <= area * (1 - t)) {
						thresh[i] = 0.0;
					} else {
						thresh[i] = 255.0;
					}
				}
			}

			int x1, y1, x2, y2;
			int max_r, max_c;

			max_r = rows - 1;
			max_c = columns - 1;

			// Check Top Border
			for (r = 0, _r = r * columns; r < ray; r++, _r += columns) {

				y1 = r - ray;
				y2 = r + ray;

				y1 = CLAMP(y1, 0, max_r);
				y2 = CLAMP(y2, 0, max_r);

				for (c = 0; c < columns; c++) {
					i = _r + c;

					x1 = c - ray;
					x2 = c + ray;

					x1 = CLAMP(x1, 0, max_c);
					x2 = CLAMP(x2, 0, max_c);

					n_pixels = (x2 - x1) * (y2 - y1);

					area = AREA[i];

					if (img[i] * n_pixels <= area * (1 - t)) {
						thresh[i] = 0.0;
					} else {
						thresh[i] = 255.0;
					}

				}
			}

			// Check Bottom Border
			for (r = rows - ray, _r = r * columns; r < rows; r++, _r += columns) {

				y1 = r - ray;
				y2 = r + ray;

				y1 = CLAMP(y1, 0, max_r);
				y2 = CLAMP(y2, 0, max_r);

				for (c = 0; c < columns; c++) {
					i = _r + c;

					x1 = c - ray;
					x2 = c + ray;

					x1 = CLAMP(x1, 0, max_c);
					x2 = CLAMP(x2, 0, max_c);

					n_pixels = (x2 - x1) * (y2 - y1);

					area = AREA[i];

					if (img[i] * n_pixels <= area * (1 - t)) {
						thresh[i] = 0.0;
					} else {
						thresh[i] = 255.0;
					}

				}
			}

			// Check Left Border
			for (r = ray, _r = r * columns; r < rows - ray; r++, _r += columns) {

				y1 = r - ray;
				y2 = r + ray;

				y1 = CLAMP(y1, 0, max_r);
				y2 = CLAMP(y2, 0, max_r);

				for (c = 0; c < ray; c++) {
					i = _r + c;

					x1 = c - ray;
					x2 = c + ray;

					x1 = CLAMP(x1, 0, max_c);
					x2 = CLAMP(x2, 0, max_c);

					n_pixels = (x2 - x1) * (y2 - y1);

					area = AREA[i];

					if (img[i] * n_pixels <= area * (1 - t)) {
						thresh[i] = 0.0;
					} else {
						thresh[i] = 255.0;
					}

				}
			}

			// Check Right Border
			for (r = ray, _r = r * columns; r < rows - ray; r++, _r += columns) {

				y1 = r - ray;
				y2 = r + ray;

				y1 = CLAMP(y1, 0, max_r);
				y2 = CLAMP(y2, 0, max_r);

				for (c = columns - ray; c < columns; c++) {
					i = _r + c;

					x1 = c - ray;
					x2 = c + ray;

					x1 = CLAMP(x1, 0, max_c);
					x2 = CLAMP(x2, 0, max_c);

					n_pixels = (x2 - x1) * (y2 - y1);

					area = AREA[i];

					if (img[i] * n_pixels <= area * (1 - t)) {
						thresh[i] = 0.0;
					} else {
						thresh[i] = 255.0;
					}
				}
			}

			mem_free(imint);
			mem_free(AREA);
		}

		template<typename T, typename Tbw> void threshold_wellner_extended(  //
				int rows, int columns, T * img,  //
				double * imint, Tbw * thresh, int width, double t) {

			/**
			 * TODO: adaptar para usar stride
			 */

			int r, c, _r, i;

			double * AREA = mem_allocn(rows*columns, double);
			T area;

			int ray = width / 2;
			int w = (ray << 1) + 1;
			int n_pixels = w * w;

			img::image_integral_to_area(rows, columns, imint, columns, AREA, columns, ray);

			// Check Image Center
			for (r = ray, _r = r * columns; r < rows - ray; r++, _r += columns) {
				for (c = ray; c < columns - ray; c++) {
					i = _r + c;

					area = AREA[i];
					if (img[i] * n_pixels <= area * (1 - t)) {
						thresh[i] = 0.0;
					} else {
						thresh[i] = 255.0;
					}
				}
			}

			int x1, y1, x2, y2;
			int max_r, max_c;

			max_r = rows - 1;
			max_c = columns - 1;

			// Check Top Border
			for (r = 0, _r = r * columns; r < ray; r++, _r += columns) {

				y1 = r - ray;
				y2 = r + ray;

				y1 = CLAMP(y1, 0, max_r);
				y2 = CLAMP(y2, 0, max_r);

				for (c = 0; c < columns; c++) {
					i = _r + c;

					x1 = c - ray;
					x2 = c + ray;

					x1 = CLAMP(x1, 0, max_c);
					x2 = CLAMP(x2, 0, max_c);

					n_pixels = (x2 - x1) * (y2 - y1);

					area = AREA[i];

					if (img[i] * n_pixels <= area * (1 - t)) {
						thresh[i] = 0.0;
					} else {
						thresh[i] = 255.0;
					}

				}
			}

			// Check Bottom Border
			for (r = rows - ray, _r = r * columns; r < rows; r++, _r += columns) {

				y1 = r - ray;
				y2 = r + ray;

				y1 = CLAMP(y1, 0, max_r);
				y2 = CLAMP(y2, 0, max_r);

				for (c = 0; c < columns; c++) {
					i = _r + c;

					x1 = c - ray;
					x2 = c + ray;

					x1 = CLAMP(x1, 0, max_c);
					x2 = CLAMP(x2, 0, max_c);

					n_pixels = (x2 - x1) * (y2 - y1);

					area = AREA[i];

					if (img[i] * n_pixels <= area * (1 - t)) {
						thresh[i] = 0.0;
					} else {
						thresh[i] = 255.0;
					}

				}
			}

			// Check Left Border
			for (r = ray, _r = r * columns; r < rows - ray; r++, _r += columns) {

				y1 = r - ray;
				y2 = r + ray;

				y1 = CLAMP(y1, 0, max_r);
				y2 = CLAMP(y2, 0, max_r);

				for (c = 0; c < ray; c++) {
					i = _r + c;

					x1 = c - ray;
					x2 = c + ray;

					x1 = CLAMP(x1, 0, max_c);
					x2 = CLAMP(x2, 0, max_c);

					n_pixels = (x2 - x1) * (y2 - y1);

					area = AREA[i];

					if (img[i] * n_pixels <= area * (1 - t)) {
						thresh[i] = 0.0;
					} else {
						thresh[i] = 255.0;
					}

				}
			}

			// Check Right Border
			for (r = ray, _r = r * columns; r < rows - ray; r++, _r += columns) {

				y1 = r - ray;
				y2 = r + ray;

				y1 = CLAMP(y1, 0, max_r);
				y2 = CLAMP(y2, 0, max_r);

				for (c = columns - ray; c < columns; c++) {
					i = _r + c;

					x1 = c - ray;
					x2 = c + ray;

					x1 = CLAMP(x1, 0, max_c);
					x2 = CLAMP(x2, 0, max_c);

					n_pixels = (x2 - x1) * (y2 - y1);

					area = AREA[i];

					if (img[i] * n_pixels <= area * (1 - t)) {
						thresh[i] = 0.0;
					} else {
						thresh[i] = 255.0;
					}
				}
			}

			mem_free(AREA);
		}

		/**
		 * \brief Fast Sauvola's Adptive Thresholding.
		 *
		 * This function calculates a binary image using a Sauvola algorithm.
		 * The Fast Adaptive Image Binarization uses Sauvola's test and Summed Area Tables.
		 *
		 * \see Faisal Shafait, Daniel Keysers, Thomas Breuel, (2008), "Efficient Implementation of Local Adaptive Thresholding Techniques Using Integral Images", Proceedings of the 15th Document Recognition and Retrieval Conference, IS&T/SPIE International Symposium on Electronic Imaging, January 26-31, San Jose, CA, USA volume 6815.
		 * \see Sauvola, J & Pietaksinen, M (2000), "Adaptive Document Image Binarization", Pattern Recognition 33(2): 225-236
		 *
		 * @param rows
		 * @param columns
		 * @param Img
		 * @param BW
		 * @param width
		 * @param k  [0.2 ~ 0.5 ]
		 * @param max_s  ~128 for document images
		 *
		 *
		 * TODO: precisa rever o cálculo da área usando a imagem integral.
		 * Sugiro fazer como na função threshold_wellner_extended, usando as funções já implementadas
		 * pelo image_integral.hpp
		 */
		template<typename T, typename Tbw> void threshold_Sauvola_fast(int rows, int columns, T * Img, Tbw * BW, int width, T k) {

			/*
			 * TODO: adicionar suporte a stride para cada imagem!
			 */

			// stride
			int st_img = columns;
			//int st_bw = columns;

			T area, area2;

			/* Adaptive threshold calculated by each window */
			T Th;
			/* Maximum value of standard deviation */
			T max_s = 128.0;

			/* mean */
			T u;
			/* variance */
			T s2;
			/* standard deviation */
			T s;

			/* number of neighbors */
			int nn = width / 2;
			width = (2 * nn) + 1;
			int shift = nn * columns;
			/* window size */
			int ws = width * width;
			//ws = (2n+1)(2n+1) = 4n² + 4n + 1;

			T * Imgi = mem_allocn( rows*columns, T );
			T * Imgi2 = mem_allocn( rows*columns, T );
			image_integral2(rows, columns, Img, st_img, Imgi, columns, Imgi2, columns);

			int r, c, _r, i;
			int i1, i2, i3, i4;

			for (r = nn, _r = r * columns; r < rows - nn; r++, _r += columns) {
				for (c = nn; c < columns - nn; c++) {
					i = _r + c;

					i1 = i - shift - nn;
					i2 = i - shift + nn;
					i3 = i + shift + nn;
					i4 = i + shift - nn;

					area = Imgi[i3] - Imgi[i2] - Imgi[i4] + Imgi[i1];
					area2 = Imgi2[i3] - Imgi2[i2] - Imgi2[i4] + Imgi2[i1];

					u = area / ws;
					s2 = (area2 - u * u) / ws;

					s = sqrt(s2);

					/* adaptive threshold */
					Th = u * (1 + (k * ((s / max_s) - 1)));

					if (Img[i] > Th) {
						BW[i] = 255.0;
					} else {
						BW[i] = 0.0;

					}
				}
			}

			/* = Border treatment =================================================== */
			int x1, y1, x2, y2;
			int rows1, columns1;

			rows1 = rows - 1;
			columns1 = columns - 1;

			// <editor-fold defaultstate="collapsed" desc="Check Top Border">
			for (r = 0, _r = r * columns; r < nn; r++, _r += columns) {

				y1 = r - nn;
				y2 = r + nn;
				y1 = __clamp(y1, 0, rows1);
				y2 = __clamp(y2, 0, rows1);

				for (c = 0; c < columns; c++) {
					i = _r + c;

					x1 = c - nn;
					x2 = c + nn;
					x1 = __clamp(x1, 0, columns1);
					x2 = __clamp(x2, 0, columns1);

					ws = (x2 - x1) * (y2 - y1);

					i1 = x1 + y1 * columns;
					i2 = x2 + y1 * columns;
					i3 = x2 + y2 * columns;
					i4 = x1 + y2 * columns;

					area = Imgi[i3] - Imgi[i2] - Imgi[i4] + Imgi[i1];
					area2 = Imgi2[i3] - Imgi2[i2] - Imgi2[i4] + Imgi2[i1];

					u = area / ws;
					s2 = (area2 - u * u) / ws;

					s = sqrt(s2);

					/* adaptive threshold */
					Th = u * (1 + (k * ((s / max_s) - 1)));

					if (Img[i] > Th) {
						BW[i] = 255.0;
					} else {
						BW[i] = 0.0;

					}
				}
			}
			// </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Check Bottom Border">
			for (r = rows - nn, _r = r * columns; r < rows; r++, _r += columns) {

				y1 = r - nn;
				y2 = r + nn;

				y1 = __clamp(y1, 0, rows1);
				y2 = __clamp(y2, 0, rows1);

				for (c = 0; c < columns; c++) {
					i = _r + c;

					x1 = c - nn;
					x2 = c + nn;

					x1 = __clamp(x1, 0, columns1);
					x2 = __clamp(x2, 0, columns1);

					ws = (x2 - x1) * (y2 - y1);

					i1 = x1 + y1 * columns;
					i2 = x2 + y1 * columns;
					i3 = x2 + y2 * columns;
					i4 = x1 + y2 * columns;

					area = Imgi[i3] - Imgi[i2] - Imgi[i4] + Imgi[i1];
					area2 = Imgi2[i3] - Imgi2[i2] - Imgi2[i4] + Imgi2[i1];

					u = area / ws;
					s2 = (area2 - u * u) / ws;

					s = sqrt(s2);

					/* adaptive threshold */
					Th = u * (1 + (k * ((s / max_s) - 1)));

					if (Img[i] > Th) {
						BW[i] = 255.0;
					} else {
						BW[i] = 0.0;

					}
				}
			}  // </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Check Left Border">
			for (r = nn, _r = r * columns; r < rows - nn; r++, _r += columns) {

				y1 = r - nn;
				y2 = r + nn;

				y1 = __clamp(y1, 0, rows1);
				y2 = __clamp(y2, 0, rows1);

				for (c = 0; c < nn; c++) {
					i = _r + c;

					x1 = c - nn;
					x2 = c + nn;

					x1 = __clamp(x1, 0, columns1);
					x2 = __clamp(x2, 0, columns1);

					ws = (x2 - x1) * (y2 - y1);

					i1 = x1 + y1 * columns;
					i2 = x2 + y1 * columns;
					i3 = x2 + y2 * columns;
					i4 = x1 + y2 * columns;

					area = Imgi[i3] - Imgi[i2] - Imgi[i4] + Imgi[i1];
					area2 = Imgi2[i3] - Imgi2[i2] - Imgi2[i4] + Imgi2[i1];

					u = area / ws;
					s2 = (area2 - u * u) / ws;

					s = sqrt(s2);

					/* adaptive threshold */
					Th = u * (1 + (k * ((s / max_s) - 1)));

					if (Img[i] > Th) {
						BW[i] = 255.0;
					} else {
						BW[i] = 0.0;

					}

				}
			}  // </editor-fold>

			// <editor-fold defaultstate="collapsed" desc="Check Right Border">
			for (r = nn, _r = r * columns; r < rows - nn; r++, _r += columns) {

				y1 = r - nn;
				y2 = r + nn;

				y1 = __clamp(y1, 0, rows1);
				y2 = __clamp(y2, 0, rows1);

				for (c = columns - nn; c < columns; c++) {
					i = _r + c;

					x1 = c - nn;
					x2 = c + nn;

					x1 = __clamp(x1, 0, columns1);
					x2 = __clamp(x2, 0, columns1);

					ws = (x2 - x1) * (y2 - y1);

					i1 = x1 + y1 * columns;
					i2 = x2 + y1 * columns;
					i3 = x2 + y2 * columns;
					i4 = x1 + y2 * columns;

					area = Imgi[i3] - Imgi[i2] - Imgi[i4] + Imgi[i1];
					area2 = Imgi2[i3] - Imgi2[i2] - Imgi2[i4] + Imgi2[i1];

					u = area / ws;
					s2 = (area2 - u * u) / ws;

					s = sqrt(s2);

					/* adaptive threshold */
					Th = u * (1 + (k * ((s / max_s) - 1)));

					if (Img[i] > Th) {
						BW[i] = 255.0;
					} else {
						BW[i] = 0.0;

					}
				}
			}  // </editor-fold>

			mem_free(Imgi);
			mem_free(Imgi2);
		}

	}
}
#endif /* IMPL_THRESHOLD_ADPTIVE_HPP */
