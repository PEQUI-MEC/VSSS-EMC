/*
 * transpose.hpp
 *
 *  Created on: Aug 11, 2013
 *      Author: gtl
 */

#ifndef TRANSPOSE_HPP_
#define TRANSPOSE_HPP_

#include "MatrixFunctionBase.hpp"

#include "../impl/m_blaop.hpp"

namespace mat {

	template<typename T> class Matrix;
	template<typename T> class MatrixView;

	template<typename T> class MatrixTranspose: public MatrixFunctionBase<MatrixTranspose<T> > {
			const Matrix<T>& M;
		public:
			MatrixTranspose(const Matrix<T>& M) :
					M(M) {
			}

			template<class Tout> void apply(Matrix<Tout>& A) const {

				A.set_size(M.ncols, M.nrows);

				impl::matops::m_transpose(  //
						M.nrows, M.ncols, M.data, M.stride,  //
						A.data, A.stride);
			}

			template<class Tout> void apply(MatrixView<Tout>& A) const {

				__assert_msg((A.nrows == M.nrows && A.ncols == M.ncols), INVALID_MATRIX_DIMENSION);

				impl::matops::m_transpose(  //
						M.nrows, M.ncols, M.data, M.stride,  //
						A.data, A.stride);
			}

	};

	template<typename T> MatrixTranspose<T> transpose(Matrix<T>& M) {
		return MatrixTranspose<T>(M);
	}
}

#endif /* TRANSPOSE_HPP_ */
