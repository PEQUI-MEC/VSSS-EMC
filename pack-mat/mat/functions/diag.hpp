/*
 * diag.hpp
 *
 *  Created on: Feb 23, 2013
 *      Author: gustavo
 */

#ifndef DIAG_HPP_
#define DIAG_HPP_

#include "../Matrix.hpp"
#include "MatrixFunctionBase.hpp"

#include <util/mem/mem_access.hpp>

namespace mat {

	template<typename T> class Diag: public MatrixFunctionBase<Diag<T> > {

		public:
			const Matrix<T>& M;

			Diag(const Matrix<T>& Min) :
					M(Min) {
			}

			template<typename Tout> bool apply(Matrix<Tout>& A) const {

				if (M.is_vector()) {
					uint dim = M.size;
					A.set_size(dim, dim);
					A.fill(0);

					for (uint i = 0; i < dim; ++i) {
						mem_access::rwp(A.data)[i * A.stride + i] = M.data[i];
					}

				} else {

					uint dim = (M.nrows < M.ncols) ? M.nrows : M.ncols;
					A.set_size(dim, 1);
					for (uint i = 0; i < dim; ++i) {
						mem_access::rwp(A.data)[i] = M.data[i * M.stride + i];
					}

				}
				return true;

			}

	};

	template<typename T> Diag<T> diag(const Matrix<T>& M) {
		Diag<T> mf(M);
		return mf;
	}

}
#endif /* DIAG_HPP_ */
