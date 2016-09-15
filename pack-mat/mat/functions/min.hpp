/*
 * min.hpp
 *
 *  Created on: Aug 18, 2013
 *      Author: gtl
 */

#ifndef MIN_HPP_
#define MIN_HPP_

#include "../Matrix.hpp"
#include "MatrixFunctionBase.hpp"

#include <impl/math/mstat.hpp>

namespace mat {

	template<typename T> class Min: public MatrixFunctionBase<Min<T> > {
			const Matrix<T>& M;
			int dim;

		public:
			Min(const Matrix<T>& M, int dim) :
					M(M), dim(dim) {
			}

			bool apply(Matrix<T>& MMin) const {

				switch (dim) {
				case 1:
					MMin.set_size(1, M.ncols);
					impl::matops::min_col(M.nrows, M.ncols, M.data, M.stride, MMin.data);
					break;
				case 2:
					MMin.set_size(M.nrows, 1);
					impl::matops::min_row(M.nrows, M.ncols, M.data, M.stride, MMin.data);
				}

				return true;
			}

			bool apply(Matrix<T>& MMin, Matrix<int>& Arg) const {

				switch (dim) {
				case 1:
					MMin.set_size(1, M.ncols);
					Arg.set_size(1, M.ncols);
					impl::matops::min_col(M.nrows, M.ncols, M.data, M.stride, MMin.data, Arg.data);
					break;
				case 2:
					MMin.set_size(M.nrows, 1);
					Arg.set_size(M.nrows, 1);
					impl::matops::min_row(M.nrows, M.ncols, M.data, M.stride, MMin.data, Arg.data);
				}

				return true;
			}
	};

	template<typename T> Min<T> min(const Matrix<T>& M, int dim = 1) {
		return Min<T>(M, dim);
	}
}

#endif /* MIN_HPP_ */
