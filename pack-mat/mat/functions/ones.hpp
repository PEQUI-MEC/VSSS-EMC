/*
 * ones.hpp
 *
 *  Created on: Feb 23, 2013
 *      Author: gustavo
 */

#ifndef ONES_HPP_
#define ONES_HPP_

#include "MatrixFunctionBase.hpp"

namespace mat {

	class Ones: public MatrixFunctionBase<Ones> {
		private:
			int nrows;
			int ncols;
		public:

			Ones( int rows, int cols ) :
					nrows( rows ), ncols( cols ) {
			}

			template<typename T> bool apply( Matrix<T>& M ) const {
				M.set_size( nrows, ncols );
				M.fill( 1 );
				return true;
			}

	};

	Ones ones( int rows, int cols ) {
		Ones fn( rows, cols );
		return fn;
	}

	Ones ones( int dim ) {
		Ones fn( dim, dim );
		return fn;
	}

}
#endif /* ONES_HPP_ */
