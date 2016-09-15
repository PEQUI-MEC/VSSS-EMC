/*
 * svd.hpp
 *
 *  Created on: Oct 20, 2012
 *      Author: gustavo
 */

#ifndef IMPL_SVD_HPP
#define IMPL_SVD_HPP

#include "config.hpp"

#include "svd/svd_algol.hpp"
//#include "svd/svdcmp.hpp"
//#include "svd/svd_inspirit.hpp"
#include "mblaop.hpp"
#include "../util/mem.h"

namespace impl {
	namespace math {

		/**
		 * \brief Singular Value Decomposition
		 *
		 * This function realize the Singular Value Decomposition (SVD) of an
		 * arbitrary matrix m-by-n. All matrices must be pre-allocated.
		 *
		 * A[m][n]
		 * mn = min(m,n)
		 * U[m][mn]
		 * S[mn]
		 * V[mn][n]
		 *
		 *
		 * @param m number of rows in input matrix A[m][n]
		 * @param n number of columns in input matrix A[m][n]
		 * @param withu	should be set to 1 if the matrix u[][] in the decomposition is desired, and to 0 otherwise.
		 * @param withv	should be set to 1 if the matrix v[][] in thedecomposition is desired, and to 0 otherwise.
		 * @param A		A[m][n] - the rectangular input matrix mxn to be decomposed: a = u * d * v^T
		 *	            A[i][j] - the element of a with row i and column j
		 * @param U		U[m][n] - the output column-ortogonal matrix (n top eigenvectors of a * a^T)
		 *              if withu has been set to 1; U[i][j] -- the element of u with row i
		 *              and column j. Otherwise it is used as a temporary array. It may
		 *      		coincide with a. If an error exit is made, the columns of the matrix
		 *              u corresponding to indices of correct singular values should be
		 *              correct.
		 * @param Q		Q[n] - the output non-negative singular values
		 *              [the unordered diagonal elements of the diagonal matrix d]
		 *              If an error exit is made, the singular values should be correct for
		 *              indices retval+1, retval+2, ..., n.
		 * @param V		V[n][n] - the output column-orthogonal matrix (n eigenvectors of a^T * a)
		 *	            if matv has been set to 1: V[i][j] -- the element of V with row i
		 *              and column j. Otherwise it is not referenced. It may also coincide
		 *              with the matrix a if u is not needed. If an error exit is made, the
		 *				columns of V corresponding to indices of correct singular values
		 *              should be correct.
		 * @return		The output value retval is set to:
		 *      	    zero for normal return,
		 *              k if the k-th singular value has not been determined after 30 iterations, and
		 *
		 * @see svd/svd_algol.hpp
		 */
		template<typename T> int svd( int m, int n, int withu, int withv, const T * A, T * U, T * S, T * V ) {

			if (n > m) {
				T * At = mem_allocn( m*n, T );
				m_transpose( m, n, A, n, At, m );
				int r = svd_algol( n, m, withv, withu, EPS, TOL, At, S, V, U );
				mem_free( At );
				return r;
			}

			// m > n case
			return svd_algol( m, n, withu, withv, EPS, TOL, A, S, U, V );
		}

		template<typename T> int pseudo_inverse( int m, int n, T * A, T * Pinv ) {

			int mn = __min(m,n);

			T U[m * mn];
			T S[mn];
			T V[n * mn];

			int ret = impl::math::svd( m, n, 1, 1, A, U, S, V );

			if (ret != 0) return ret;

			//Pinv = V * S^{⁻1} * U'

			for (int r = 0; r < n; ++r) {
				for (int c = 0; c < m; ++c) {

					double sum = 0;
					for (int k = 0; k < mn; ++k) {

						sum += (V[r * mn + k] * U[c * mn + k]) / S[k];

					}
					Pinv[r * m + c] = sum;
				}
			}

			return 0;
		}
	}
}
#endif /* IMPL_SVD_HPP */
