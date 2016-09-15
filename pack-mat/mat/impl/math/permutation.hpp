/*
 * permutation.hpp
 *
 *  Created on: Oct 17, 2012
 *      Author: gustavo
 */

#ifndef IMPL_PERMUTATION_HPP
#define IMPL_PERMUTATION_HPP

namespace impl {
	namespace math {

		template<typename T> void permutation_row( int m, int * e, T * P, int stp ) {

			int r, c;
			for( r = 0; r < m; ++r ) {
				// clear row
				for( c = 0; c < m; ++c ) {
					P[r * stp + c] = 0;
				}
				P[r * stp + e[r]] = 1;
			}

		}

		template<typename T> void permutation_column( int m, int * e, T * P, int stp ) {

			int r, c;
			for( c = 0; c < m; ++c ) {
				// clear column
				for( r = 0; r < m; ++r ) {
					P[r * stp + c] = 0;
				}
				P[e[r] * stp + c] = 1;
			}

		}

	}
}

#endif /* IMPL_PERMUTATION_HPP */
