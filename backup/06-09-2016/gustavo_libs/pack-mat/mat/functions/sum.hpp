/*
 * sum.hpp
 *
 *  Created on: Aug 19, 2013
 *      Author: gtl
 */

#ifndef SUM_HPP_
#define SUM_HPP_

#include "../Matrix.hpp"
#include "MatrixFunctionBase.hpp"

#include <impl/math/mstat.hpp>

namespace mat {

	template<typename T> class MatrixSum: public MatrixFunctionBase<MatrixSum<T> > {
			const Matrix<T>& M;
			int dim;

		public:
			MatrixSum(const Matrix<T>& M, int dim) :
					M(M), dim(dim) {
			}

			bool apply(Matrix<T>& MMatrixSum) const {

				switch (dim) {
				case 1:
					MMatrixSum.set_size(1, M.ncols);
					impl::matops::max_col(M.nrows, M.ncols, M.data, M.stride, MMatrixSum.data);
					break;
				case 2:
					MMatrixSum.set_size(M.nrows, 1);
					impl::matops::max_row(M.nrows, M.ncols, M.data, M.stride, MMatrixSum.data);
				}

				return true;
			}

			bool apply(Matrix<T>& MMatrixSum, Matrix<int>& Arg) const {

				switch (dim) {
				case 1:
					MMatrixSum.set_size(1, M.ncols);
					Arg.set_size(1, M.ncols);
					impl::matops::sum(M.nrows, M.ncols, M.data, M.stride, MMatrixSum.data, MMatrixSum.stride);
					break;
				case 2:
					MMatrixSum.set_size(M.nrows, 1);
					Arg.set_size(M.nrows, 1);
					impl::matops::sum_row(M.nrows, M.ncols, M.data, M.stride, MMatrixSum.data, MMatrixSum.stride);
				}

				return true;
			}
	};

	template<typename T> MatrixSum<T> sum(const Matrix<T>& M, int dim = 1) {
		return MatrixSum<T>(M, dim);
	}
}

#endif /* SUM_HPP_ */
