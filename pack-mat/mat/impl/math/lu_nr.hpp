/* 
 * File:   ldu.hpp
 * Author: gustavo
 *
 * Created on August 27, 2012, 11:28 AM
 */

#ifndef IMPL_LU_HPP
#define	IMPL_LU_HPP

#include "../util/common_macros.h"
#include "math_macros.h"

#define TINY 1.0e-40
#define __tolerance 1.e-50

namespace impl {
	namespace math {

		/**
		 *
		 * @param m - rows
		 * @param n - columns
		 * @param M - input matrix
		 * @param vv -
		 * @param LU
		 * @param indx -
		 * @param pivot - pivoting mapping
		 * @return
		 */
		template<typename T> int lu_nr(int m, int n, T * LU, T * vv, int * indx, int * pivot) {

			int i, j, k;

			// Initialize the determinant of row permutation matrix
			int pdet = 1;
			// Initialize pivot like: pivot = [0, 1, 2, 3 ..., m-1]
			for (i = 0; i < m; i++) {
				pivot[i] = i;
			}

			// loop over the rows to get implicit scaling information
			for (i = 0; i < m; i++) {
				T big = 0.0;
				for (j = 0; j < n; j++) {
					T temp = abs(LU[i * n + j]);
					if (big < temp) big = temp;
				}
				// see if it is singular
				if (big == 0.0) big = 1.0;
				vv[i] = 1.0 / big;
			}

			// outermost kij loop
			for (k = 0; k < n; k++) {
				int imax = -1;

				// start search by row for largest pivot element
				T big = 0.0;
				for (i = k; i < m; i++) {
					T temp = vv[i] * LU[i * n + k];
					if (temp < 0) temp = -temp;
					if (temp > big) {
						big = temp;
						imax = i;
					}
				}

				// see if it is singular
				if (imax < 0) {
					// TODO flag as singular
					indx[k] = -1;
				} else {
					// check to see if rows need to be interchanged
					if (k != imax) {
						int imax_n = imax * n;
						int k_n = k * n;
						int end = k_n + n;
						// j=0:n-1
						for (; k_n < end; imax_n++, k_n++) {
							T temp = LU[imax_n];
							LU[imax_n] = LU[k_n];
							LU[k_n] = temp;
						}
						pdet = -pdet;
						vv[imax] = vv[k];

						int z = pivot[imax];
						pivot[imax] = pivot[k];
						pivot[k] = z;
					}

					indx[k] = imax;
					// for some applications it is better to have this set to tiny even though
					// it is singular.  see the book
					T element_kk = LU[k * n + k];
					if (element_kk == 0.0) {
						LU[k * n + k] = TINY;
						element_kk = TINY;
					}

					// the large majority of the processing time is spent in the code below
					for (i = k + 1; i < m; i++) {
						int i_n = i * n;

						// divide the pivot element
						T temp = LU[i_n + k] /= element_kk;

						int k_n = k * n + k + 1;
						int end = i_n + n;
						i_n += k + 1;
						// reduce remaining submatrix
						// j = k+1:n-1
						for (; i_n < end; k_n++, i_n++) {
							// dataLU[i*n +j] -= temp* dataLU[k* n +j];
							LU[i_n] -= temp * LU[k_n];
						}
					}
				}
			}
			return pdet;
		}

	}
}

#endif	/* IMPL_LU_HPP */

