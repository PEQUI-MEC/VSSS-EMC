/*
 * inverse.hpp
 *
 *  Created on: Oct 17, 2012
 *      Author: gustavo
 */

#ifndef IMPL_INVERSE_HPP
#define IMPL_INVERSE_HPP

#include "config.hpp"
#include "math_macros.h"
#include "mbop.hpp"

#include "../util/msg.h"

namespace impl {
	namespace math {

		template<typename T> T determinant_3x3(T * M) {

//				a0 a1 a2
//			A = a3 a4 a5
//				a6 a7 a8

			T det;
			det = 0;
			det += M[0] * (M[4] * M[8] - M[5] * M[7]);
			det -= M[1] * (M[3] * M[8] - M[5] * M[6]);
			det += M[2] * (M[3] * M[7] - M[4] * M[6]);

			return det;
		}

		template<typename T> void inverse_3x3(T * M, T * Inv) {

//				a0 a1 a2
//			A = a3 a4 a5
//				a6 a7 a8
			T det, inv_det;

			det = determinant_3x3(M);

			if ( __abs(det) < TOL ) {
				__error("Matrix is singular!\n");
			}

			inv_det = 1 / det;

			Inv[0] = (M[4] * M[8] - M[5] * M[7]) * inv_det;
			Inv[1] = (M[5] * M[6] - M[3] * M[8]) * inv_det;
			Inv[2] = (M[3] * M[7] - M[4] * M[6]) * inv_det;
			Inv[3] = (M[2] * M[7] - M[1] * M[8]) * inv_det;
			Inv[4] = (M[0] * M[8] - M[2] * M[6]) * inv_det;
			Inv[5] = (M[1] * M[6] - M[0] * M[7]) * inv_det;
			Inv[6] = (M[1] * M[5] - M[2] * M[4]) * inv_det;
			Inv[7] = (M[2] * M[3] - M[0] * M[5]) * inv_det;
			Inv[8] = (M[0] * M[4] - M[1] * M[3]) * inv_det;

		}

		/**
		 * Retorna a matriz inversa da matriz quadrada <code>A</code>. Utiliza o
		 * método de Gauss-Jordan (matriz aumentada).
		 *
		 *
		 * @param A
		 * @param inv
		 *
		 *
		 */

		template<typename T> void inverse_GaussJordan(int dim, T * A, int sta, T * Inv, int sti) {

			int r, c, p;
			int _ra, _ri, _rpa, _rpi;
			int ida, idi;
			T P, k;

			// = Create a identity matrix ======================================
			m_set_identity(dim, Inv, sti);

			// = Upper Triangle ================================================
			for (p = 0; p < dim; ++p) {

				_rpa = p * sta;
				_rpi = p * sti;

				P = A[_rpa + p];

				for (r = p + 1; r < dim; ++r) {

					_ra = r * sta;
					_ri = r * sti;

					if (__abs(A[_ra + p]) < TOL) continue;

					k = -P / A[_ra + p];

					for (c = 0; c < dim; ++c) {

						ida = _ra + c;
						idi = _ri + c;
						A[ida] = k * A[ida] + A[_rpa + c];
						Inv[idi] = k * Inv[idi] + Inv[_rpi + c];

					}
				}
			}

			//= Lower Triangle =================================================
			for (p = dim - 1; p >= 0; --p) {

				_rpa = p * sta;
				_rpi = p * sti;

				P = A[_rpa + p];

				for (r = p - 1; r >= 0; --r) {

					_ra = r * sta;
					_ri = r * sti;

					if (A[_ra + p] == 0) continue;
					k = (-1) * P / A[_ra + p];

					for (c = 0; c < dim; c++) {

						ida = _ra + c;
						idi = _ri + c;
						A[ida] = k * A[ida] + A[_rpa + c];
						Inv[idi] = k * Inv[idi] + Inv[_rpi + c];
					}
				}
			}

			for (p = 0; p < dim; p++) {
				P = A[p * sta + p];
				for (c = 0; c < dim; c++) {
					A[p * sta + c] /= P;
					Inv[p * sti + c] /= P;
				}
			}
		}

	}
}

#endif /* IMPL_INVERSE_HPP */
