/*
 * Size.hpp
 *
 *  Created on: Mar 9, 2013
 *      Author: gustavo
 */

#ifndef SIZE_HPP_
#define SIZE_HPP_

#include "impl/util/mem.hpp"

namespace mat {

	template<int DIM> class Size {
		public:
			int dim[DIM];

			int& operator[]( int i ) const {
				return impl::util::mem::access::rw( dim[i] );
			}

			bool operator ==( const Size<DIM>& s ) const {
				for (int i = 0; i < DIM; ++i) {
					if (dim[i] != s.dim[i]) return false;
				}
				return true;
			}
	};
}

#endif /* SIZE_HPP_ */
