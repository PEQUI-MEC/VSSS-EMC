/*
 * svd.hpp
 *
 *  Created on: Aug 10, 2013
 *      Author: gtl
 */

#ifndef SVD_HPP_
#define SVD_HPP_

#include <impl/math/svd.hpp>

#include "../Matrix.hpp"

namespace mat {

	template<typename Tin> class SVD: public MatrixFunctionBase<SVD<Tin> > {

		public:
			const Matrix<Tin>& M;

			SVD(const Matrix<Tin>& Min) :
					M(Min) {
			}

			template<typename Tout> bool apply(Matrix<Tout>& S) const {

				cout << "S = svd()\n";
				return true;
			}

			template<typename Tout> bool apply(Matrix<Tout>& U, Matrix<Tout>& S, Matrix<Tout>& V) const {

				int mn = __min(M.nrows, M.ncols);

				U.set_size(M.nrows, mn);
				S.set_size(mn, 1);
				V.set_size(M.ncols, mn);

				int r = impl::math::svd(M.nrows, M.ncols, 1, 1, M.data, U.data, S.data, V.data);

				if (0 == r) return true;

				__warning("SVD Function: %d-th singular value has not been determined after 30 iterations!\n", r);

				return false;
			}

	};

	template<typename Tin> SVD<Tin> svd(const Matrix<Tin>& M) {
		SVD<Tin> mf(M);
		return mf;
	}

}

#endif /* SVD_HPP_ */
