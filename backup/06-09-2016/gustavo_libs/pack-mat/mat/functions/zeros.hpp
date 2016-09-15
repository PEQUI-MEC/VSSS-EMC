/*
 * zeros.hpp
 *
 *  Created on: Feb 23, 2013
 *      Author: gustavo
 */

#ifndef ZEROS_HPP_
#define ZEROS_HPP_

#include "../Size.hpp"

#include "MatrixFunctionBase.hpp"

namespace mat {

	class Zeros: public MatrixFunctionBase<Zeros> {
		private:
			uint n_rows;
			uint n_columns;
		public:

			Zeros( uint rows, uint cols ) :
					n_rows( rows ), n_columns( cols ) {
			}

			template<typename T> bool apply( Matrix<T>& M ) const {
				M.set_size( n_rows, n_columns );
				M.fill( 0 );
				return true;
			}

			template<typename T> bool apply( MatrixView<T>& M ) const {
				M.fill( 0 );
				return true;
			}
	};

	Zeros zeros( uint rows, uint cols ) {
		Zeros mod( rows, cols );
		return mod;
	}

	Zeros zeros( const Size<2>& sz ) {
		Zeros mod( sz[0], sz[1] );
		return mod;
	}

}
#endif /* ZEROS_HPP_ */
