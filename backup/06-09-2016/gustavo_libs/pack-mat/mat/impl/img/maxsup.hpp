/*
 * nmaxsup.hpp
 *
 *  Created on: Sep 21, 2012
 *      Author: gustavo
 */

#ifndef IMPL_NMAXSUP_HPP
#define IMPL_NMAXSUP_HPP

#include "../util/mem.h"
#include "../math/mbop.hpp"

namespace impl {
	namespace img {

		/**
		 *
		 * @param rows number of rows
		 * @param columns number of columns
		 * @param data_in input data
		 * @param st_in input data stride
		 * @param square_ray radious of the region
		 * @param data_out output data. The non maximal suppression result.
		 * @param st_out stride of output data
		 * @return
		 */
		template<typename Tin, typename Tout> int nonmaxsup( int rows, int columns, Tin * data_in, int st_in, int square_ray, Tout * nms, int st_nms ) {

			int r, c, _r, i;
			long int qtd;

			Tin MAX;

			int rf, cf;

			// store index of each row
			int * r_shift = mem_allocn(rows, int);
			for (r = 0, c = 0; r < rows; ++r, c += st_in)
				r_shift[r] = c;

			qtd = 0;
			for (r = square_ray, _r = r * st_in; r < rows - square_ray; ++r, _r += st_in) {
				for (c = square_ray; c < columns - square_ray; ++c) {

					i = _r + c;

					MAX = data_in[i];

					register int mr;
					register int mc;
					for (mr = -square_ray; mr <= square_ray; ++mr) {

						rf = r + mr;

						for (mc = -square_ray; mc <= square_ray; ++mc) {

							cf = c + mc;

							register int d = r_shift[rf] + cf;

							if (data_in[d] > MAX) MAX = data_in[d];

						}
					}

					if (data_in[i] == MAX) {
						nms[i] = (Tout) MAX;
						qtd++;
					} else {
						nms[i] = 0;
					}
				}
			}
			mem_free( r_shift );
			return qtd;
		}

		/**
		 *
		 * Maximum values are filled with value 1 and 0 otherwise.
		 *
		 * @param rows number of rows
		 * @param columns number of columns
		 * @param data_in input data
		 * @param st_in input data stride
		 * @param square_ray radious of the region
		 * @param data_out output data. The non maximal suppression result.
		 * @param st_out stride of output data
		 * @param th minimal value
		 * @return
		 */
		template<typename Tin, typename Tout> int nonmaxsup_th( int rows, int columns, Tin * data_in, int st_in, int square_ray, Tout * nms, int st_nms, double th ) {

			int r, c, _r, i;
			long int qtd;

			Tin MAX;

			int rf, cf;

			// store index of each row
			int * r_shift = mem_allocn(rows, int);
			for (r = 0, c = 0; r < rows; ++r, c += st_in)
				r_shift[r] = c;

			// cleaning the border
			math::m_set_border( rows, columns, nms, st_nms, square_ray, 0 );

			qtd = 0;
			for (r = square_ray, _r = r * st_in; r < rows - square_ray; ++r, _r += st_in) {
				for (c = square_ray; c < columns - square_ray; ++c) {

					i = _r + c;

					if (data_in[i] < th) {
						nms[i] = 0;
						continue;
					}

					MAX = data_in[i];

					register int mr;
					register int mc;
					for (mr = -square_ray; mr <= square_ray; ++mr) {

						rf = r + mr;

						for (mc = -square_ray; mc <= square_ray; ++mc) {

							cf = c + mc;

							register int d = r_shift[rf] + cf;

							if (data_in[d] > MAX) MAX = data_in[d];

						}
					}

					if (data_in[i] == MAX) {
						nms[i] = 1;
						qtd++;
					} else {
						nms[i] = 0;
					}
				}
			}
			mem_free( r_shift );
			return qtd;
		}

		template<typename T> int nonmaxsup_feature_nonzero( int rows, int columns, T * Mat, int st_mat, int square_ray, T * NMS, int st_nms ) {

			int r, c, _r, i;
			long int qtd;

			T MAX;

			int rf, cf;

			// store index of each row
			int * r_shift = mem_allocn(rows, int);
			for (r = 0, c = 0; r < rows; ++r, c += st_mat)
				r_shift[r] = c;

			qtd = 0;
			for (r = square_ray, _r = r * st_mat; r < rows - square_ray; ++r, _r += st_mat) {
				for (c = square_ray; c < columns - square_ray; ++c) {

					i = _r + c;

					if (Mat[i] == 0) continue;

					MAX = Mat[i];

					register int mr;
					register int mc;
					for (mr = -square_ray; mr <= square_ray; ++mr) {

						rf = r + mr;

						for (mc = -square_ray; mc <= square_ray; ++mc) {

							cf = c + mc;

							register int d = r_shift[rf] + cf;

							if (Mat[d] > MAX) MAX = Mat[d];

						}
					}

					if (Mat[i] == MAX) {
						NMS[i] = MAX;
						qtd++;
					}
				}
			}
			mem_free( r_shift );
			return qtd;
		}

		template<typename Ti, typename Tm, typename To> int nonmaxsup_feature_mask( //
				int rows, int columns, Ti * Mat, int st_mat, //
				int square_ray, //
				Tm * Mask, //
				To * NMS ) {

			int r, c, _r, i;
			long int qtd;

			Ti MAX;

			int rf, cf;

			// store index of each row
			int * r_shift = mem_allocn(rows, int);
			for (r = 0, c = 0; r < rows; ++r, c += st_mat)
				r_shift[r] = c;

			qtd = 0;
			for (r = square_ray, _r = r * st_mat; r < rows - square_ray; ++r, _r += st_mat) {
				for (c = square_ray; c < columns - square_ray; ++c) {

					i = _r + c;

					if (Mask[i] == 0) continue;

					MAX = Mat[i];

					register int mr;
					register int mc;
					for (mr = -square_ray; mr <= square_ray; ++mr) {

						rf = r + mr;

						for (mc = -square_ray; mc <= square_ray; ++mc) {

							cf = c + mc;

							register int d = r_shift[rf] + cf;

							if (Mat[d] > MAX) MAX = Mat[d];

						}
					}

					if (Mat[i] == MAX) {
						NMS[i] = MAX;
						qtd++;
					}
				}
			}
			mem_free( r_shift );
			return qtd;
		}

	}
}

#endif /* IMPL_NMAXSUP_HPP */
