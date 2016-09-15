/*
 * File:   bop.hpp
 * Author: gustavo
 *
 * Created on August 10, 2012, 1:47 AM
 */

#ifndef IMPL_MBOP_HPP
#define	IMPL_MBOP_HPP

#include <stdio.h>
#include <stdlib.h> // for RAND_MAX
#include "math_macros.h"

namespace impl {
	namespace math {

		/**
		 * This function prints a matrix M at console.
		 * @param m - number of rows of the matrix.
		 * @param n - number of columns of the matrix.
		 * @param M - pointer to the first matrix element.
		 * @param stm - matrix stride. Number of elements from one row to the next row
		 * in the memory. This value corresponds to the distance (sizeof(*M) * stm) in bytes.
		 */
		template<typename T> inline void m_print(int rows, int columns, T * M, int stm) {

			int r, c;

			if (!M) {
				printf("Null.\n\n");
				return;
			}

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					//printf("%4.4f\t", (double) ( abs(*M) < __eps) ? 0.0 : *M );
					printf("%4.3f\t", (float) *M);
					M++;
				}
				printf("\n");
				M += (stm - columns);
			}
			printf("\n\n");
		}

		/**
		 * This function copies a matrix to another.
		 *
		 * @param m
		 * @param n
		 * @param Orig
		 * @param sto
		 * @param Dest
		 * @param std
		 */
		template<typename T> inline void m_copy(int rows, int columns, const T * Orig, int sto, T * Dest, int std) {

			int r, c;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					*Dest = *Orig;
					Orig++;
					Dest++;
				}
				Orig += (sto - columns);
				Dest += (std - columns);
			}
		}

		/**
		 * Conversion between types
		 * @param m
		 * @param n
		 * @param A
		 * @param sta
		 * @param C
		 * @param stc
		 */
		template<typename T1, typename T2> inline void m_copy(int rows, int columns, const T1 * Orig, int sto, T2 * Dest, int std) {

			int r, c;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					*Dest = T2(*Orig);
					Orig++;
					Dest++;
				}
				Orig += (sto - columns);
				Dest += (std - columns);
			}
		}

		/**
		 *
		 * @param m
		 * @param n
		 * @param A
		 * @param sta
		 * @param r1
		 * @param r2
		 */
		template<typename T> inline void m_rows_exchange(int rows, int columns, T * A, int sta, int r1, int r2) {

			T tmp;
			int c;

			T * a1 = A + r1 * sta;
			T * a2 = A + r2 * sta;

			for (c = 0; c < columns; c++) {

				tmp = *a1;
				*a1 = *a2;
				*a2 = tmp;
				a1++;
				a2++;
			}

		}

		/**
		 *
		 * @param m
		 * @param n
		 * @param A
		 * @param sta
		 * @param c1
		 * @param c2
		 */
		template<typename T> inline void m_columns_exchange(int rows, int columns, T * A, int sta, int c1, int c2) {

			T tmp;
			int r;

			T * a1 = A + c1;
			T * a2 = A + c2;

			for (r = 0; r < rows; r++) {

				tmp = *a1;
				*a1 = *a2;
				*a2 = tmp;

				a1 += sta;
				a2 += sta;
			}

		}

		/**
		 * This function fill the matrix A with the value of <code>val</code>.
		 * @param m
		 * @param n
		 * @param A
		 * @param sta
		 * @param val
		 */
		template<typename T> inline void m_set(int rows, int columns, T * A, int sta, double val) {

			int r, c;
			int inca = (sta - columns);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					*A = (T) val;
					A++;
				}
				A += inca;
			}
		}

		template<typename T> inline void m_set_border(int rows, int columns, T * A, int sta, int border_width, double val) {

			math::m_set(border_width, columns, A, sta, val);  // top
			math::m_set((rows - 2 * border_width), border_width, &A[border_width * sta], sta, val);  // left
			math::m_set((rows - 2 * border_width), border_width, &A[border_width * sta + (columns - border_width)], sta, val);  // right
			math::m_set(border_width, columns, &A[(rows - border_width) * sta], sta, val);  // down
		}

		/**
		 * This function fill the matrix A with the value of <code>val</code>.
		 * @param m
		 * @param n
		 * @param A
		 * @param sta
		 * @param val
		 */
		template<typename T> inline void m_set_rand(int rows, int columns, T * A, int sta) {

			int r, c;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					*A = (T) rand() / (T) RAND_MAX;
					A++;
				}
				A += (sta - columns);
			}
		}

		/**
		 * This function fills the diagonal of a matrix A with val.
		 * @param m - dimension of A.
		 * @param A - pointer to the first matrix element.
		 * @param sta - stride of A.
		 */
		template<typename T> inline void m_set_diag(int dim, T * A, int sta, T val) {

			int d;

			for (d = 0; d < dim; d++) {
				*A = val;
				A += (sta + 1);
			}
		}

		template<typename T> inline void m_set_identity(int dim, T * A, int sta) {
			m_set(dim, dim, A, sta, (T) 0);
			m_set_diag(dim, A, sta, (T) 1);
		}

		template<typename Tin, typename Tout> inline void m_sum(int rows, int columns, const Tin * A, int sta, Tout * S, int sts) {

			int r, c, _r;

			m_set(1, columns, S, sts, (Tout) 0);

			for (r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (c = 0; c < columns; ++c) {
					S[c] += A[_r + c];
				}
			}

		}

		template<typename Tin, typename Tout> inline void m_sum_row(int rows, int columns, const Tin * A, int sta, Tout * S, int sts) {

			int r, c, _r;

			m_set(rows, 1, S, sts, (Tout) 0);

			for (r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (c = 0; c < columns; ++c) {
					S[r] += A[_r + c];
				}
			}

		}

		template<typename T> inline T m_max(int rows, int columns, const T * A, int sta, int * r_max = 0, int * c_max = 0) {

			T max_val;

			int r, c;
			int _r, _c;

			max_val = *A;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					if (*A > max_val) {
						max_val = *A;
						_r = r;
						_c = c;
					}

					A++;
				}
				A += (sta - columns);
			}

			if (r_max) *r_max = _r;
			if (c_max) *c_max = _c;

			return max_val;

		}

		template<typename T> inline T m_min(int rows, int columns, const T * A, int sta, int * r_min = 0, int * c_min = 0) {

			T min_val;

			int r, c;
			int _r, _c;

			min_val = *A;
			_r = 0;
			_c = 0;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					if (*A < min_val) {
						min_val = *A;
						_r = r;
						_c = c;
					}

					A++;
				}
				A += (sta - columns);
			}

			if (r_min) *r_min = _r;
			if (c_min) *c_min = _c;

			return min_val;

		}

		template<typename T> inline void m_max_min(int rows, int columns, T * A, int sta, T * max_val, T * min_val, int * r_max = 0, int * c_max = 0, int * r_min = 0, int * c_min =
				0) {

			int r, c;
			int Mr, Mc;
			int mr, mc;

			*max_val = *A;
			*min_val = *A;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					if (*A > *max_val) {
						*max_val = *A;
						Mr = r;
						Mc = c;
					}

					if (*A < *min_val) {
						*min_val = *A;
						mr = r;
						mc = c;
					}

					A++;
				}
				A += (sta - columns);
			}

			if (r_max) *r_max = Mr;
			if (c_max) *c_max = Mc;

			if (r_min) *r_min = mr;
			if (c_min) *c_min = mc;

		}

		template<typename T> inline void m_normalize(int rows, int columns, T * A, int sta, T min, T max) {

			int r, c;
			T MAX;
			T MIN;
			double k;

			m_max_min(rows, columns, A, sta, &MAX, &MIN);

			k = (double) (max - min) / (double) (MAX - MIN);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					*A = (T) (*A - MIN) * k + min;
					A++;
				}
				A += (sta - columns);
			}

		}

		template<typename To, typename Td> inline void m_copy_normalized(int rows, int columns, To * Orig, int sto, Td min, Td max, Td * Dest, int std) {

			int r, c;
			To MAX;
			To MIN;

			double k;

			m_max_min(rows, columns, Orig, sto, &MAX, &MIN);

			k = (double) (max - min) / (double) (MAX - MIN);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					*Dest = (Td) ((*Orig - MIN) * k + min);
					Orig++;
					Dest++;
				}
				Orig += (sto - columns);
				Dest += (std - columns);
			}

		}

		template<typename T> inline void m_repmat(int rows, int columns, const T * A, int sta, int xr, int xc, T * M, int stm) {

			int r, c;

			for (r = 0; r < xr; ++r) {
				for (c = 0; c < xc; ++c) {
					m_copy(rows, columns, A, sta, &M[r * rows * stm + c * columns], stm);
				}
			}

		}

	}
}
#endif	/* IMPL_MBOP_HPP */

