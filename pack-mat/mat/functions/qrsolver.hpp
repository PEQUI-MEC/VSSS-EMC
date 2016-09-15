/*
 * qrsolver.hpp
 *
 *  Created on: Oct 22, 2012
 *      Author: gustavo
 */

#ifndef IMP_QRSOLVER_HPP
#define IMP_QRSOLVER_HPP

#include "../Matrix.hpp"
#include "MatrixFunctionBase.hpp"

#include <impl/math/qr_householder.hpp>
#include <impl/math/triangular_solver.hpp>

namespace mat {

	/**
	 * solve Ax = B
	 */
	template<typename T> class QRSolver: public MatrixFunctionBase<QRSolver<T> > {
		private:

			const Matrix<T>& A;
			const Matrix<T>& B;

		public:

			QRSolver(const Matrix<T>& A, const Matrix<T>& B) :
					A(A), B(B) {
			}

			bool apply(Matrix<T>& X) const {

				Matrix<T> H;
				Matrix<T> Qt;
				Matrix<T> R;
				Matrix<T> Tmp;

				X.set_size(B.nrows, B.ncols);

				H = A;
				Qt.set_size(A.nrows, A.nrows);
				R.set_size(A.nrows, A.ncols);
				impl::math::qr_householder_full(H.nrows, H.ncols, H.data, H.stride, Qt.data, Qt.stride, R.data, R.stride);

				Tmp = Qt * B;
				impl::math::back_substitution(R.nrows, R.data, R.stride, Tmp.ncols, Tmp.data, Tmp.stride, X.data, X.stride);

				return true;

			}
	};

	template<typename T> QRSolver<T> qrsolver(Matrix<T>& A, Matrix<T>& B) {
		return QRSolver<T>(A, B);
	}

}

#endif /* IMP_QRSOLVER_HPP */
