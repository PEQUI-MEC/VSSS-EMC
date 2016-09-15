/*
 * scale.hpp
 *
 *  Created on: May 9, 2013
 *      Author: gustavo
 */

#ifndef SCALE_HPP_
#define SCALE_HPP_

#include "MatrixOperationBase.hpp"
#include "../msg/common_msg.hpp"
#include <impl/mat/m_blaop.hpp>

namespace mat {

	template<typename T> class Matrix;
	template<typename T> class MatrixView;

	template<typename T> class MatScale: public MatrixOperationBase<MatScale<T> > {
			Matrix<T> M;
			double k;
		public:
			MatScale(const Matrix<T>& M_in, double k) :
					M(), k(k) {
				M.refTo(M_in);
			}

			template<class Tout> void apply(Matrix<Tout>& A) const {

				A.set_size(M.nrows, M.ncols);

				impl::matops::m_scale(  //
						M.nrows, M.ncols, M.data, M.stride,  //
						k,  //
						A.data, A.stride);
			}

			template<class Tout> void apply(MatrixView<Tout>& A) const {

				__assert_msg((A.nrows == M.nrows && A.ncols == M.ncols), INVALID_MATRIX_DIMENSION);

				impl::matops::m_scale(  //
						M.nrows, M.ncols, M.data, M.stride,  //
						k,  //
						A.data, A.stride);
			}

	};

	template<class C> MatScale<typename C::ElementType> operator *(const MatrixBase<C>& M, double k) {
		MatScale<typename C::ElementType> op(M.get_derived(), k);
		return op;
	}
	template<class C> MatScale<typename C::ElementType> operator /(const MatrixBase<C>& M, double k) {
		MatScale<typename C::ElementType> op(M.get_derived(), 1. / k);
		return op;
	}

	template<class C> MatScale<typename C::ElementType> operator *(double k, const MatrixBase<C>& M) {
		MatScale<typename C::ElementType> op(M.get_derived(), k);
		return op;
	}
	template<class C> MatScale<typename C::ElementType> operator /(double k, const MatrixBase<C>& M) {
		MatScale<typename C::ElementType> op(M.get_derived(), 1. / k);
		return op;
	}

	template<class C> MatScale<double> operator *(const MatrixOperationBase<C>& OP, double k) {

		Matrix<double> M;
		M = OP;

		MatScale<double> op(M, k);
		return op;
	}

	template<class C> MatScale<double> operator /(const MatrixOperationBase<C>& OP, double k) {

		Matrix<double> M;
		M = OP;

		MatScale<double> op(M, 1. / k);
		return op;
	}

	template<class C> MatScale<double> operator *(double k, const MatrixOperationBase<C>& OP) {

		Matrix<double> M;
		M = OP;

		MatScale<double> op(M, k);
		return op;
	}

	template<class C> MatScale<double> operator /(double k, const MatrixOperationBase<C>& OP) {

		Matrix<double> M;
		M = OP;

		MatScale<double> op(M, 1. / k);
		return op;
	}

}

#endif /* SCALE_HPP_ */
