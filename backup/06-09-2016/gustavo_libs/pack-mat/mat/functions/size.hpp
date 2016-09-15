/*
 * size.hpp
 *
 *  Created on: Mar 9, 2013
 *      Author: gustavo
 */

#ifndef SIZE_FN_HPP_
#define SIZE_FN_HPP_

#include "../Size.hpp"

namespace mat {

	template<typename T> class Matrix;
	template<typename T> class MatrixView;

	template<typename T> Size<2> size( const Matrix<T>& A ) {
		Size<2> s;
		s[0] = A.nrows;
		s[1] = A.ncols;
		return s;
	}

	template<class C> struct __size_traits;
	template<typename T> struct __size_traits<Matrix<T> > {
			static const int value = 1;
	};
	template<typename T> struct __size_traits<MatrixView<T> > {
			static const int value = 1;
	};

	template<class C> Size<2> size( const MatrixBase<C>& MB, int junk = __size_traits<C>::value ) {
		Size<2> s;

		s[0] = MB.get_derived().nrows;
		s[1] = MB.get_derived().ncols;

		return s;
	}

}

#endif /* SIZE_FN_HPP_ */
