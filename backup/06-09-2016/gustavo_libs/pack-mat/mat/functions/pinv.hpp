/*
 * pinv.hpp
 *
 *  Created on: Apr 24, 2013
 *      Author: gustavo
 */

#ifndef PINV_HPP_
#define PINV_HPP_

#include <impl/math/svd.hpp>

#include "../Matrix.hpp"
#include "MatrixFunctionBase.hpp"

namespace mat {

	template<typename T> class PseudoInverse: public MatrixFunctionBase<PseudoInverse<T> > {
		private:

			const Matrix<T>& M;

		public:

			PseudoInverse(const Matrix<T>& M) :
					M(M) {
			}

			template<typename Tout> void eval(Matrix<Tout>& PI) const {
				PI.set_size(M.ncols, M.nrows);
				impl::math::pseudo_inverse(M.nrows, M.ncols, M.data, PI.data);

			}
	};

	template<typename T> PseudoInverse<T> pinv(const Matrix<T>& A) {
		PseudoInverse<T> op(A);
		return op;
	}

}
#endif /* PINV_HPP_ */
