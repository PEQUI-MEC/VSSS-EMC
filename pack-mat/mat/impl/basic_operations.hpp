/*
 * basic_operations.hpp
 *
 *  Created on: Mar 8, 2013
 *      Author: gustavo
 */

#ifndef BASIC_OPERATIONS_HPP_
#define BASIC_OPERATIONS_HPP_

#include <stdlib.h>

#include "../impl/array/arrayops.hpp"
#include "m_stat.hpp"

namespace impl {
	namespace matops {

		/**
		 * This function fill the matrix A with the value of <code>val</code>.
		 * @param m
		 * @param n
		 * @param A
		 * @param sta
		 * @param val
		 */
		template<typename T, typename Tval> inline void fill(int rows, int columns, T * A, int sta, Tval val) {
			int inc = (sta - columns);
			for (int r = 0; r < rows; ++r) {
				for (int c = 0; c < columns; ++c) {
					*A = (T) val;
					A++;
				}
				A += inc;
			}
		}
		template<typename T, typename Tval> inline void fill(int rows, int columns, T * A, Tval val) {
			size_t size = rows * columns;
			for (; size > -1; --size) {
				*(A++) = (T) val;
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
		template<typename T> inline void set_rand(int rows, int columns, T * A, int sta) {

			int inc = (sta - columns);
			for (int r = 0; r < rows; r++) {
				for (int c = 0; c < columns; c++) {
					*A = (T) rand() / (T) (RAND_MAX+1);
					A++;
				}
				A += inc;
			}
		}

		/**
		 * This function fills the diagonal of a matrix A with val.
		 * @param m - dimension of A.
		 * @param A - pointer to the first matrix element.
		 * @param sta - stride of A.
		 */
		template<typename T, typename Tval> inline void fill_diagonal(int dim, T * A, int sta, Tval val) {

			int d;
			int inc = sta + 1;

			for (d = 0; d < dim; d++) {
				*A = val;
				A += inc;
			}
		}

		template<typename T> inline void set_identity(int dim, T * A, int sta) {
			fill(dim, dim, A, sta, (T) 0);
			fill_diagonal(dim, A, sta, (T) 1);
		}

		template<typename T> inline void normalize(int rows, int columns, T * A, int sta, T min, T max) {

			int r, c;
			T MAX;
			T MIN;
			double k;

			int inc = (sta - columns);

			amax_min(rows, columns, A, sta, &MAX, &MIN);

			k = (double) (max - min) / (double) (MAX - MIN);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {

					*A = (T) (*A - MIN) * k + min;
					A++;
				}
				A += inc;
			}

		}

		template<typename To, typename Td> inline void copy(int rows, int columns, To * Orig, int sto, Td * Dest, int std) {

			int r, c;
			int inc_orig = (sto - columns);
			int inc_dest = (std - columns);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					*Dest = (Td) *Orig;
					Orig++;
					Dest++;
				}
				Orig += inc_orig;
				Dest += inc_dest;
			}

		}

		template<typename To, typename Td> inline void copy_normalized(int rows, int columns, To * Orig, int sto, Td min, Td max, Td * Dest, int std) {

			int r, c;
			To MAX;
			To MIN;

			int inc_orig = (sto - columns);
			int inc_dest = (std - columns);

			double k;

			amax_min(rows, columns, Orig, sto, &MAX, &MIN);

			k = (double) (max - min) / (double) (MAX - MIN);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < columns; c++) {
					*Dest = (Td) ((*Orig - MIN) * k + min);
					Orig++;
					Dest++;
				}
				Orig += inc_orig;
				Dest += inc_dest;
			}

		}

		/**
		 * M[0:m-1][0:n-1] = A[0:m-1][0:n-1] + B[0:m-1][0:n-1]
		 */
		template<typename T> inline void add(int rows, int cols, T * A, int sta, T * B, int stb, T * M, int stm) {

			int r, c;
			int incM = (stm - cols);
			int incA = (sta - cols);
			int incB = (stb - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*M = *A + *B;

					A++;
					M++;
					B++;
				}
				M += incM;
				A += incA;
				B += incB;
			}
		}

		/**
		 *
		 * @param rows_a
		 * @param cols_a
		 * @param A
		 * @param sta
		 * @param rows_b
		 * @param cols_b
		 * @param B
		 * @param stb
		 * @param M
		 * @param stm
		 */
		template<typename T> inline void mult(int rows_a, int cols_a, T * A, int sta, int rows_b, int cols_b, T * B, int stb, T * M, int stm) {

			int r, c, k;

			T * _B = B;
			T * Ai;
			T * Bi;

			int incm = (stm - cols_b);

			for (r = 0; r < rows_a; r++) {

				B = _B;

				for (c = 0; c < cols_b; c++) {

					Ai = A;
					Bi = B;
					*M = 0;

					for (k = 0; k < rows_b; k++) {
						*M += (*Ai) * (*Bi);

						Ai++;
						Bi += stb;
					}
					M++;
					B++;
				}
				M += incm;
				A += sta;
			}

		}

	}

}

#endif /* BASIC_OPERATIONS_HPP_ */
