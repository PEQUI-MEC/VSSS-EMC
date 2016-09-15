/*
 * argmin.hpp
 *
 *  Created on: Nov 2, 2012
 *      Author: gustavo
 */

#ifndef IMP_ARGMIN_HPP
#define IMP_ARGMIN_HPP

#include <impl/math/mbop.hpp>

#include "../Matrix.hpp"
#include "MatrixFunctionBase.hpp"

namespace mat {

	template<typename T> class ArgMin: public MatrixFunctionBase<ArgMin<T> > {
			const Matrix<T>& M;

		public:
			ArgMin(const Matrix<T>& M) :
					M(M) {
			}

	};

	template<typename T> int argmin(const Matrix<T>& M) {

		int r, c;
		impl::math::m_min(M.nrows, M.ncols, M.data, M.stride, &r, &c);
		return r * M.stride + c;
	}

	template<typename T> T argmin(Matrix<T>& M, int * r, int * c) {

		return impl::math::m_min(M.rows, M.columns, M.data, M.stride, r, c);

	}

}

#endif /* IMP_ARGMIN_HPP */
