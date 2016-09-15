/* 
 * File:   ldu.hpp
 * Author: gustavo
 *
 * Created on August 27, 2012, 11:28 AM
 */

#ifndef IMPL_LU_HPP
#define	IMPL_LU_HPP

#include "math_macros.h"
#define __tolerance 1.0e-50

namespace impl {
	namespace math {

		/**
		 * \brief LU matrix factorization. (Single Precision)
		 *
		 * This function computes de LU factorization of a matrix. A matrix A is
		 * expressed as the product of two triangular matrices, a lower triangular
		 * matrix (L) and an upper triangular matrix (U).
		 *
		 * In a common factorization, the matrix L has an unit diagonal, diagonal(L) = 1.
		 *
		 * Due the numerical instability of factorization a matrix of row permutations
		 * (P) is used in the factorization to numeric stability, such that P'A = LU or
		 * A = PLU.
		 *
		 * Sometimes the special factorization P'A = LDU, may be necessary. In this case
		 * D is a diagonal matrix and both L and U have an unit diagonal.
		 *
		 * @note
		 * When P needs to be calculated, this function degenerates the input matrix A.
		 * The matrices L, D, U and P must be pre-allocated.
		 *
		 * @param m - number of rows.
		 * @param n - number of columns.
		 * @param A - m by n input matrix.
		 * @param L - m by k lower triangular matrix.
		 * @param D - k by k diagonal matrix. D is computed if D != NULL.
		 * @param U - k by n upper triangular matrix.
		 * @param P - m by m row permutation matrix. P is computed if P != NULL.
		 * @param n_changes - number of row exchanges.
		 * @return 1 if A is a non-singular matrix and 0 if A is a singular matrix.
		 */
		template<typename T> int ldu(int m, int n, T * A, T * L, T * D, T * U, T * P, int * n_changes) {

			int r, c, p, _p, _r;
			int k;

			T Pivot;
			T tmp;
			T max;
			int r_of_max;

			T sum;

			int rows;
			int columns;
			int dim;

			char flag_p = 1;
			char flag_d = 1;

			(*n_changes) = 0;

			rows = m;
			columns = n;

			dim = __min(rows, columns);

			if (P == 0)
				flag_p = 0;
			if (D == 0)
				flag_d = 0;

			if (flag_p) {
				// Initialize P as an identity matrix.
				for (p = 0; p < rows * rows; p += rows + 1) {
					P[p] = 1;
				}
			}

			// Initialize L as an identity matrix.
			for (p = 0; p < rows * dim; p += (dim + 1)) {
				L[p] = 1;
			}

			for (p = 0; p < dim; p++) {
				// for each pivot do:
				_p = p * columns;
				Pivot = A[_p + p];

				// If we need do the matrix permutations
				if (flag_p) {
					// We need do the row exchange if the pivot is not the max values of column.
					// To this, we have to find the row with the max absolute value below the pivot.
					max = abs(Pivot);
					r_of_max = p;
					for (r = p + 1; r < rows; r++) {
						tmp = A[r * columns + p];

						max = abs(max);
						tmp = abs(tmp);

						if (max < tmp) {
							max = tmp;
							r_of_max = r;
						}
					}
					//if (r_of_max == -1 && Pivot == 0.0 ) {
					if (max == Pivot) {
						// The Pivot is bigger and we don't need exchange the rows.

						// If Pivot is less than 0, then the matrix is singular.
						if (Pivot < abs) {
							// we didn't find a absolute value different of zero.
							*L = 0;
							*U = 0;
							return 0;
						}

					} else {
						// We found a absolute value different of zero. Then, make row
						// exchange between p and r_of_max.

						(*n_changes)++;

						_p = p * columns;
						_r = r_of_max * columns;
						for (c = 0; c < columns; c++) {
							tmp = A[_p + c];
							A[_p + c] = A[_r + c];
							A[_r + c] = tmp;
						}

						// Do the same row exchange in the Permutations ImpMatrix
						//m_columns_exchange(rows, rows, P, rows, p, r_of_max);
						//__mcolumns_exchange(rows, rows, P, rows, p, r_of_max);
						_p = p * rows;
						_r = r_of_max * rows;
						for (c = 0; c < rows; c++) {
							tmp = P[_p + c];
							P[_p + c] = P[_r + c];
							P[_r + c] = tmp;
						}

						// Select the new Pivot value
						Pivot = A[_p + p];
					}
				}

				// In this point of the algorithm, we have a good pivot.
				// Then, let's to calculate the L and U values.

				for (r = 0; r < rows; r++) {
					for (c = 0; c < columns; c++) {

						if (r <= c) {
							sum = 0.0;
							for (k = 0; k < r; k++) {
								sum += L[r * dim + k] * U[k * columns + c];
							}
							U[r * columns + c] = A[r * columns + c] - sum;
						} else {
							sum = 0.0;
							for (k = 0; k < c; k++) {
								sum += L[r * dim + k] * U[k * columns + c];
							}
							if (U[c * columns + c] == 0) {
								/* divide by 0 */
								continue;
							}
							if (abs(U[c * columns + c]) < __tolerance) {
								//printf("Indeterminado!\n");
								L[r * dim + c] = 0;
							} else {
								L[r * dim + c] = (A[r * columns + c] - sum) / U[c * columns + c];
							}
						}
					}
				}
			}

			if (flag_d) {
				// Compute the D matrix
				for (p = 0; p < dim; p++) {

					_p = p * dim;
					_r = p * columns;

					D[_p + p] = U[_r + p];

					for (r = p; r < columns; r++) {
						if (abs(D[_p + p]) < __tolerance)
							U[_r + r] = 0.0;
						else
							U[_r + r] /= D[_p + p];
					}
				}
			}

		}

	}
}

#endif	/* IMPL_LU_HPP */

