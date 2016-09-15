/*
 * sub.hpp
 *
 *  Created on: Aug 20, 2013
 *      Author: gtl
 */

#ifndef SUB_HPP_
#define SUB_HPP_

#include "MatrixOperationBase.hpp"
#include "../msg/common_msg.hpp"

#include <impl/mat/m_blaop.hpp>

namespace mat {

	template<typename T> class Matrix;
	template<typename T> class MatrixView;

	template<typename TL, typename TR> class MatrixSubtraction: public MatrixOperationBase<MatrixSubtraction<TL, TR> > {
			Matrix<TL> L;
			Matrix<TR> R;

		public:
			MatrixSubtraction(const Matrix<TL>& l, const Matrix<TR>& r) :
					L(), R() {

				L.refTo(l);
				R.refTo(r);
				__assert_msg(L.nrows == R.nrows && L.ncols == R.ncols, INVALID_MATRIX_DIMENSION);

			}

			template<typename T3> void apply(Matrix<T3>& M) const {

				M.set_size(L.nrows, L.ncols);
				impl::matops::m_sub(L.nrows, L.ncols, L.data, L.stride,  //
						R.data, R.stride,  //
						M.data, M.stride);

			}

			template<typename T3> void apply(MatrixView<T3>& M) const {

				__assert_msg(L.nrows == M.nrows && L.ncols == M.ncols, INVALID_MATRIX_DIMENSION);

				impl::matops::m_sub(L.nrows, L.ncols, L.data, L.stride,  //
						R.data, R.stride,  //
						M.data, M.stride);

			}

	};

	template<typename T1, typename T2> MatrixSubtraction<T1, T2> operator -(const Matrix<T1>& L, const Matrix<T2>& R) {
		MatrixSubtraction<T1, T2> mop(L, R);
		return mop;
	}

	template<class MatOp, typename T3> MatrixSubtraction<double, T3> operator -(const MatrixOperationBase<MatOp>& LOp, const Matrix<T3>& R) {

		Matrix<double> M;
		M = LOp;
		MatrixSubtraction<double, T3> mop(M, R);
		return mop;
	}

	template<typename T1, class MatOp> MatrixSubtraction<T1, double> operator -(const Matrix<T1>& L, const MatrixOperationBase<MatOp>& ROp) {

		Matrix<double> M;
		M = ROp;
		MatrixSubtraction<T1, double> mop(L, M);
		return mop;
	}

	template<class MatOp1, class MatOp2> MatrixSubtraction<double, double> operator -(const MatrixOperationBase<MatOp1>& LOp, const MatrixOperationBase<MatOp2>& ROp) {

		Matrix<double> ML, MR;
		ML = LOp;
		MR = ROp;
		MatrixSubtraction<double, double> mop(ML, MR);
		return mop;
	}

}

#endif /* SUB_HPP_ */
