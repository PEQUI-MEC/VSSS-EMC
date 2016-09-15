/*
 * arrayop.hpp
 *
 *  Created on: Oct 29, 2012
 *      Author: gustavo
 */

#ifndef IMPL_ABOP_HPP
#define IMPL_ABOP_HPP

#include <math.h>

namespace impl {
	namespace math {

		/**
		 * C[0:m-1][0:n-1]  = A[0:m-1][0:n-1] .* B[0:m-1][0:n-1]
		 */
		template<typename Ta, typename Tb, typename Tc> inline void a_mult( int rows, int cols, Ta * A, int sta, const Tb * B, int stb, Tc * C, int stc ) {

			int r, c;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*C = *A * *B;

					A++;
					B++;
					C++;
				}
				A += (sta - cols);
				B += (stb - cols);
				C += (stc - cols);
			}

		}

		/**
		 * C[0:m-1][0:n-1]  = A[0:m-1][0:n-1] .^ p
		 */
		template<typename Ta, typename Tc> inline void a_power( int rows, int cols, Ta * A, int sta, double p, Tc * C, int stc ) {

			int r, c;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*C = pow( *A, p );

					A++;
					C++;
				}
				A += (sta - cols);
				C += (stc - cols);
			}

		}

	}
}

#endif /* IMPL_ABOP_HPP */
