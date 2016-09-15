/*
 * mult.hpp
 *
 *  Created on: May 9, 2013
 *      Author: gustavo
 */

#ifndef MULT_HPP_
#define MULT_HPP_

#include "MatrixOperationBase.hpp"
#include "../msg/common_msg.hpp"

#include <impl/mat/m_blaop.hpp>

namespace mat {

	template<typename T> class Matrix;
	template<typename T> class MatrixView;

	template<typename TL, typename TR> class MatMult: public MatrixOperationBase<MatMult<TL, TR> > {
			Matrix<TL> L;
			Matrix<TR> R;

		public:
			MatMult( const Matrix<TL>& l, const Matrix<TR>& r ) :
					L(), R() {

				L.refTo( l );
				R.refTo( r );
				__assert_msg( L.ncols == R.nrows, INVALID_MATRIX_DIMENSION );

			}

			template<typename T3> void apply( Matrix<T3>& M ) const {

				M.set_size( L.nrows, R.ncols );
				impl::matops::m_mult( L.nrows, L.ncols, L.data, L.stride, //
						R.nrows, R.ncols, R.data, R.stride, //
						M.data, M.stride );

			}

			template<typename T3> void apply( MatrixView<T3>& M ) const {

				__assert_msg( L.nrows == M.nrows && R.ncols == M.ncols, INVALID_MATRIX_DIMENSION );

				impl::matops::m_mult( L.nrows, L.ncols, L.data, L.stride, //
						R.nrows, R.ncols, R.data, R.stride, //
						M.data, M.stride );

			}

	};

	template<typename T1, typename T2> MatMult<T1, T2> operator *( const Matrix<T1>& L, const Matrix<T2>& R ) {
		MatMult<T1, T2> mop( L, R );
		return mop;
	}

	template<class MatOp, typename T3> MatMult<double, T3> operator *( const MatrixOperationBase<MatOp>& LOp, const Matrix<T3>& R ) {

		Matrix<double> M;
		M = LOp;
		MatMult<double, T3> mop( M, R );
		return mop;
	}

	template<typename T1, class MatOp> MatMult<T1, double> operator *( const Matrix<T1>& L, const MatrixOperationBase<MatOp>& ROp ) {

		Matrix<double> M;
		M = ROp;
		MatMult<T1, double> mop( L, M );
		return mop;
	}

	template<class MatOp1, class MatOp2> MatMult<double, double> operator *( const MatrixOperationBase<MatOp1>& LOp, const MatrixOperationBase<MatOp2>& ROp ) {

		Matrix<double> ML, MR;
		ML = LOp;
		MR = ROp;
		MatMult<double, double> mop( ML, MR );
		return mop;
	}
}

#endif /* MULT_HPP_ */
