/*
 * rand.hpp
 *
 *  Created on: May 18, 2013
 *      Author: gustavo
 */

#ifndef RAND_HPP_
#define RAND_HPP_

#include <math.h>
#include <stdlib.h>

#include "MatrixFunctionBase.hpp"

namespace mat {
	class Rand: public MatrixFunctionBase<Rand> {
		private:
			int n_rows;
			int n_columns;
			double min, max;
		public:

			Rand( int rows, int cols, double min, double max ) :
					n_rows( rows ), n_columns( cols ), min( min ), max( max ) {
			}

			template<typename T> bool apply( Matrix<T>& M ) const {
				M.set_size( n_rows, n_columns );

				double diff = max - min;

				for (int r = 0; r < M.n_rows; ++r) {
					for (int c = 0; c < n_columns; ++c) {

						double d = (double) random() / ((double) RAND_MAX + 1);
						M( r, c ) = (min + d * (diff));
					}
				}

				return true;
			}

	};

	Rand rand( int rows, int cols, double min = 0, double max = 1 ) {
		Rand fn( rows, cols, min, max );
		return fn;
	}

}

#endif /* RAND_HPP_ */
