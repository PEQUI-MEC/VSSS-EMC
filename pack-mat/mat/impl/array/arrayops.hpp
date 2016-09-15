/*
 * arrayops.hpp
 *
 *  Created on: Feb 24, 2013
 *      Author: gustavo
 */

#ifndef ARRAYOPS_HPP_
#define ARRAYOPS_HPP_

#include "array_statistics.hpp"

namespace impl {
	namespace arrayops {

		template<typename Tdest, typename Torig> void copy_big( size_t size, Tdest * dest, const Torig * orig ) {

			switch (size) {
				default:

					for (size_t i = 0; i < size; ++i) {
						*dest = *orig;
						++dest;
						++orig;
					}
					break;

				case 32:
					dest[31] = orig[31];
					/* no break */
				case 31:
					dest[30] = orig[30];
					/* no break */
				case 30:
					dest[29] = orig[29];
					/* no break */
				case 29:
					dest[28] = orig[28];
					/* no break */
				case 28:
					dest[27] = orig[27];
					/* no break */
				case 27:
					dest[26] = orig[26];
					/* no break */
				case 26:
					dest[25] = orig[25];
					/* no break */
				case 25:
					dest[24] = orig[24];
					/* no break */
				case 24:
					dest[23] = orig[23];
					/* no break */
				case 23:
					dest[22] = orig[22];
					/* no break */
				case 22:
					dest[21] = orig[21];
					/* no break */
				case 21:
					dest[20] = orig[20];
					/* no break */
				case 20:
					dest[19] = orig[19];
					/* no break */
				case 19:
					dest[18] = orig[18];
					/* no break */
				case 18:
					dest[17] = orig[17];
					/* no break */
				case 17:
					dest[16] = orig[16];
					/* no break */
				case 16:
					dest[15] = orig[15];
					/* no break */
				case 15:
					dest[14] = orig[14];
					/* no break */
				case 14:
					dest[13] = orig[13];
					/* no break */
				case 13:
					dest[12] = orig[12];
					/* no break */
				case 12:
					dest[11] = orig[11];
					/* no break */
				case 11:
					dest[10] = orig[10];
					/* no break */
				case 10:
					dest[9] = orig[9];
					/* no break */
				case 9:
					dest[8] = orig[8];
					/* no break */
				case 8:
					dest[7] = orig[7];
					/* no break */
				case 7:
					dest[6] = orig[6];
					/* no break */
				case 6:
					dest[5] = orig[5];
					/* no break */
				case 5:
					dest[4] = orig[4];
					/* no break */
				case 4:
					dest[3] = orig[3];
					/* no break */
				case 3:
					dest[2] = orig[2];
					/* no break */
				case 2:
					dest[1] = orig[1];
					/* no break */
				case 1:
					dest[0] = orig[0];
					/* no break */
			}

		}
		template<typename T> void copy_big( size_t size, T * dest, const T * orig ) {

			switch (size) {
				default:

					for (size_t i = 0; i < size; ++i) {
						*dest = *orig;
						++dest;
						++orig;
					}
					break;

				case 32:
					dest[31] = orig[31];
					/* no break */
				case 31:
					dest[30] = orig[30];
					/* no break */
				case 30:
					dest[29] = orig[29];
					/* no break */
				case 29:
					dest[28] = orig[28];
					/* no break */
				case 28:
					dest[27] = orig[27];
					/* no break */
				case 27:
					dest[26] = orig[26];
					/* no break */
				case 26:
					dest[25] = orig[25];
					/* no break */
				case 25:
					dest[24] = orig[24];
					/* no break */
				case 24:
					dest[23] = orig[23];
					/* no break */
				case 23:
					dest[22] = orig[22];
					/* no break */
				case 22:
					dest[21] = orig[21];
					/* no break */
				case 21:
					dest[20] = orig[20];
					/* no break */
				case 20:
					dest[19] = orig[19];
					/* no break */
				case 19:
					dest[18] = orig[18];
					/* no break */
				case 18:
					dest[17] = orig[17];
					/* no break */
				case 17:
					dest[16] = orig[16];
					/* no break */
				case 16:
					dest[15] = orig[15];
					/* no break */
				case 15:
					dest[14] = orig[14];
					/* no break */
				case 14:
					dest[13] = orig[13];
					/* no break */
				case 13:
					dest[12] = orig[12];
					/* no break */
				case 12:
					dest[11] = orig[11];
					/* no break */
				case 11:
					dest[10] = orig[10];
					/* no break */
				case 10:
					dest[9] = orig[9];
					/* no break */
				case 9:
					dest[8] = orig[8];
					/* no break */
				case 8:
					dest[7] = orig[7];
					/* no break */
				case 7:
					dest[6] = orig[6];
					/* no break */
				case 6:
					dest[5] = orig[5];
					/* no break */
				case 5:
					dest[4] = orig[4];
					/* no break */
				case 4:
					dest[3] = orig[3];
					/* no break */
				case 3:
					dest[2] = orig[2];
					/* no break */
				case 2:
					dest[1] = orig[1];
					/* no break */
				case 1:
					dest[0] = orig[0];
					/* no break */
			}

		}
		template<typename Tdest, typename Torig> inline void copy( size_t size, Tdest * dest, const Torig * orig ) {

			switch (size) {
				default:
					copy_big( size, dest, orig );
					break;
				case 9:
					dest[8] = orig[8];
					/* no break */
				case 8:
					dest[7] = orig[7];
					/* no break */
				case 7:
					dest[6] = orig[6];
					/* no break */
				case 6:
					dest[5] = orig[5];
					/* no break */
				case 5:
					dest[4] = orig[4];
					/* no break */
				case 4:
					dest[3] = orig[3];
					/* no break */
				case 3:
					dest[2] = orig[2];
					/* no break */
				case 2:
					dest[1] = orig[1];
					/* no break */
				case 1:
					dest[0] = orig[0];
					/* no break */
			}

		}
		template<typename T> inline void copy( size_t size, T * dest, const T * orig ) {

			switch (size) {
				default:
					copy_big( size, dest, orig );
					break;
				case 9:
					dest[8] = orig[8];
					/* no break */
				case 8:
					dest[7] = orig[7];
					/* no break */
				case 7:
					dest[6] = orig[6];
					/* no break */
				case 6:
					dest[5] = orig[5];
					/* no break */
				case 5:
					dest[4] = orig[4];
					/* no break */
				case 4:
					dest[3] = orig[3];
					/* no break */
				case 3:
					dest[2] = orig[2];
					/* no break */
				case 2:
					dest[1] = orig[1];
					/* no break */
				case 1:
					dest[0] = orig[0];
					/* no break */
			}

		}

		template<typename T> inline void fill( size_t size, T * A, double val ) {

			size_t r;

			for (r = 0; r < size; r++) {
				*A = (T) val;
				A++;
			}
		}

		template<typename T> inline void inplace_add( size_t size, T * A, double val ) {
			for (int i = 0; i < size; ++i) {
				*A += val;
				++A;
			}
		}
		template<typename T> inline void inplace_sub( size_t size, T * A, double val ) {
			for (int i = 0; i < size; ++i) {
				*A -= val;
				++A;
			}
		}
		template<typename T> inline void inplace_mult( size_t size, T * A, double val ) {
			for (int i = 0; i < size; ++i) {
				*A *= val;
				++A;
			}
		}
		template<typename T> inline void inplace_div( size_t size, T * A, double val ) {
			for (int i = 0; i < size; ++i) {
				*A /= val;
				++A;
			}
		}
		template<typename T> inline void inplace_mod( size_t size, T * A, double val ) {
			for (int i = 0; i < size; ++i) {
				*A %= val;
				++A;
			}
		}

		template<typename Tdest, typename Torig> inline void inplace_add( Tdest * dest, const Torig * orig, int n_elements ) {
			for (int i = 0; i < n_elements; ++i) {
				*dest += *orig;
				++dest;
				++orig;
			}
		}
		template<typename Tdest, typename Torig> inline void inplace_sub( Tdest * dest, const Torig * orig, int n_elements ) {
			for (int i = 0; i < n_elements; ++i) {
				*dest -= *orig;
				++dest;
				++orig;
			}
		}
		template<typename Tdest, typename Torig> inline void inplace_mult( Tdest * dest, const Torig * orig, int n_elements ) {
			for (int i = 0; i < n_elements; ++i) {
				*dest *= *orig;
				++dest;
				++orig;
			}
		}
		template<typename Tdest, typename Torig> inline void inplace_div( Tdest * dest, const Torig * orig, int n_elements ) {
			for (int i = 0; i < n_elements; ++i) {

				if (*orig == 0) *dest = 0;
				else *dest /= *orig;
				++dest;
				++orig;
			}
		}

		template<typename Tdest, typename Ta, typename Tb> inline void add( Tdest * dest, const Ta * a, const Tb * b, int n_elements ) {
			for (int i = 0; i < n_elements; ++i) {
				*dest = *a + *b;
				++dest;
				++a;
				++b;
			}
		}
		template<typename Tdest, typename Ta, typename Tb> inline void sub( Tdest * dest, const Ta * a, const Tb * b, int n_elements ) {
			for (int i = 0; i < n_elements; ++i) {
				*dest = *a - *b;
				++dest;
				++a;
				++b;
			}
		}
		template<typename Tdest, typename Ta, typename Tb> inline void mult( Tdest * dest, const Ta * a, const Tb * b, int n_elements ) {
			for (int i = 0; i < n_elements; ++i) {
				*dest = *a * *b;
				++dest;
				++a;
				++b;
			}
		}
		template<typename Tdest, typename Ta, typename Tb> inline void div( Tdest * dest, const Ta * a, const Tb * b, int n_elements ) {
			for (int i = 0; i < n_elements; ++i) {

				if (*b == 0) *dest = 0;
				else *dest = *a / *b;
				++dest;
				++a;
				++b;
			}
		}

		template<typename T> inline size_t max( size_t size, T * A, T * MAX ) {

			size_t i;
			*MAX = *A;
			size_t i_max;

			for (i = 0; i < size; i++) {
				if (*A > *MAX) {
					*MAX = *A;
					i_max = i;
				}
				A++;
			}
			return i_max;
		}
	}
}

#endif /* ARRAYOPS_HPP_ */
