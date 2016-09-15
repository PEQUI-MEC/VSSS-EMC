/* 
 * File:   blao_primitives.h
 * Author: gustavo
 *
 * Created on May 20, 2012, 1:40 AM
 */

#ifndef IMPL_MAT_PRIMITIVES_HPP
#define	IMPL_MAT_PRIMITIVES_HPP

#include <math.h>
#include <stdio.h>
namespace impl {
	namespace math {

//		/**
//		 * This function prints a matrix M at console.
//		 * @param m - number of rows of the matrix.
//		 * @param n - number of columns of the matrix.
//		 * @param M - pointer to the first matrix element.
//		 * @param stm - matrix stride. Number of elements from one row to the next row
//		 * in the memory. This value corresponds to the distance (sizeof(*M) * stm) in bytes.
//		 */
//		template<typename T> inline void __mprint(int m, int n, T * M, int stm) {
//
//			int r, c;
//
//			if (!M) {
//				printf("Null.\n\n");
//				return;
//			}
//
//			for (r = 0; r < m; r++) {
//				for (c = 0; c < n; c++) {
//					//printf("%4.4f\t", (double) ( abs(*M) < __eps) ? 0.0 : *M );
//					printf("%4.4f ", (double) *M);
//					M++;
//				}
//				printf("\n");
//				M += (stm - n);
//			}
//			printf("\n\n");
//		}
//
//		/**
//		 * This function copies a matrix to another.
//		 * @param m - number of lines of origin matrix.
//		 * @param n - number of columns of origin matrix.
//		 * @param A - pointer to first element of origin matrix.
//		 * @param sta - stride of origin matrix.
//		 * @param C - pointer to first element of destination matrix.
//		 * @param stc - stride of destination matrix.
//		 */
//		template<typename T> inline void __mcopy(int m, int n, T * A, int sta, T * C, int stc) {
//
//			int r, c;
//
//			for (r = 0; r < m; r++) {
//				for (c = 0; c < n; c++) {
//					*C = *A;
//					A++;
//					C++;
//				}
//				A += (sta - n);
//				C += (stc - n);
//			}
//		}
//
//		/**
//		 * Conversion between types
//		 * @param m
//		 * @param n
//		 * @param A
//		 * @param sta
//		 * @param C
//		 * @param stc
//		 */
//		template<typename T1, typename T2> inline void __mcopy_different_types(int m, int n, T1 * Orig, int sto, T2 * Dest, int std) {
//
//			int r, c;
//
//			for (r = 0; r < m; r++) {
//				for (c = 0; c < n; c++) {
//					*Dest = T2(*Orig);
//					Orig++;
//					Dest++;
//				}
//				Orig += (sto - n);
//				Dest += (std - n);
//			}
//		}
//
//		template<typename T> inline void __mset(int m, int n, T * A, int sta, T val) {
//
//			int r, c;
//
//			for (r = 0; r < m; r++) {
//				for (c = 0; c < n; c++) {
//					*A = val;
//					A++;
//				}
//				A += (sta - n);
//			}
//		}
//
//		/**
//		 * This function fills the diagonal of a matrix A with val.
//		 * @param m - dimension of A.
//		 * @param A - pointer to the first matrix element.
//		 * @param sta - stride of A.
//		 */
//		template<typename T> inline void __msetdiag(int m, T * A, int sta, T val) {
//			int r, c;
//			T * Ai;
//
//			Ai = A;
//			for (r = 0; r < m; r++) {
//				for (c = 0; c < m; c++) {
//
//					if (r == c) *Ai = val;
//
//					Ai++;
//				}
//				Ai += (sta - m);
//			}
//
//		}
//
//		/**
//		 * This function fills the matrix A with the matrix identity.
//		 * @param m - dimension of A.
//		 * @param A - pointer to the first matrix element.
//		 * @param sta - stride of A.
//		 */
//		template<typename T> inline void __meye(int m, T * A, int sta) {
//			int r, c;
//			T * Ai;
//
//			Ai = A;
//			for (r = 0; r < m; r++) {
//				for (c = 0; c < m; c++) {
//
//					if (r == c) *Ai = 1;
//					else *Ai = 0;
//
//					Ai++;
//				}
//				Ai += (sta - m);
//			}
//
//		}













		/**
		 * This function multiplies two matrices. The result is stored in M.
		 * @param rows_a - number of rows of the matrix A.
		 * @param cols_a - number of columns of the matrix A.
		 * @param A - pointer to the first element of the matrix.
		 * @param sta - stride of A. Number of elements in memory from the first row to the next.
		 * @param rows_b - number of rows of the matrix B.
		 * @param cols_b - number of columns of the matrix B.
		 * @param B - pointer to the first element of the matrix.
		 * @param stb - stride of B. Number of elements in memory from the first row to the next.
		 * @param M - pointer to the first element of the product matrix.
		 * @param stm - stride of M. Number of elements in memory from the first row to the next.
		 */
		template<typename T> inline void __mmult(int rows_a, int cols_a, T * A, int sta, int rows_b, int cols_b, T * B, int stb, T * M, int stm) {

			int r, c, k;

			T * _B = B;
			T * Ai;
			T * Bi;

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
				M += (stm - cols_b);
				A += sta;
			}

		}

		template<typename T> inline void __mdiv(int rows_a, int cols_a, T * A, int sta, T * B, int stb, T * M, int stm) {

			int r, c;

			for (r = 0; r < rows_a; r++) {
				for (c = 0; c < cols_a; c++) {

					*M = *A / *B;

					A++;
					M++;
					B++;
				}
				M += (stm - cols_a);
				A += (sta - cols_a);
				B += (stb - cols_a);
			}

		}

		/**
		 * This function computes a element-by-element k power of a matrix.
		 * @param m - number of rows.
		 * @param n - number of columns.
		 * @param A - pointer to the first element of Matrix.
		 * @param sta - increment to the next row of Matrix A.
		 * @param P - pointer to the first element of the power Matrix.
		 * @param stp - increment to the next row of Matrix P.
		 * @param k - the power parameter.
		 */
		template<typename T> inline void __mpow(int m, int n, T * A, int sta, double k, double * P, int stp) {

			int r, c;

			for (r = 0; r < m; r++) {
				for (c = 0; c < n; c++) {
					*P = pow(double(*A), k);
					A++;
					P++;
				}
				A += (sta - n);
				P += (stp - n);
			}
		}

		template<typename T> inline void __mrows_exchange(int m, int n, T * A, int sta, int r1, int r2) {

			T tmp;
			int r, c;

			T * a1 = A + r1 * sta;
			T * a2 = A + r2 * sta;

			for (c = 0; c < n; c++) {

				tmp = *a1;
				*a1 = *a2;
				*a2 = tmp;
				a1++;
				a2++;
			}

		}

		template<typename T> inline void __mcolumns_exchange(int m, int n, T * A, int sta, int c1, int c2) {

			T tmp;
			int r, c;

			T * a1 = A + c1;
			T * a2 = A + c2;

			for (r = 0; r < m; r++) {

				tmp = *a1;
				*a1 = *a2;
				*a2 = tmp;

				a1 += sta;
				a2 += sta;
			}

		}

	}
}

#endif	/* IMPL_MAT_PRIMITIVES_HPP */

