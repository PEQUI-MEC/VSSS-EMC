/*
 * File:   vblaop.hpp
 * Author: gustavo
 *
 * Created on August 27, 2012, 12:08 PM
 */

#ifndef IMPL_VBLAOP_HPP
#define	IMPL_VBLAOP_HPP

//#include <math.h>

#include "config.hpp"
#include "vbop.hpp"

/* = Operations ========================================================= */

namespace impl {
	namespace math {

		/**
		 * u[0...m-1] = - v[0...m-1]
		 * @param m
		 * @param u
		 * @param incu
		 * @param v
		 * @param incv
		 */
		template<typename T> inline void v_move_neg( int m, T * u, int incu, const T * v, int incv ) {

			if (incu != 1 || incv != 1) {

				for (; m > 0; --m, u += incu, v += incv) {
					*u = -(*v);
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;
				int iv = incv * __N__;

				for (; n4 > 0; --n4, u += iu, v += iv) {
					u[0] = -v[0];
					u[1] = -v[1];
					u[2] = -v[2];
					u[3] = -v[3];
				}
				for (; left > 0; --left, u += incu, v += incv) {
					u[0] = -v[0];
				}

			}

		}

		/**
		 * u[0...m-1] = alpha * v[0...m-1]
		 * @param m
		 * @param u
		 * @param incu
		 * @param v
		 * @param incv
		 * @param scale
		 */
		template<typename T> inline void v_move_scale( int m, T * u, int incu, const T * v, int incv, T alpha ) {

			if (incu != 1 || incv != 1) {

				for (; m > 0; --m, u += incu, v += incv) {
					*u = alpha * (*v);
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;
				int iv = incv * __N__;

				for (; n4 > 0; --n4, u += iu, v += iv) {
					u[0] = alpha * v[0];
					u[1] = alpha * v[1];
					u[2] = alpha * v[2];
					u[3] = alpha * v[3];
				}
				for (; left > 0; --left, u += incu, v += incv) {
					u[0] = alpha * v[0];
				}

			}

		}

		/**
		 * This function calculates the dot|inner|scalar product of the vectors u and v.
		 * <p>
		 * result = u[0]*v[0] + u[1]*v[1] + ... + u[m-1]*v[m-1]
		 * resutl = <u,v>
		 * <p>
		 * @param m - number of elements of each vector.
		 * @param u - pointer to the first vector element.
		 * @param incu - stride of u. Number of elements in memory from the first element to the next.
		 * @param v - pointer to the first vector element.
		 * @param incv - stride of v. Number of elements in memory from the first element to the next.
		 * @return the dot product.
		 */
		template<typename T> inline T v_dot( int m, const T * u, int incu, const T * v, int incv ) {

			T result = 0;

			if (incu != 1 || incv != 1) {

				for (; m > 0; --m, u += incu, v += incv) {
					result += (*u) * (*v);
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;
				int iv = incv * __N__;

				for (; n4 > 0; --n4, u += iu, v += iv) {

					result += u[0] * v[0] + u[1] * v[1] + u[2] * v[2] + u[3] * v[3];
				}
				for (; left > 0; --left, u += incu, v += incv) {
					result += u[0] * v[0];
				}

			}

			return result;
		}

		/**
		 * u[0...m-1]  += v[0...m-1]
		 *
		 * @param m
		 * @param u
		 * @param incu
		 * @param v
		 * @param incv
		 */
		template<typename T> inline void v_add( int m, T * u, int incu, const T * v, int incv ) {

			if (incu != 1 || incv != 1) {

				for (; m > 0; --m, u += incu, v += incv) {
					*u += *v;
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;
				int iv = incv * __N__;

				for (; n4 > 0; --n4, u += iu, v += iv) {
					u[0] += v[0];
					u[1] += v[1];
					u[2] += v[2];
					u[3] += v[3];
				}
				for (; left > 0; --left, u += incu, v += incv) {
					u[0] += v[0];
				}

			}

		}

		/**
		 * u[0...m-1] += alpha * v[0...m-1]
		 *
		 * @param m
		 * @param u
		 * @param incu
		 * @param v
		 * @param incv
		 * @param alpha
		 */
		template<typename T> inline void v_add_scale( int m, T * u, int incu, const T * v, int incv, T alpha ) {

			if (incu != 1 || incv != 1) {

				for (; m > 0; --m, u += incu, v += incv) {
					*u += alpha * *v;
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;
				int iv = incv * __N__;

				for (; n4 > 0; --n4, u += iu, v += iv) {
					u[0] += alpha * v[0];
					u[1] += alpha * v[1];
					u[2] += alpha * v[2];
					u[3] += alpha * v[3];
				}
				for (; left > 0; --left, u += incu, v += incv) {
					u[0] += alpha * v[0];
				}

			}

		}

		/**
		 * u[0...m-1]  -= v[0...m-1]
		 *
		 * @param m
		 * @param u
		 * @param incu
		 * @param v
		 * @param incv
		 * @param scale
		 */
		template<typename T> inline void v_sub( int m, T * u, int incu, const T * v, int incv ) {

			if (incu != 1 || incv != 1) {

				for (; m > 0; --m, u += incu, v += incv) {
					*u -= *v;
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;
				int iv = incv * __N__;

				for (; n4 > 0; --n4, u += iu, v += iv) {
					u[0] -= v[0];
					u[1] -= v[1];
					u[2] -= v[2];
					u[3] -= v[3];
				}
				for (; left > 0; --left, u += incu, v += incv) {
					u[0] -= v[0];
				}

			}

		}

		/**
		 * u[0...m-1] -= alpha * v[0...m-1]
		 *
		 * @param m
		 * @param u
		 * @param incu
		 * @param v
		 * @param incv
		 * @param alpha
		 */
		template<typename T> inline void v_sub_scale( int m, T * u, int incu, const T * v, int incv, T alpha ) {

			v_add_scale( m, u, incu, v, incv, -alpha );

		}

		/**
		 * u[0...m-1] += alpha
		 * @param m
		 * @param u
		 * @param incu
		 * @param inc
		 */
		template<typename T> inline void v_inc( int m, T * u, int incu, T alpha ) {

			if (incu != 1) {

				for (; m > 0; --m, u += incu) {
					*u += alpha;
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;

				for (; n4 > 0; --n4, u += iu) {
					u[0] += alpha;
					u[1] += alpha;
					u[2] += alpha;
					u[3] += alpha;
				}
				for (; left > 0; --left, u += incu) {
					u[0] += alpha;
				}

			}

		}

		/**
		 * u[0...m-1] -= alpha
		 * @param m
		 * @param u
		 * @param incu
		 * @param inc
		 */
		template<typename T> inline void v_dec( int m, T * u, int incu, T alpha ) {

			v_inc( m, u, incu, -alpha );

		}

		/**
		 * u[0...m-1] *= alpha
		 *
		 * @param m
		 * @param u
		 * @param incu
		 * @param alpha
		 */
		template<typename T> inline void v_scale( int m, T * u, int incu, T alpha ) {

			if (incu != 1) {

				for (; m > 0; --m, u += incu) {
					*u *= alpha;
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;

				for (; n4 > 0; --n4, u += iu) {
					u[0] *= alpha;
					u[1] *= alpha;
					u[2] *= alpha;
					u[3] *= alpha;
				}
				for (; left > 0; --left, u += incu) {
					u[0] *= alpha;
				}

			}

		}

		/**
		 * This function computes a element-by-element k power of a vector.
		 * <p>
		 * u[0...m-1] = u[0...m-1]^alpha
		 * <p>
		 * @param m - number of elements.
		 * @param u - pointer to the first element of input vector.
		 * @param incu - increment to the next element of input vector.
		 * @param alpha - the power parameter.
		 */
		template<typename T> inline void v_pow( int m, T * u, int incu, T alpha ) {

			for (; m > 0; --m, u += incu) {
				//*u = (T) pow( (double) *u, (double) alpha );
				*u = (T) pow( *u, alpha );
			}

		}

		/**
		 * This function calculate the norm of a vector. This procedure uses
		 * normalization to avoid overflow.
		 * <p>
		 * norm = sqrt( u[0...m-1] * u[0...m-1] )
		 * <p>
		 * @param m - number of elements of the vector.
		 * @param u - pointer to the first vector element.
		 * @param incu - stride of u. Number of elements in memory from the first element to the next element.
		 * @return
		 */
		template<typename T> inline T v_norm_safe( int m, const T * u, int incu ) {

			T p, dp;
			T max_val = v_max( m, u, incu, NULL );

			dp = 0;

			for (; m > 0; --m, u += incu) {
				p = *u / max_val;
				dp += p * p;
			}

			dp = sqrt( dp * max_val );

			return dp;
		}

		template<typename T> inline T v_norm( int m, const T * u, int incu ) {

			T dp;

			dp = v_dot( m, u, incu, u, incu );
			dp = sqrt( dp );

			return dp;
		}

		/**
		 * This function calculates the outer product of two vectors u and v. The result is stored in O.
		 * <p>
		 * M = u * u^T
		 * <p>
		 * @param m - number of elements of each vector.
		 * @param u - pointer to the first vector element.
		 * @param incu - stride of v. Number of elements in memory from the first element to the next element.
		 * @param v - pointer to the first vector element.
		 * @param incv - stride of v. Number of elements in memory from the first element to the next element.
		 * @param M - pointer to the first element of the outer product matrix.
		 * @param stm - stride of M. Number of elements in memory from the first row to the next row.
		 */
		template<typename T> inline void v_outer( int m, const T * u, int incu, const T * v, int incv, T * M, int stm ) {
			int r, c;

			const T * vi;

			for (r = 0; r < m; r++) {

				vi = v;

				*M = 0.0;

				for (c = 0; c < m; c++) {

					*M += *u * *vi;

					vi += incv;
					M++;
				}

				M += (stm - m);
				u += incu;
			}

		}

		/**
		 * This function computes the vector squared length.
		 * <p>
		 * len = u[0...m-1].^2
		 * <p>
		 * @param m - number of elements.
		 * @param u - pointer to the first element of input vector.
		 * @param incu - increment to the next element of input vector.
		 *
		 * @return squared length.
		 */
		template<typename T> inline T v_length2( int m, T * u, int incu ) {

			T len = 0;

			for (; m > 0; --m, u += incu)
				len += (*u) * (*u);

			return len;
		}
		template<typename T> inline T v_length( int m, T * u, int incu ) {
			T len = v_length2( m, u, incu );
			return sqrt( len );
		}

		template<typename T> inline T v_norm_inf( int m, T * v, int incv ) {

			T vn;
			int i;

			vn = __abs(*v);
			for (i = 0; i < m; ++i) {

				if (*v > vn) vn = *v;

				v += incv;
			}
			return vn;
		}

		template<typename T> inline T v_norm1( int m, T * v, int incv ) {

			T vn;
			int i;

			vn = 0;
			for (i = 0; i < m; ++i) {
				vn += __abs(*v);
				v += incv;
			}

			return vn;
		}

		template<typename T> inline T v_norm2( int m, T * v, int incv ) {

			T vn;
			int i;

			vn = 0;
			for (i = 0; i < m; ++i) {
				vn += (*v) * (*v);
				v += incv;
			}
			vn = sqrt( vn );

			return vn;
		}

		template<typename T> inline T v_normp( int m, T * v, int incv, double p ) {

			T vn;
			int i;

			T tmp;
			vn = 0;
			for (i = 0; i < m; ++i) {
				tmp = __abs(*v);
				vn += pow( tmp, p );
				v += incv;
			}
			vn = pow( vn, 1.0 / p );

//			if (p % 2 == 0) {
//
//				vn = 0;
//				for (i = 0; i < m; ++i) {
//					vn += pow( *v, p );
//					v += incv;
//				}
//				vn = pow( vn, 1 / p );
//
//			} else {
//
//				T tmp;
//				vn = 0;
//				for (i = 0; i < m; ++i) {
//					tmp = __abs(*v);
//					vn += pow( tmp, p );
//					v += incv;
//				}
//				vn = pow( vn, 1.0 / p );
//
//			}

			return vn;
		}

	}
}
#endif	/* IMPL_VBLAOP_HPP */

