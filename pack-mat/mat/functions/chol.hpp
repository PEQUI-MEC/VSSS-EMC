/*
 * chol.hpp
 *
 *  Created on: Nov 1, 2012
 *      Author: gustavo
 */

#ifndef IMP_CHOL_HPP
#define IMP_CHOL_HPP

#include "../Matrix.hpp"
#include "MatrixFunctionBase.hpp"

#include <impl/math/cholesky_factoriszation.hpp>
#include <impl/util/msg.h>

#include <string.h>

namespace mat {

	template<typename T> class CholeskyFactorization: public MatrixFunctionBase<CholeskyFactorization<T> > {

			const Matrix<T>& A;
			const char * type;

		public:

			CholeskyFactorization(const Matrix<T>& orig, const char * type) :
					A(orig), type(type) {

//				__assert_msg(orig.rows == orig.columns, "The matrix must be square.");
			}

			template<typename Tout> bool apply(Matrix<Tout>& MM) const {

				Matrix<T> R;
				int dim = A.nrows;

				R = A;

				impl::math::cholesky_factorization(dim, R.data);

				MM.set_size(dim, dim);

				if (strcmp(type, "lower") == 0) {

					for (int r = 0; r < dim; ++r) {
						for (int c = 0; c < dim; ++c) {
							if (c > r) MM(r, c) = 0;
							else MM(r, c) = R(r, c);
						}
					}

				} else if (strcmp(type, "upper") == 0) {

					for (int r = 0; r < dim; ++r) {
						for (int c = 0; c < dim; ++c) {
							if (r > c) MM(r, c) = 0;
							else MM(r, c) = R(c, r);
						}
					}

				} else {
					__error("Invalid type for CholeskyFactorization Factorization!");
				}

				return true;
			}
	};

	template<typename T> CholeskyFactorization<T> chol(const Matrix<T>& A, const char * type = "upper") {
		return CholeskyFactorization<T>(A, type);
	}

}

#endif /* IMP_CHOL_HPP */
