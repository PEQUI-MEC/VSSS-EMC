/*
 * randn.hpp
 *
 *  Created on: May 18, 2013
 *      Author: gustavo
 */

#ifndef RANDN_HPP_
#define RANDN_HPP_

#include <math.h>
#include <stdlib.h>

#include "MatrixFunctionBase.hpp"

#include "../../math/rng.hpp"

namespace mat {
	class RandN: public MatrixFunctionBase<RandN> {
		private:
			int nrows;
			int ncols;
			double mean, std;
		public:

			RandN( int rows, int cols, double mean, double std ) :
					nrows( rows ), ncols( cols ), mean( mean ), std( std ) {
			}

			template<typename T> bool apply( Matrix<T>& M ) const {
				M.set_size( nrows, ncols );

				for (int r = 0; r < M.nrows; ++r) {
					for (int c = 0; c < ncols; ++c) {

						double d = math::rng::randn();
						M( r, c ) = (mean + d * (std));
					}
				}

				return true;
			}

	};

	RandN randn( int rows, int cols, double mean = 0, double std = 1 ) {
		RandN fn( rows, cols, mean, std );
		return fn;
	}

}

#endif /* RANDN_HPP_ */
