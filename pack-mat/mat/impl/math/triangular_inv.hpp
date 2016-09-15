/*
 * triangular_inv.hpp
 *
 *  Created on: Oct 22, 2012
 *      Author: gustavo
 */

#ifndef IMPL_TRIANGULAR_INV_HPP
#define IMPL_TRIANGULAR_INV_HPP

namespace impl {
	namespace math {

		template<typename T> void upper_inv(int dim, T * A, int sta, T * Inv, int sti) {

			int ra, _ra;
			int _rx;

			int c;

			for (ra = (dim - 1), _ra = ra * sta, _rx = ra * sti; //
					ra >= 0; //
					--ra, _ra -= sta, _rx -= sti //
					) {

				T Pa = A[_ra + ra];

				for (c = ra; c < dim; ++c) {

					T sum = 0;
					int _irx;
					for (int ci = ra + 1, _irx = ci * sti; ci < dim; ++ci, _irx += sti) {
						sum += A[_ra + ci] * Inv[_irx + c];
					}

					if (ra == c) Inv[_rx + c] = (1 - sum) / Pa;
					else Inv[_rx + c] = -sum / Pa;

				}

			}

		}

		template<typename T> void lower_inv(int dim, T * A, int sta, T * Inv, int sti) {

			int ra, _ra;
			int _rx;

			int c;

			for (ra = 0, _ra = ra * sta, _rx = ra * sti; //
					ra < dim; //
					++ra, _ra += sta, _rx += sti //
					) {

				T Pa = A[_ra + ra];

				for (c = 0; c < ra + 1; ++c) {

					T sum = 0;
					int _irx;
					for (int ci = 0, _irx = ci * sti; ci < ra; ++ci, _irx += sti) {
						sum += A[_ra + ci] * Inv[_irx + c];
					}

					if (ra == c) Inv[_rx + c] = (1 - sum) / Pa;
					else Inv[_rx + c] = -sum / Pa;

				}

			}

		}

	}
}

#endif /* IMPL_TRIANGULAR_INV_HPP */
