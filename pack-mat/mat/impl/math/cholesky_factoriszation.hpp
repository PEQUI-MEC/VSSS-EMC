/*
 * cholesky_factoriszation.hpp
 *
 *  Created on: Nov 1, 2012
 *      Author: gustavo
 */

#ifndef IMPL_CHOLESKY_FACTORISZATION_HPP
#define IMPL_CHOLESKY_FACTORISZATION_HPP

#include <math.h>

#include "math_macros.h"
#include "config.hpp"

#include "../util/msg.h"

namespace impl {
	namespace math {

		/**
		 * \brief Cholesky Factorization.
		 *
		 * Computes the Cholesky Factorization of a symmetric positive definite
		 * matrix A. A is factorized as A = L*L', where L is a lower triangular
		 * matrix.
		 *
		 * The matrix L overwrites the lower triangular part of A. The upper
		 * triangular of A is the same.
		 */
		template<typename T> void cholesky_factorization( int dim, T * A ) {

			//			const double tol = impl::math::TOL;
			const double tol = 10e-16;

			for (int k = 0; k < dim; ++k) {

				if (__abs( A[k * dim + k] ) < tol || A[k * dim + k] < 0) {
					__error( "The Matrix in the Cholesky Factorization must be positive definite." );
				}

				A[k * dim + k] = sqrt( A[k * dim + k] );
				for (int i = k + 1; i < dim; ++i) {
					A[i * dim + k] = A[i * dim + k] / A[k * dim + k];
				}
				for (int j = k + 1; j < dim; ++j) {
					for (int i = j; i < dim; ++i) {
						A[i * dim + j] = A[i * dim + j] - A[i * dim + k] * A[j * dim + k];
					}
				}
			}

		}

		template<typename T> void cholesky_factorization( int dim, T * A, int sta ) {

//			const double tol = impl::math::TOL;
			const double tol = 10e-16;

			for (int k = 0; k < dim; ++k) {

				if (__abs( A[k * sta + k] ) < tol || A[k * sta + k] < 0) {
					__error( "The Matrix in the Cholesky Factorization must be positive definite." );
				}

				A[k * sta + k] = sqrt( A[k * sta + k] );
				for (int i = k + 1; i < dim; ++i) {
					A[i * sta + k] = A[i * sta + k] / A[k * sta + k];
				}
				for (int j = k + 1; j < dim; ++j) {
					for (int i = j; i < dim; ++i) {
						A[i * sta + j] = A[i * sta + j] - A[i * sta + k] * A[j * sta + k];
					}
				}
			}

		}

		template<typename T> void cholesky_incomplete_factorization( int dim, T * A ) {

			for (int k = 0; k < dim; ++k) {

				if (__abs( A[k * dim + k] ) < TOL || A[k * dim + k] < 0) {
					__error( "The Matrix in the Cholesky Factorization must be positive definite." );
				}

				A[k * dim + k] = sqrt( A[k * dim + k] );
				for (int i = k + 1; i < dim; ++i) {

					if (A[i * dim + k] != 0) {
						A[i * dim + k] = A[i * dim + k] / A[k * dim + k];
					}
				}
				for (int j = k + 1; j < dim; ++j) {
					for (int i = j; i < dim; ++i) {

						if (A[i * dim + j] != 0) {
							A[i * dim + j] = A[i * dim + j] - A[i * dim + k] * A[j * dim + k];
						}

					}
				}
			}

		}

	}
}

#endif /* IMPL_CHOLESKY_FACTORISZATION_HPP */
