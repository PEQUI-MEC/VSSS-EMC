/*
 * MatrixOperationBase.hpp
 *
 *  Created on: Mar 3, 2013
 *      Author: gustavo
 */

#ifndef MATRIX_OPERATION_BASE_HPP_
#define MATRIX_OPERATION_BASE_HPP_

namespace mat {

	template<typename T> class Matrix;

	template<class Derived> class MatrixOperationBase {
		public:
			typedef Derived DerivedClassType;

			MatrixOperationBase();
			virtual ~MatrixOperationBase();

			inline const Derived& get_derived() const;
			inline virtual void info() {
			}
	};

	template<class Derived> MatrixOperationBase<Derived>::MatrixOperationBase() {
	}
	template<class Derived> MatrixOperationBase<Derived>::~MatrixOperationBase() {
	}
	template<class Derived> const Derived& MatrixOperationBase<Derived>::get_derived() const {
		return static_cast<const Derived&>( *this );
	}

}

#endif /* MATRIX_OPERATION_BASE_HPP_ */
