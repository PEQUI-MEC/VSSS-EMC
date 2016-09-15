/*
 * File:   rref_gaussian.hpp
 * Author: gustavo
 *
 * Created on August 28, 2012, 2:08 AM
 */

#ifndef IMPL_RREF_GAUSSIAN_HPP
#define	IMPL_RREF_GAUSSIAN_HPP

#include "math_macros.h"
#include "../util/mem.h"

#include "mbop.hpp"

namespace impl {
	namespace math {

		/**
		 * \brief Reduced Row Echelon Form.
		 *
		 * This function calculates the reduced row echelon form of an input matrix.
		 * No pivoting is used.
		 *
		 * @note
		 * The input matrix R is replaced by the reduced row echelon form.
		 *
		 * @param m - number of rows.
		 * @param n - number of columns.
		 * @param R - input matrix (m by n) to be transformed to the row echelon form.
		 * @param Ind - [1 x n] vector describing the dependences of the variables. If Ind[k] == 1, the variable k is independent and it is dependent otherwise.
		 * @param N - [n x n-rank] null space matrix.
		 * @param rank - rank of the matrix R.
		 *
		 * @returns R in the reduced row echelon form.
		 */
		template<typename T> void rref_gaussian( int m, int n, T * R, char * Ind, T ** N, int * rank ) {

			int columns;
			int rows;
			int w, h, pw, ph, pi, i, c;
			T P, k;
			T tmp;

			*rank = 0;
			columns = n;
			rows = m;

			for( i = 0; i < n; i++ )
				Ind[i] = 0;

			// For each pivot, do:
			for( ph = pw = 0; pw < columns && ph < rows; pw++, ph++ ) {
				pi = ph * columns + pw;

				//Ind[pw] = 0;

				// select the pivot element
				P = R[pi];

				// = Row Exchange if needed ====================================
				if( abs( P ) == 0.0 ) {
					// if pivot is zero, then we have to find a non-zero
					// element below it and perform the row exchange

					for( h = ph; h < rows; h++ ) {
						// finding a non-zero element
						tmp = R[h * columns + pw];
						if( abs( tmp ) != 0 ) {
							break;
						}
					}
					if( h == rows ) {
						// There is no non-zero element below pivot.
						// Then we have to find a new pivot rightmost
						ph--;
						// ph is decremented because the loop (for) increment ph and pw
						// at the same time. Thus, the next increment will get the
						// rightmost index to pw.
						continue;
					} else {
						// There is a non-zero element below pivot in row h.
						// Then we have to perform row exchange between h and ph

						i = ph * columns;
						c = h * columns;
						for( w = pw; w < columns; w++ ) {
							tmp = R[i + w];
							R[i + w] = R[c + w];
							R[c + w] = tmp;
						}

						// Select the new pivot element
						P = R[pi];
					}
				}

				(*rank)++;
				Ind[pw] = 1;

				// = Normalize the row of pivot ================================
				if( P != 1.0 ) {
					i = ph * columns;
					for( w = pw; w < columns; w++ ) {
						R[i + w] *= 1.0 / P;
					}
					P = 1;
				}

				// = Do the lower elimination ==================================
				for( h = ph + 1; h < rows; h++ ) {

					// Get the value below pivot element
					k = R[h * columns + pw];

					if( k == 0.0 ) {
						// If the below value is zero, then we have perform
						// the elimination of the next row
						continue;
					}

					// We just normalize the pivot row. Thus, the factor k does not be
					// divided by P, because P is 1.
					// k = k / P;

					i = h * columns;
					c = ph * columns;
					for( w = pw; w < columns; w++ ) {
						// Row elimination
						R[i + w] = R[i + w] - k * R[c + w];
					}
				}

				// = Do the upper elimination ==================================
				for( h = ph - 1; h >= 0; h-- ) {
					// Get the value above pivot element
					k = R[h * columns + pw];
					if( k == 0.0 ) {
						// If the below value is zero, then we have perform
						// the elimination of the above rowjust
						continue;
					}

					// We just normalize the pivot row. Thus, the factor k does not be
					// divided by P, because P is 1.
					// k = k / P;

					i = h * columns;
					for( w = pw; w < columns; w++ ) {
						// Row elimination
						R[i + w] = R[i + w] - k * R[ph * columns + w];
					}
				}

			}

			// = Null Space ====================================================
			if( N != 0 ) {
				// Need to compute the Null space of matrix R

				if( *rank == columns ) {
					// The rank is equal to number of columns (variables)
					// The Null Space is only the vector 0.
					*N = 0;
				} else {

					// Need to compute the Null space of matrix R
					int p, hr, wn, hn;
					int nrows = columns;
					int ncols = columns - (*rank);

					(*N) = mem_allocn(nrows * ncols, T);

					hr = 0; // row R index

					p = 0; // aux index to identity matrix

					for( hn = 0; hn < columns; hn++ ) {
						// for each variable do
						if( Ind[hn] == 1.0 ) {
							// i is a independent variable
							wn = 0; // column N index
							for( w = 0; w < columns; w++ ) {
								if( Ind[w] == 0.0 ) {
									(*N)[hn * ncols + wn] = (-1) * R[hr * columns + w];
									wn++;
								}
							}
							hr++;
						} else {
							// i is a dependent variable.
							(*N)[hn * ncols + p] = 1;
							p++;
						}
					}
				}
			} // </editor-fold>
		}
	}
}

#endif	/* IMPL_RREF_GAUSSIAN_HPP */

