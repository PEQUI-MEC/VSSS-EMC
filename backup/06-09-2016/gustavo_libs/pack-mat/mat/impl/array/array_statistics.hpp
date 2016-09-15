/*
 * array_statistics.hpp
 *
 *  Created on: Mar 30, 2013
 *      Author: gustavo
 */

#ifndef ARRAY_STATISTICS_HPP_
#define ARRAY_STATISTICS_HPP_

#include <math.h>

namespace impl {
	namespace arraystat {

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
			int inc = (sta - columns);
			T sum = 0;
			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					sum += *A;
					A++;
				}
				A += inc;
			}

			return sum;
		}

		template<typename T> inline double a_mean( int rows, int columns, T * A, int sta ) {

			double sum = a_sum( rows, columns, A, sta );
			return sum / (double) (rows * columns);
		}

		template<typename T> inline double a_var( int rows, int columns, T * A, int sta ) {

			int r, c;
			double var = 0;
			int inc = (sta - columns);

			double mean = a_mean( rows, columns, A, sta );

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					T d = *A - mean;
					var += d * d;

					A++;
				}
				A += inc;
			}

			return var / (double) (rows * columns - 1);

		}

		template<typename T> inline void a_mean_var( int rows, int columns, T * A, int sta, double * mean, double * var ) {

			int r, c;
			int inc = (sta - columns);

			*mean = a_mean( rows, columns, A, sta );
			*var = 0;
			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					T d = *A - *mean;
					(*var) += d * d;

					A++;
				}
				A += inc;
			}

			*var = *var / (double) ((rows * columns) - 1);

		}

		template<typename T> inline double a_std( int rows, int columns, T * A, int sta ) {

			double var = a_var( rows, columns, A, sta );
			return sqrt( var );

		}

	}
}

#endif /* ARRAY_STATISTICS_HPP_ */
