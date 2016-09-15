/*
 * File:   vbop.hpp
 * Author: gustavo
 *
 * Created on August 10, 2012, 1:57 AM
 */

#ifndef IMPL_VBOP_HPP
#define	IMPL_VBOP_HPP

#include <stdio.h>

#include "config.hpp"
#include "math_macros.h"

namespace impl {
	namespace math {

		/* = Data visualization ================================================= */

		/**
		 *
		 * @param m
		 * @param u
		 * @param incu
		 * @param val
		 */
		template<typename T> inline void v_print( int m, const T * u, int incu ) {

			for( ; m > 0; --m, u += incu ) {
				printf( "%.3f\n", *u );
			}

			printf( "\n\n" );

		}

		/* = Data manipulation ================================================== */

		/**
		 * u[0...m-1] = val
		 * @param m
		 * @param u
		 * @param incu
		 * @param val
		 */
		template<typename T> inline void v_set( int m, T * u, int incu, T val ) {

			if( incu != 1 ) {

				for( ; m > 0; --m, u += incu ) {
					*u = val;
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;

				for( ; n4 > 0; --n4, u += iu ) {
					u[0] = u[1] = u[2] = u[3] = val;
				}
				for( ; left > 0; --left, u += incu ) {
					u[0] = val;
				}

			}

		}

		/**
		 * u[0...m-1] = v[0...m-1]
		 * @param m
		 * @param u
		 * @param incu
		 * @param v
		 * @param incv
		 */
		template<typename T> inline void v_copy( int m, T * u, int incu, const T * v, int incv ) {

			if( incu != 1 || incv != 1 ) {

				for( ; m > 0; --m, u += incu, v += incv ) {
					*u = *v;
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;
				int iv = incv * __N__;

				for( ; n4 > 0; --n4, u += iu, v += iv ) {
					u[0] = v[0];
					u[1] = v[1];
					u[2] = v[2];
					u[3] = v[3];
				}
				for( ; left > 0; --left, u += incu, v += incv ) {
					u[0] = v[0];
				}

			}

		}

		/**
		 * u[0...m-1] = v[0...m-1]
		 * @param m
		 * @param u
		 * @param incu
		 * @param v
		 * @param incv
		 */
		template<typename T> inline void v_move( int m, T * u, int incu, const T * v, int incv ) {

			if( incu != 1 || incv != 1 ) {

				for( ; m > 0; --m, u += incu, v += incv ) {
					*u = *v;
				}

			} else {

				int n4 = m / __N__;
				int left = m % __N__;
				int iu = incu * __N__;
				int iv = incv * __N__;

				for( ; n4 > 0; --n4, u += iu, v += iv ) {
					u[0] = v[0];
					u[1] = v[1];
					u[2] = v[2];
					u[3] = v[3];
				}
				for( ; left > 0; --left, u += incu, v += incv ) {
					u[0] = v[0];
				}

			}

		}

		template<typename T> inline T v_sum( int m, T * u, int incu ) {

			int i;
			T s;
			s = 0.0;
			for( i = 0; i < m; i++ ) {
				s += *u;
				u += incu;
			}
			return s;
		}

		template<typename T> inline T v_max( int m, const T * u, int incu, int * imax = NULL ) {

			T max_val;
			int i;

			max_val = *u;

			if( imax == NULL ) {

				for( i = 1; i < m; ++i, u += incu )
					max_val = __max( max_val, *u );

			} else {
				/* Get index of max value */
				*imax = 0;
				for( i = 0; i < m; ++i, u += incu ) {
					if( *u > max_val ) {
						max_val = *u;
						*imax = i;
					}
				}
			}

			return max_val;

		}

		template<typename T> inline T v_min( int m, const T * u, int incu, int * imin ) {

			T min_val;
			int i;

			min_val = *u;

			if( imin == NULL ) {

				for( i = 1; i < m; ++i, u += incu )
					min_val = __min( min_val, *u );

			} else {
				/* Get index of max value */
				*imin = 0;
				for( i = 1; i < m; ++i, u += incu ) {
					if( *u < min_val ) {
						min_val = *u;
						*imin = i;
					}
				}
			}

			return min_val;

		}

	}
}

#endif	/* IMPL_VBOP_HPP */

