/*
 * m_util.hpp
 *
 *  Created on: Apr 21, 2013
 *      Author: gustavo
 */

#ifndef M_UTIL_HPP_
#define M_UTIL_HPP_

#include "../array/arrayops.hpp"

namespace impl {
	namespace matops {

		/**
		 * This function prints a matrix M at console.
		 * @param m - number of rows of the matrix.
		 * @param n - number of columns of the matrix.
		 * @param M - pointer to the first matrix element.
		 * @param stm - matrix stride. Number of elements from one row to the next row
		 * in the memory. This value corresponds to the distance (sizeof(*M) * stm) in bytes.
		 */
		template<typename T> inline void print( int rows, int columns, T * M, int stm ) {

			int r, c;
			int inc = (stm - columns);

			if (!M) {
				printf( "Null.\n\n" );
				return;
			}

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					//printf("%4.4f\t", (double) ( abs(*M) < __eps) ? 0.0 : *M );
					printf( "%4.3f\t", (float) *M );
					M++;
				}
				printf( "\n" );
				M += inc;
			}
			printf( "\n\n" );
		}

		/**
		 * This function copies a matrix to another.
		 *
		 * @param rows
		 * @param columns
		 * @param orig
		 * @param stride_orig
		 * @param dest
		 * @param stride_dest
		 */
		template<typename T> inline void copy( //
				int rows, int columns, //
				const T * orig, int stride_orig, //
				T * dest, int stride_dest ) {

			if (stride_orig == columns && stride_dest == columns) {
				impl::arrayops::copy( rows * columns, dest, orig );
				return;
			}

			int inc_orig = (stride_orig - columns);
			int inc_dest = (stride_dest - columns);

			for (int r = 0; r < rows; r++) {
				for (int c = 0; c < columns; c++) {
					*dest = *orig;
					orig++;
					dest++;
				}
				orig += inc_orig;
				dest += inc_dest;
			}
		}
		template<typename Tdest, typename Torig> inline void copy( //
				int rows, int columns, //
				const Torig * orig, int stride_orig, //
				Tdest * dest, int stride_dest ) {

			if (stride_orig == columns && stride_dest == columns) {
				impl::arrayops::copy( rows * columns, dest, orig );
				return;
			}

			int inc_orig = (stride_orig - columns);
			int inc_dest = (stride_dest - columns);

			for (int r = 0; r < rows; r++) {
				for (int c = 0; c < columns; c++) {
					*dest = *orig;
					orig++;
					dest++;
				}
				orig += inc_orig;
				dest += inc_dest;
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
		template<typename T> inline void rows_exchange( int rows, int columns, T * A, int sta, int r1, int r2 ) {

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
		template<typename T> inline void columns_exchange( int rows, int columns, T * A, int sta, int c1, int c2 ) {

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
		 *
		 * @param rows
		 * @param columns
		 * @param A
		 * @param sta
		 * @param xr
		 * @param xc
		 * @param M
		 * @param stm
		 */
		template<typename T> inline void repmat( int rows, int columns, T * A, int sta, int xr, int xc, T * M, int stm ) {

			int r, c;

			for (r = 0; r < xr; ++r) {
				for (c = 0; c < xc; ++c) {
					copy( rows, columns, A, sta, &M[r * rows * stm + c * columns], stm );
				}
			}

		}

	}
}

#endif /* M_UTIL_HPP_ */
