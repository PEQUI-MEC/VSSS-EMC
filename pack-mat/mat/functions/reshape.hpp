/*
 * reshape.hpp
 *
 *  Created on: Oct 26, 2012
 *      Author: gustavo
 */

#ifndef IMP_RESHAPE_HPP
#define IMP_RESHAPE_HPP

#include "../Matrix.hpp"

#include <impl/math/svd.hpp>

namespace imp {
	namespace math {

		namespace MatrixFunctionType {
			struct Reshape;
		}

		template<typename T> class MatrixOperator<T, MatrixFunctionType::Reshape> {
			private:

				Matrix<T> A;
				int r_rows;
				int r_columns;

			public:

				MatrixOperator( Matrix<T>& Orig, int rows, int columns ) {

					if (rows * columns != Orig.size) {
						__error( "To RESHAPE the number of elements must not change.\n" );
					}

					this->A.refTo( Orig );
					this->r_rows = rows;
					this->r_columns = columns;
				}

				void eval( math::Matrix<T>& R ) const {

					R.set_size( r_rows, r_columns );

					int i = 0;
					for (int r = 0; r < r_rows; ++r) {
						for (int c = 0; c < r_columns; ++c) {
							R( r, c ) = A( i );
							++i;
						}
					}

				}
		};

		template<typename T> MatrixOperator<T, MatrixFunctionType::Reshape> reshape( Matrix<T>& A, int rows, int columns ) {
			MatrixOperator<T, MatrixFunctionType::Reshape> op( A, rows, columns );
			return op;
		}

	}
}

#endif /* IMP_RESHAPE_HPP */
