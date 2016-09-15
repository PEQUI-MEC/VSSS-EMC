/*
 * MatrixFunctionBase.hpp
 *
 *  Created on: Mar 20, 2013
 *      Author: gustavo
 */

#ifndef MATRIXFUNCTIONBASE_HPP_
#define MATRIXFUNCTIONBASE_HPP_

namespace mat {

	template<typename T> class Matrix;

	template<class Derived> class MatrixFunctionBase {
		public:
			typedef Derived DerivedClassType;

			MatrixFunctionBase();
			virtual ~MatrixFunctionBase();

			inline const Derived& get_derived() const;
			inline virtual void info() {
			}
	};

	template<class Derived> MatrixFunctionBase<Derived>::MatrixFunctionBase() {
	}
	template<class Derived> MatrixFunctionBase<Derived>::~MatrixFunctionBase() {
	}
	template<class Derived> const Derived& MatrixFunctionBase<Derived>::get_derived() const {
		return static_cast<const Derived&>( *this );
	}

}

#endif /* MATRIXFUNCTIONBASE_HPP_ */
