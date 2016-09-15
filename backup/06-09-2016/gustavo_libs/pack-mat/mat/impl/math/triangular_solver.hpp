/*
 * triangular_solver.hpp
 *
 *  Created on: Oct 22, 2012
 *      Author: gustavo
 */

#ifndef IMPL_TRIANGULAR_SOLVER_HPP
#define IMPL_TRIANGULAR_SOLVER_HPP

namespace impl {
	namespace math {

		template<typename T> void back_substitution(int dim, T * A, int sta, int nb, T * B, int stb, T * X, int stx) {

			int ra, _ra;
			int _rb;
			int _rx;

			int c;

			for (ra = (dim - 1), _ra = ra * sta, _rb = ra * stb, _rx = ra * stx; //
			        ra >= 0; //
			        --ra, _ra -= sta, _rb -= stb, _rx -= stx //
			        ) {

				T Pa = A[_ra + ra];

				for (c = 0; c < nb; ++c) {

					T sum = 0;
//					int _irx;
					for (int ci = ra + 1, _irx = ci * stx; ci < dim; ++ci, _irx += stx) {
						sum += A[_ra + ci] * X[_irx + c];
					}

					X[_rx + c] = (B[_rb + c] - sum) / Pa;

				}

			}

		}

		template<typename T> void forward_substitution(int dim, T * A, int sta, int nb, T * B, int stb, T * X, int stx) {

			int ra, ca, _ra, ia;
			int cb, _rb;
			int rx, cx, _rx;

			int c;

			for (ra = 0, _ra = ra * sta, _rb = ra * stb, _rx = ra * stx; //
			        ra < dim; //
			        ++ra, _ra += sta, _rb += stb, _rx += stx //
			        ) {

				T Pa = A[_ra + ra];

				for (c = 0; c < nb; ++c) {

					T sum = 0;
					int _irx;
					for (int ci = 0, _irx = ci * stx; ci < ra; ++ci, _irx += stx) {
						sum += A[_ra + ci] * X[_irx + c];
					}

					X[_rx + c] = (B[_rb + c] - sum) / Pa;

				}

			}

		}

	}
}

#endif /* IMPL_TRIANGULAR_SOLVER_HPP */
