/*
 * m_blaop.hpp
 *
 *  Created on: Apr 21, 2013
 *      Author: gustavo
 */

#ifndef M_BLAOP_HPP_
#define M_BLAOP_HPP_

#include "../impl/math/math_macros.h"
#include "../impl/util/msg.h"

#include "../impl/math/vblaop.hpp"

namespace impl {
	namespace matops {

		/**
		 * A[0:m-1][0:n-1]  += k
		 */
		template<typename T> inline void m_inc( int rows, int cols, T * A, int sta, T k ) {
			int r, c;
			int incA = (sta - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {
					*A += k;
					A++;
				}
				A += incA;
			}
		}
		/**
		 * M[0:m-1][0:n-1]  = A[0:m-1][0:n-1] + k
		 */
		template<typename T1, typename T2> inline void m_inc( int rows, int cols, T1 * A, int sta, double k, T2 * M, int stm ) {

			int r, c;
			int incA = (sta - cols);
			int incM = (stm - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {
					*M = *A + k;
					A++;
					M++;
				}

				A += incA;
				M += incM;
			}
		}

		/**
		 * A[0:m-1][0:n-1]  -= k
		 */
		template<typename T> inline void m_dec( int rows, int cols, T * A, int sta, T k ) {
			int r, c;
			int incA = (sta - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {
					*A -= k;
					A++;
				}
				A += incA;
			}
		}
		/**
		 * M[0:m-1][0:n-1]  = A[0:m-1][0:n-1] - k
		 */
		template<typename T> inline void m_dec( int rows, int cols, T * A, int sta, T k, T * M, int stm ) {

			int r, c;
			int incA = (sta - cols);
			int incM = (stm - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*M = *A - k;

					A++;
					M++;
				}

				A += incA;
				M += incM;
			}
		}

		/**
		 * M[0:m-1][0:n-1]  = k - A[0:m-1][0:n-1]
		 */
		template<typename T> inline void m_dec_neg( int rows, int cols, T * A, int sta, T k, T * M, int stm ) {
			int r, c;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*M = k - *A;

					A++;
					M++;
				}
				M += (stm - cols);
				A += (sta - cols);
			}
		}

		/**
		 * M[0:m-1][0:n-1]  = k * A[0:m-1][0:n-1]
		 *
		 * This function scales by k element-by-element of a matrix.
		 * @param m - number of rows.
		 * @param n - number of columns.
		 * @param A - pointer to the first element of Matrix.
		 * @param sta - increment to the next row of Matrix A.
		 * @param k - the scale parameter.
		 * @param M - pointer to the first element of the scaled Matrix.
		 * @param stm - increment to the next row of Matrix M.
		 */
		template<typename T, typename Tk> inline void m_scale( int rows, int cols, const T * A, int sta, Tk k, T * M, int stm ) {

			int r, c;

			int inca = sta - cols;
			int incm = stm - cols;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {
					*M = *A * k;
					A++;
					M++;
				}
				A += inca;
				M += incm;
			}

		}

		template<typename T, typename Tk> inline void m_scale( int rows, int cols, T * A, int sta, Tk k ) {

			int r, c;
			int inca = sta - cols;

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {
					*A = *A * k;
					A++;
				}
				A += inca;
			}
		}

		/**
		 * A[0:m-1][0:n-1]  += B[0:m-1][0:n-1]
		 */
		template<typename T> inline void m_add( int rows, int cols, T * A, int sta, const T * B, int stb ) {

			int r, c;
			int incA = (sta - cols);
			int incB = (stb - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*A += *B;

					A++;
					B++;
				}
				A += incA;
				B += incB;
			}

		}

		/**
		 * M[0:m-1][0:n-1] = A[0:m-1][0:n-1] + B[0:m-1][0:n-1]
		 */
		template<typename T1, typename T2, typename Tout> inline void m_add( int rows, int cols, const T1 * A, int sta, const T2 * B, int stb, Tout * M, int stm ) {

			int r, c;
			int incA = (sta - cols);
			int incB = (stb - cols);
			int incM = (stm - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*M = *A + *B;

					A++;
					M++;
					B++;
				}
				A += incA;
				B += incB;
				M += incM;
			}
		}

		/**
		 * A[0:m-1][0:n-1]  -= B[0:m-1][0:n-1]
		 */
		template<typename T> inline void m_sub( int rows, int cols, T * A, int sta, const T * B, int stb ) {

			int r, c;
			int incA = (sta - cols);
			int incB = (stb - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*A -= *B;

					A++;
					B++;
				}
				A += incA;
				B += incB;
			}

		}

		/**
		 *  M[0:m-1][0:n-1] = A[0:m-1][0:n-1] - B[0:m-1][0:n-1]
		 */
		template<typename T1, typename T2, typename Tout> inline void m_sub( int rows, int cols, const T1 * A, int sta, const T2 * B, int stb, Tout * M, int stm ) {

			int r, c;
			int incA = (sta - cols);
			int incB = (stb - cols);
			int incM = (stm - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*M = *A - *B;

					A++;
					M++;
					B++;
				}
				A += incA;
				B += incB;
				M += incM;
			}

		}

		/**
		 * A[0:m-1][0:n-1] = - B[0:m-1][0:n-1]
		 *
		 * @param m
		 * @param A
		 * @param inca
		 * @param B
		 * @param incb
		 */
		template<typename Tin, typename Tout> inline void m_move( int rows, int cols, const Tin * Orig, int st_orig, Tout * Dest, int st_dest ) {
			int r, c;

			int inco = (st_orig - cols);
			int incd = (st_dest - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*Dest = *Orig;

					Dest++;
					Orig++;
				}
				Dest += incd;
				Orig += inco;
			}
		}

		/**
		 * A[0:m-1][0:n-1] = - B[0:m-1][0:n-1]
		 *
		 * @param m
		 * @param A
		 * @param inca
		 * @param B
		 * @param incb
		 */
		template<typename Tin, typename Tout> inline void m_move_neg( int rows, int cols, const Tin * Orig, int st_orig, Tout * Dest, int st_dest ) {
			int r, c;

			int inco = (st_orig - cols);
			int incd = (st_dest - cols);

			for (r = 0; r < rows; r++) {
				for (c = 0; c < cols; c++) {

					*Dest = -*Orig;

					Dest++;
					Orig++;
				}
				Dest += incd;
				Orig += inco;
			}
		}

		/**
		 * This function multiplies two matrices. The result is stored in M.
		 *
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
		template<typename Tin1, typename Tin2, typename Tout> inline void m_mult( //
				int rows_a, int cols_a, Tin1 * A, int sta, //
				int rows_b, int cols_b, Tin2 * B, int stb, //
				Tout * M, int stm ) {

			int r, c, k;

			Tin2 * _B = B;
			Tin1 * Ai;
			Tin2 * Bi;

			int incm = (stm - cols_b);

			for (r = 0; r < rows_a; r++) {

				B = _B;

				for (c = 0; c < cols_b; c++) {

					Ai = A;
					Bi = B;
					*M = 0;

					for (k = 0; k < rows_b; k++) {
						*M += (double) (*Ai) * (double) (*Bi);

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

		/**
		 * \brief Calculates the column norm.
		 *
		 * This function computes the norm of each column vector of the input matrix.
		 *
		 * @param rows number of rows.
		 * @param columns number of columns.
		 * @param mA input matrix.
		 * @param sta mA stride.
		 * @param vlen output vector.
		 * @param incl increment of vlen.
		 */
		template<typename T> inline void m_column_length( int rows, int columns, T * mA, int sta, T * vlen, int incl ) {

			int c;

			for (c = 0; c < columns; ++c) {
				*vlen = impl::math::v_length( rows, &mA[c], sta );
				vlen += incl;
			}

		}

		template<typename T> inline void m_row_length( int rows, int columns, T * mA, int sta, T * vlen, int incl ) {

			int r, _r;

			for (r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				*vlen = impl::math::v_length( columns, &mA[_r], 1 );
				vlen += incl;
			}

		}

		/**
		 * Compute the p-norm of a matrix like a vector.
		 */
		template<typename T> inline T m_pnorm( int rows, int columns, T * A, int sta, int p ) {

			int r, c;
			T tmp;
			T norm = 0;

			for (r = 0; r < rows; r++) {

				T * a = &A[r * sta];

				for (c = 0; c < columns; c++) {

					tmp = __abs( *a );
					norm += pow( tmp, p );
					a++;

				}
			}
			return pow( norm, 1.0 / p );
		}

		template<typename T> inline T m_norm_inf( int rows, int columns, T * A, int sta ) {
			int r, c;

			T norm = *A;
			for (r = 0; r < rows; r++) {

				T * a = &A[r * sta];
				T sum = 0;

				for (c = 0; c < columns; c++) {
					sum += __abs( *a );
					a++;
				}

				if (sum > norm) norm = sum;

			}
			return norm;
		}

		template<typename T> inline T m_norm1( int rows, int columns, T * A, int sta ) {

			int r, c;

			T norm = *A;

			for (c = 0; c < columns; c++) {

				T * a = &A[c];
				T sum = 0;

				for (r = 0; r < rows; r++) {
					sum += __abs( *a );
					a += sta;
				}

				if (sum > norm) norm = sum;

			}

			return norm;
		}

		template<typename T> inline T m_norm_frobenius( int rows, int columns, T * A, int sta ) {

			int r, c, _r;

			T norm;

			norm = 0;
			for (r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				T * a = &A[_r];
				for (c = 0; c < columns; c++) {
					norm += (*a) * (*a);
					a++;
				}
			}

			return sqrt( norm );
		}

		/**
		 * TODO Needs implements matrix norm-2. Is the max eigenvalue of the Matrix.
		 */
		template<typename T> inline T m_norm2( int rows, int columns, T * A, int sta ) {
			__error( "Not implemented!\n" );
			return 0;
		}

		template<typename T1, typename T2> inline void m_transpose( int rows, int columns, T1 * A, int sta, T2 * At, int stt ) {

			int r, c, _r, _rt;

			for (r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
				for (c = 0, _rt = c * stt; c < columns; ++c, _rt += stt) {
					At[_rt + r] = A[_r + c];
				}
			}
		}

		template<typename T> inline void m_transpose( int rows, int columns, T * A, int sta, T * At, int stt ) {

					int r, c, _r, _rt;

					for (r = 0, _r = r * sta; r < rows; ++r, _r += sta) {
						for (c = 0, _rt = c * stt; c < columns; ++c, _rt += stt) {
							At[_rt + r] = A[_r + c];
						}
					}
				}

	}
}

#endif /* M_BLAOP_HPP_ */
