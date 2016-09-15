/*
 * File:   tf.h
 * Author: gustavo
 *
 * Created on June 13, 2012, 6:42 AM
 */

#ifndef IMPL_TRIANGULAR_HPP
#define	IMPL_TRIANGULAR_HPP

#include "../util/common_macros.h"

#include "config.hpp"
#include "math_macros.h"
#include "mbop.hpp"

namespace impl {
	namespace math {

		template<typename T> void upper_triangle(int m, int n, T * A, int sta) {
			int c, r, _r, i, p;
			T P, k;
			int ip;
			int dim;

			dim = (m < n) ? m : n;

			for (p = 0, ip = p * sta; p < dim - 1; ++p, ip += sta) {

				ip = p * sta;

				P = A[ip + p];

				if (__abs( P ) < TOL) continue;

				for (r = p + 1, _r = r * sta; r < m; r++, _r += sta) {

					if (__abs(A[_r + p]) < TOL) continue;

					/* Calculate the scale factor */
					k = (-1) * A[_r + p] / P;

					for (c = p; c < n; c++) {
						i = _r + c;
						A[i] = A[i] + k * A[ip + c];
					}
				}
			}
		}

		template<typename T> void upper_triangle(int m, int n, T * A, int sta, int * e) {
			int c, r, _r, i, p;
			T P, k;
			int ip;
			int dim;

			dim = (m < n) ? m : n;

			for (p = 0; p < dim; ++p)
				e[p] = p;

			for (p = 0, ip = p * sta; p < dim - 1; ++p, ip += sta) {

				ip = p * sta;

				// Select the MAX PIVOT
				// Find the max absolute value and select it as the new pivot element
				int p_max = p;
				P = __abs(A[ip + p]);

				for (r = p; r < m; ++r) {

					T abs_val = __abs( A[r*sta + p]);

					if (abs_val > P) {
						P = abs_val;
						p_max = r;
					}
				}
				//P = impl::math::v_max( m - p, &A[ip + p], sta, &p_max );

				// If there is not non-zero element, then continue.
				if (P < TOL) continue;

				// = Row Exchange if needed ====================================
				if (p != p_max) {
					math::m_rows_exchange(m, n, A, sta, p, p_max);
					// Store the row exchange
					__swap( e[p], e[p_max], int);
				}

				P = A[ip + p];
				for (r = p + 1, _r = r * sta; r < m; r++, _r += sta) {

					if (__abs(A[_r + p]) < TOL) continue;

					/* Calculate the scale factor */
					k = (-1) * A[_r + p] / P;

					for (c = p; c < n; c++) {
						i = _r + c;
						A[i] = A[i] + k * A[ip + c];
					}

				}
			}
		}

		template<typename T> void lower_triangle(int m, int n, T * A, int sta, int * e) {
			int c, r, _r, i, p;
			T P, k;
			int ip;
			int dim;

			dim = (m < n) ? m : n;

			for (p = 0; p < dim; ++p)
				e[p] = p;

			int min_p;
			if (n > m) min_p = n - m;
			else min_p = 0;

			for (p = n - 1; p >= min_p; --p, ip -= sta) {

				int cr = m - (n - p);
				ip = cr * sta;

				// Select the MAX PIVOT
				// Find the max absolute value and select it as the new pivot element
				int r_max = 0;
				P = __abs(A[p]);

				for (r = 0; r <= cr; ++r) {

					T abs_val = __abs( A[p + r*sta]);

					if (abs_val > P) {
						P = abs_val;
						r_max = r;
					}
				}

				// If there is not non-zero element, then continue.
				if (P < TOL) continue;

				// = Row Exchange if needed ====================================
				if (cr != r_max) {
					math::m_rows_exchange(m, n, A, sta, cr, r_max);
					// Store the row exchange
					__swap( e[cr], e[r_max], int);
				}

				m_print(m, n, A, sta);

				P = A[ip + p];
				for (r = cr - 1, _r = r * sta; r >= 0; --r, _r -= sta) {

					if (__abs(A[_r + p]) < TOL) continue;

					/* Calculate the scale factor */
					k = (-1) * A[_r + p] / P;

					for (c = p; c >= 0; --c) {
						i = _r + c;
						A[i] = A[i] + k * A[ip + c];
					}

					m_print(m, n, A, sta);

				}
			}
		}

	}
}
#endif	/* IMPL_TRIANGULAR_HPP */

