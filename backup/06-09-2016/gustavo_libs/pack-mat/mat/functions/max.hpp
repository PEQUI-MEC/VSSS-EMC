/*
 * max.hpp
 *
 *  Created on: Aug 18, 2013
 *      Author: gtl
 */

#ifndef MAX_HPP_
#define MAX_HPP_

#include "../Matrix.hpp"
#include "MatrixFunctionBase.hpp"

#include <impl/mat/m_stat.hpp>

namespace mat {

	template<typename T> class Max: public MatrixFunctionBase<Max<T> > {
			const Matrix<T>& M;
			int dim;

		public:
			Max(const Matrix<T>& M, int dim) :
					M(M), dim(dim) {
			}

			bool apply(Matrix<T>& MMax) const {

				switch (dim) {
				case 1:
					MMax.set_size(1, M.ncols);
					impl::matops::max_col(M.nrows, M.ncols, M.data, M.stride, MMax.data);
					break;
				case 2:
					MMax.set_size(M.nrows, 1);
					impl::matops::max_row(M.nrows, M.ncols, M.data, M.stride, MMax.data);
				}

				return true;
			}

			bool apply(Matrix<T>& MMax, Matrix<int>& Arg) const {

				switch (dim) {
				case 1:
					MMax.set_size(1, M.ncols);
					Arg.set_size(1, M.ncols);
					impl::matops::max_col(M.nrows, M.ncols, M.data, M.stride, MMax.data, Arg.data);
					break;
				case 2:
					MMax.set_size(M.nrows, 1);
					Arg.set_size(M.nrows, 1);
					impl::matops::max_row(M.nrows, M.ncols, M.data, M.stride, MMax.data, Arg.data);
				}

				return true;
			}
	};

	template<typename T> Max<T> max(const Matrix<T>& M, int dim = 1) {
		return Max<T>(M, dim);
	}
}

#endif /* MAX_HPP_ */
