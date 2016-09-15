/*
 * m_stat.hpp
 *
 *  Created on: Apr 21, 2013
 *      Author: gustavo
 */

#ifndef M_STAT_HPP_
#define M_STAT_HPP_

#include "basic_operations.hpp"

namespace impl {
	namespace matops {

		template<typename Tin, typename Tout> inline void sum(int rows, int columns, const Tin * A, int sta, Tout * S, int sts) {

			int r, c, _r;

			fill(1, columns, S, sts, (Tout) 0);

			for (r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (c = 0; c < columns; ++c) {
					S[c] += A[_r + c];
				}
			}

		}

		template<typename Tin, typename Tout> inline void sum_row(int rows, int columns, const Tin * A, int sta, Tout * S, int sts) {

			int r, c, _r;

			fill(rows, 1, S, sts, (Tout) 0);

			for (r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (c = 0; c < columns; ++c) {
					S[r] += A[_r + c];
				}
			}

		}

		template<typename T> inline void max_col(  //
				int rows, int columns, T * A, int sta,  //
				T * max,  //
				int * i_max  //
				) {

			for (int i = 0; i < columns; ++i) {
				max[i] = A[i];
				i_max[i] = 0;
			}
			for (int r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (int c = 0; c < columns; ++c) {
					T a = A[_r + c];
					if (a > max[c]) {
						max[c] = a;
						i_max[c] = r;
					}
				}
			}

		}

		template<typename T> inline void max_row(  //
				int rows, int columns, T * A, int sta,  //
				T * max,  //
				int * i_max  //
				) {

			for (int i = 0; i < rows; ++i) {
				max[i] = A[i * columns];
				i_max[i] = 0;
			}
			for (int r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (int c = 0; c < columns; ++c) {
					T a = A[_r + c];
					if (a > max[r]) {
						max[r] = a;
						i_max[r] = c;
					}
				}
			}

		}

		template<typename T> inline void max_col(  //
				int rows, int columns, T * A, int sta,  //
				T * max  //
				) {

			for (int i = 0; i < columns; ++i) {
				max[i] = A[i];
			}
			for (int r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (int c = 0; c < columns; ++c) {
					T a = A[_r + c];
					if (a > max[c]) {
						max[c] = a;
					}
				}
			}

		}

		template<typename T> inline void max_row(  //
				int rows, int columns, T * A, int sta,  //
				T * max  //
				) {

			for (int i = 0; i < rows; ++i) {
				max[i] = A[i * columns];
			}
			for (int r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (int c = 0; c < columns; ++c) {
					T a = A[_r + c];
					if (a > max[r]) {
						max[r] = a;
					}
				}
			}

		}

		template<typename T> inline void min_col(  //
				int rows, int columns, T * A, int sta,  //
				T * min,  //
				int * i_min  //
				) {

			for (int i = 0; i < columns; ++i) {
				min[i] = A[i];
				i_min[i] = 0;
			}
			for (int r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (int c = 0; c < columns; ++c) {
					T a = A[_r + c];
					if (a < min[c]) {
						min[c] = a;
						i_min[c] = r;
					}
				}
			}

		}

		template<typename T> inline void min_row(  //
				int rows, int columns, T * A, int sta,  //
				T * min,  //
				int * i_min  //
				) {

			for (int i = 0; i < rows; ++i) {
				min[i] = A[i * columns];
				i_min[i] = 0;
			}
			for (int r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (int c = 0; c < columns; ++c) {
					T a = A[_r + c];
					if (a < min[r]) {
						min[r] = a;
						i_min[r] = c;
					}
				}
			}

		}

		template<typename T> inline void min_col(  //
				int rows, int columns, T * A, int sta,  //
				T * min  //
				) {

			for (int i = 0; i < columns; ++i) {
				min[i] = A[i];
			}
			for (int r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (int c = 0; c < columns; ++c) {
					T a = A[_r + c];
					if (a < min[c]) {
						min[c] = a;
					}
				}
			}

		}

		template<typename T> inline void min_row(  //
				int rows, int columns, T * A, int sta,  //
				T * max  //
				) {

			for (int i = 0; i < rows; ++i) {
				max[i] = A[i * columns];
			}
			for (int r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (int c = 0; c < columns; ++c) {
					T a = A[_r + c];
					if (a < max[r]) {
						max[r] = a;
					}
				}
			}

		}

		template<typename T> inline T amax(int rows, int columns, T * A, int sta, int * r_max = 0, int * c_max = 0) {

			T max_val;

			int r, c;
			int _r, _c;

			int inc = (sta - columns);

			max_val = *A;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					if (*A > max_val) {
						max_val = *A;
						_r = r;
						_c = c;
					}

					A++;
				}
				A += inc;
			}

			if (r_max) *r_max = _r;
			if (c_max) *c_max = _c;

			return max_val;

		}

		template<typename T> inline T amin(int rows, int columns, T * A, int sta, int * r_min = 0, int * c_min = 0) {

			T min_val;

			int r, c;
			int _r, _c;

			int inc = (sta - columns);

			min_val = *A;
			_r = 0;
			_c = 0;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					if (*A < min_val) {
						min_val = *A;
						_r = r;
						_c = c;
					}

					A++;
				}
				A += inc;
			}

			if (r_min) *r_min = _r;
			if (c_min) *c_min = _c;

			return min_val;

		}

		template<typename T> inline void amax_min(int rows, int columns, T * A, int sta, T * max_val, T * min_val, int * r_max = 0, int * c_max = 0, int * r_min = 0, int * c_min =
				0) {

			int r, c;
			int Mr, Mc;
			int mr = 0, mc = 0;

			int inc = (sta - columns);

			*max_val = *A;
			*min_val = *A;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					if (*A > *max_val) {
						*max_val = *A;
						Mr = r;
						Mc = c;
					}

					if (*A < *min_val) {
						*min_val = *A;
						mr = r;
						mc = c;
					}

					A++;
				}
				A += inc;
			}

			if (r_max) *r_max = Mr;
			if (c_max) *c_max = Mc;

			if (r_min) *r_min = mr;
			if (c_min) *c_min = mc;

		}

	}
}

#endif /* M_STAT_HPP_ */
