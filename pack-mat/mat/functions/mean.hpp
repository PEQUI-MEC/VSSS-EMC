/*
 * mean.hpp
 *
 *  Created on: Jul 1, 2013
 *      Author: gustavo
 */

#ifndef MEAN_HPP_
#define MEAN_HPP_

#include "MatrixFunctionBase.hpp"

#include <impl/math/mstat.hpp>

namespace mat {

	template<typename T> class Mean: public MatrixFunctionBase<Mean<T> > {
		private:
			const Matrix<T>& M;
			int dim;
		public:

			Mean(const Matrix<T>& M, int dim) :
					M(M), dim(dim) {
			}

			bool apply(double& u) const {
				return true;
			}

			template<typename Tu> bool apply(Matrix<Tu>& U) const {

				if (dim == 1) {
					U.set_size(1, M.ncols);
					impl::math::m_mean(M.nrows, M.ncols, M.data, M.stride, U.data, 1);
				} else if (dim == 2) {
					U.set_size(M.nrows, 1);
					impl::math::m_mean_row(M.nrows, M.ncols, M.data, M.stride, U.data, U.stride);
				}

				return true;
			}

	};

	template<typename T> Mean<T> mean(const Matrix<T>& M, int dim = 1) {
		Mean<T> fn(M, dim);
		return fn;
	}

}

#endif /* MEAN_HPP_ */
