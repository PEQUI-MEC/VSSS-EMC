/*
 * abop.hpp
 *
 *  Created on: Nov 4, 2012
 *      Author: gustavo
 */

#ifndef IMPL_ASTAT_HPP
#define IMPL_ASTAT_HPP

#include <math.h>

namespace impl {
	namespace math {

		template<typename T> inline T a_max( int rows, int columns, T * A, int sta, int * mr = 0, int * mc = 0 ) {

			int r, c;
			int _mr, _mc;

			T MAX = *A;
			_mr = 0;
			_mc = 0;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					if (*A > MAX) {
						MAX = *A;
						_mr = r;
						_mc = c;
					}
					A++;
				}
				A += (sta - columns);
			}

			if (mr) *mr = _mr;
			if (mc) *mc = _mc;

			return MAX;
		}

		template<typename T> inline T a_min( int rows, int columns, T * A, int sta, int * mr = 0, int * mc = 0 ) {

			int r, c;
			int _mr, _mc;

			T MIN = *A;
			_mr = 0;
			_mc = 0;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					if (*A < MIN) {
						MIN = *A;
						_mr = r;
						_mc = c;
					}
					A++;
				}
				A += (sta - columns);
			}

			if (mr) *mr = _mr;
			if (mc) *mc = _mc;

			return MIN;
		}

		template<typename T> inline T a_sum( int rows, int columns, T * A, int sta ) {

			int r, c;
			T sum = 0;
			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					sum += *A;
					A++;
				}
				A += (sta - columns);
			}

			return sum;
		}

		template<typename T> inline double a_mean( int rows, int columns, T * A, int sta ) {

			T sum = a_sum( rows, columns, A, sta );
			return sum / (double) (rows * columns);
		}

		template<typename T> inline double a_var( int rows, int columns, T * A, int sta ) {

			int r, c;

			T var = 0;

			double mean = a_mean( rows, columns, A, sta );

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					T d = *A - mean;
					var += d * d;

					A++;
				}
				A += (sta - columns);
			}

			return var / (double) (rows * columns - 1);

		}

		template<typename T> inline void a_mean_var( int rows, int columns, T * A, int sta, double * mean, double * var ) {

			int r, c;

			*mean = a_mean( rows, columns, A, sta );
			*var = 0;
			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					T d = *A - *mean;
					(*var) += d * d;

					A++;
				}
				A += (sta - columns);
			}

			*var = *var / (double) (rows * columns - 1);

		}

		template<typename T> inline double a_std( int rows, int columns, T * A, int sta ) {

			double var = a_var( rows, columns, A, sta );
			return sqrt( var );

		}

	}
}
#endif /* IMPL_ASTAT_HPP */
