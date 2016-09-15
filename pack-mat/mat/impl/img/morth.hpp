/*
 * morth.hpp
 *
 *  Created on: Sep 22, 2012
 *      Author: gustavo
 */

#ifndef IMPL_MORTH_HPP
#define IMPL_MORTH_HPP

#include "../util/mem.h"

namespace impl {
	namespace img {
		namespace morth {

			template<typename T> void dilate_bw( int rows, int columns, T * mat, int st_in, int se_radius_x, int se_radius_y, bool * se, bool * dilate, int st_out ) {

				int r, c, _r, i;

				T MAX;
				int rf, cf;

				int se_rows = (se_radius_y << 1) + 1;
				int se_columns = (se_radius_x << 1) + 1;
				int se_max_idx = (se_rows * se_columns) - 1;

				int __2columns_minus_2 = (columns << 1) - 2;
				int __2rows_minus_2 = (rows << 1) - 2;

				int * st_aux = mem_allocn( rows, int );
				int inc;
				for (r = 0, inc = 0; r < rows; ++r, inc += st_in)
					st_aux[r] = inc;

				for (r = 0, _r = 0; r < rows; r++, _r += columns) {
					for (c = 0; c < columns; c++) {

						i = _r + c;

						MAX = 0;

						register int mr;
						register int mc;

						int se_idx = se_max_idx;

						for (mr = -se_radius_y; mr <= se_radius_y; ++mr) {

							rf = r + mr;

							// update rf
							if (rf < 0) rf = -rf;
							else if (rf >= rows) rf = __2rows_minus_2 - rf;

							for (mc = -se_radius_x; mc <= se_radius_x; ++mc, --se_idx) {

								if (!se[se_idx]) continue;

								cf = c + mc;
								// update cf
								if (cf < 0) cf = -cf;
								else if (cf >= columns) cf = __2columns_minus_2 - cf;

								register int d = st_aux[rf] + cf;

								if (mat[d] > 0) {
									MAX = 1;
									// force break in all for
									mr = se_radius_y + 1;
									mc = se_radius_x + 1;
									break;
								}

							}
						}

						dilate[i] = MAX;
					}
				}

				mem_free( st_aux );
			}

			template<typename Tin, typename Tout> void dilate( int rows, int columns, Tin * mat, int st_in, int se_radius_x, int se_radius_y, int * se, Tout * dilate, int st_out ) {

				int r, c, _r, i;

				Tin MAX;
				int rf, cf;

				int se_rows = (se_radius_y << 1) + 1;
				int se_columns = (se_radius_x << 1) + 1;
				int se_max_idx = (se_rows * se_columns) - 1;

				int __2columns_minus_2 = (columns << 1) - 2;
				int __2rows_minus_2 = (rows << 1) - 2;

				int * st_aux = mem_allocn( rows, int );
				int inc;
				for (r = 0, inc = 0; r < rows; ++r, inc += st_in)
					st_aux[r] = inc;

				for (r = 0, _r = 0; r < rows; r++, _r += columns) {
					for (c = 0; c < columns; c++) {

						i = _r + c;

						MAX = mat[i];

						register int mr;
						register int mc;

						int se_idx = se_max_idx;

						for (mr = -se_radius_y; mr <= se_radius_y; ++mr) {

							rf = r + mr;

							// update rf
							if (rf < 0) rf = -rf;
							else if (rf >= rows) rf = __2rows_minus_2 - rf;

							for (mc = -se_radius_x; mc <= se_radius_x; ++mc, --se_idx) {

								if (!se[se_idx]) continue;

								cf = c + mc;
								// update cf
								if (cf < 0) cf = -cf;
								else if (cf >= columns) cf = __2columns_minus_2 - cf;

								register int d = st_aux[rf] + cf;

								if (mat[d] > MAX) MAX = mat[d];

							}
						}

						dilate[i] = MAX;
					}
				}

				mem_free( st_aux );
			}

			template<typename Tin, typename Tout> void erode( int rows, int columns, Tin * mat, int st_in, int se_radius_x, int se_radius_y, int * se, Tout * erode, int st_out ) {

				int r, c, _r, i;

				Tin MIN;
				int rf, cf;

				int se_rows = (se_radius_y << 1) + 1;
				int se_columns = (se_radius_x << 1) + 1;
				int se_max_idx = (se_rows * se_columns) - 1;

				int __2columns_minus_2 = (columns << 1) - 2;
				int __2rows_minus_2 = (rows << 1) - 2;

				int * st_aux = mem_allocn( rows, int );
				int inc;
				for (r = 0, inc = 0; r < rows; ++r, inc += st_in)
					st_aux[r] = inc;

				for (r = 0, _r = 0; r < rows; r++, _r += columns) {
					for (c = 0; c < columns; c++) {

						i = _r + c;

						MIN = mat[i];

						register int mr;
						register int mc;

						int se_idx = se_max_idx;

						for (mr = -se_radius_y; mr <= se_radius_y; ++mr) {

							rf = r + mr;

							// update rf
							if (rf < 0) rf = -rf;
							else if (rf >= rows) rf = __2rows_minus_2 - rf;

							for (mc = -se_radius_x; mc <= se_radius_x; ++mc, --se_idx) {

								if (!se[se_idx]) continue;

								cf = c + mc;
								// update cf
								if (cf < 0) cf = -cf;
								else if (cf >= columns) cf = __2columns_minus_2 - cf;

								register int d = st_aux[rf] + cf;

								if (mat[d] < MIN) MIN = mat[d];

							}
						}

						erode[i] = MIN;
					}
				}

				mem_free( st_aux );
			}

		}
	}
}

#endif /* IMPL_MORTH_HPP */
