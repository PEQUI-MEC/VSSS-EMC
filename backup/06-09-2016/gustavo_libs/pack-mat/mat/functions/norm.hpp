/*
 * norm.hpp
 *
 *  Created on: Oct 18, 2012
 *      Author: gustavo
 */

#ifndef IMP_NORM_HPP
#define IMP_NORM_HPP

#include <string.h>

#include "../Matrix.hpp"

#include <impl/math/mblaop.hpp>

namespace mat {

//		namespace MatrixFunctionType {
//			struct Norm;
//		}
//
//		template<typename T> class MatrixOperator<T, MatrixFunctionType::Norm> {
//			private:
//				Matrix<T> M;
//			public:
//				MatrixOperator(Matrix<T>& orig) {
//
//					M.refTo(orig);
//				}
//
//				void eval(math::Matrix<T>& MM) const {
//
//					Matrix<T> A;
//					A.copy(M);
//
//					MM.set_size(M.nrows, M.ncols);
//					//impl::math::inverse_GaussJordan(A.nrows, A.data, A.stride, MM.data, MM.stride);
//				}
//		};

	template<typename T> double norm(Matrix<T>& A, double p = 2.0) {

		if (A.is_number()) {

			return (T) A(0);

		} else if (A.is_vector()) {
			// is a vector
			if (p == 1) return impl::math::v_norm1(A.size, A.data, A.stride);

			if (p == 2) {
				return impl::math::v_norm2(A.size, A.data, 1);
			}

			return impl::math::v_normp(A.size, A.data, 1, p);

		} else {

			if (p == 1) return impl::math::m_norm1(A.nrows, A.ncols, A.data, A.stride);

			if (p == 2) {
				//__warning( "Matrix Norm-2 not implemented! Using Frobenius Norm!\n" );
				return impl::math::m_norm_frobenius(A.nrows, A.ncols, A.data, A.stride);
			}

			__warning("Interpreting the Matrix as a vector.\n");
			return impl::math::m_pnorm(A.nrows, A.ncols, A.data, A.stride, p);
		}

	}

	template<typename T> T norm(Matrix<T>& A, const char * type) {

		T pNorm;

		if (A.size == 1) {

			return (T) A(0);

		} else {

			if (strcmp("fro", type) == 0) {

				pNorm = impl::math::m_norm_frobenius(A.nrows, A.ncols, A.data, A.stride);

			} else if (strcmp("inf", type) == 0) {

				pNorm = impl::math::m_norm_inf(A.nrows, A.ncols, A.data, A.stride);
			}
		}

		return pNorm;
	}
}

#endif /* IMP_NORM_HPP */
