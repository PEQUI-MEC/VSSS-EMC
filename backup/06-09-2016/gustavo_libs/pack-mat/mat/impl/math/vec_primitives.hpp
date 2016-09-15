/* 
 * File:   blao_vector_primitives.h
 * Author: gustavo
 *
 * Created on June 15, 2012, 4:03 AM
 */

#ifndef IMPL_VEC_PRIMITIVES_HPP
#define	IMPL_VEC_PRIMITIVES_HPP

namespace impl {
	namespace math {

		/**
		 * This function computes a element-by-element k power of a vector.
		 * @param m - number of elements.
		 * @param u - pointer to the first element of input vector.
		 * @param incu - increment to the next element of input vector.
		 * @param k - the power parameter.
		 * @param v - pointer to the first element of output vector.
		 * @param incv - increment to the next element of output vector.
		 */
		template<typename T> inline void __vpow(int m, T * u, int incu, double k, T * v, int incv) {

			do {
				m--;
				*v = pow((double) *u, k);
				u += incu;
				v += incv;

			} while (m >= 0);
		}

		/**
		 * This function calculates the dot/inner/scalar product of the vectors u and v.
		 * @param m - number of elements of each vector.
		 * @param u - pointer to the first vector element.
		 * @param incu - stride of u. Number of elements in memory from the first element to the next.
		 * @param v - pointer to the first vector element.
		 * @param incv - stride of v. Number of elements in memory from the first element to the next.
		 * @return the dot product.
		 */
		template<typename T> inline T __vdot(int m, T * u, int incu, T * v, int incv) {

			T dot;

			dot = 0;
			while (m) {

				dot += *u * *v;

				u += incu;
				v += incv;
				m--;
			}

			return dot;
		}

		/**
		 * This function calculate the norm of a vector.
		 * @param m - number of elements of the vector.
		 * @param u - pointer to the first vector element.
		 * @param incu - stride of u. Number of elements in memory from the first element to the next.
		 * @return
		 */
		template<typename T> inline T __vnorm(int m, T * u, int incu) {

			T norm;

			norm = __vdot(m, u, incu, u, incu);

			norm = sqrt(norm);
			return norm;
		}

		/**
		 * This function calculates the outer product of two vectors u and v. The result is stored in O.
		 * @param m - number of elements of each vector.
		 * @param u - pointer to the first vector element.
		 * @param incu - stride of v. Number of elements in memory from the first element to the next.
		 * @param v - pointer to the first vector element.
		 * @param incv - stride of v. Number of elements in memory from the first element to the next.
		 * @param O - pointer to the first element of the outer product matrix.
		 * @param sto - stride of O. Number of elements in memory from the first row to the next.
		 */
		template<typename T> inline void __vouter(int m, T * u, int incu, T * v, int incv, T * O, int sto) {
			int r, c;

			T * vi;

			for (r = 0; r < m; r++) {

				vi = v;

				*O = 0.0;

				for (c = 0; c < m; c++) {

					*O += *u * *vi;

					vi += incv;
					O++;
				}

				O += (sto - m);
				u += incu;
			}

		}

		/**
		 * This function increments a vector by a factor k.
		 * @param m - number of elements.
		 * @param u - pointer to the first element of vector.
		 * @param incu - increment to the next element of vector.
		 * @param k - the additive parameter.
		 */
		template<typename T> inline void __vinc(int m, T * u, int incu, T * v, int incv, T k) {

			do {
				m--;

				*v = *u + k;
				u += incu;
				v += incv;

			} while (m >= 0);
		}

	}
}
#endif	/* IMPL_VEC_PRIMITIVES_H */

