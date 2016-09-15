/*
 * MatrixBase.hpp
 *
 *  Created on: Feb 20, 2013
 *      Author: gustavo
 */

#ifndef MATRIXBASE_HPP_
#define MATRIXBASE_HPP_

namespace mat {

	template<typename T> class Matrix;

	template<class Derived> class MatrixBase {
		public:
			typedef Derived DerivedClassType;

			MatrixBase();
			virtual ~MatrixBase();

			const Derived& get_derived() const;
			virtual void info() {
			}
	};

	template<class Derived> MatrixBase<Derived>::MatrixBase() {
	}
	template<class Derived> MatrixBase<Derived>::~MatrixBase() {
	}
	template<class Derived> const Derived& MatrixBase<Derived>::get_derived() const {
		return static_cast<const Derived&>( *this );
	}

}

#endif /* MATRIXBASE_HPP_ */
