/*
 * inv.hpp
 *
 *  Created on: Oct 17, 2012
 *      Author: gustavo
 */

#ifndef IMP_INV_HPP
#define IMP_INV_HPP

#include "../Matrix.hpp"
#include "MatrixFunctionBase.hpp"

#include <impl/util/msg.h>
#include <impl/math/inverse.hpp>

namespace mat {

	template<typename T> class MatrixInverse: public MatrixFunctionBase<MatrixInverse<T> > {
		private:
			const Matrix<T>& M;
		public:
			MatrixInverse(const Matrix<T>& orig) :
					M(orig) {
				__assert_msg(orig.nrows == orig.ncols, "Matrix is not square!\n");
//				M.refTo(orig);
			}

			bool apply(Matrix<T>& MM) const {

				Matrix<T> A;
				A = M;

				MM.set_size(M.nrows, M.ncols);
				impl::math::inverse_GaussJordan(A.nrows, A.data, A.stride, MM.data, MM.stride);

				return true;
			}
	};

	template<typename T> MatrixInverse<T> inv(const Matrix<T>& A) {
		return MatrixInverse<T>(A);
	}

}

#endif /* IMP_INV_HPP */
