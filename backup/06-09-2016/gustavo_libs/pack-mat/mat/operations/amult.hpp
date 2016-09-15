/*
 * amult.hpp
 *
 *  Created on: Jul 4, 2013
 *      Author: gustavo
 */

#ifndef AMULT_HPP_
#define AMULT_HPP_


#include "MatrixOperationBase.hpp"
#include "../msg/common_msg.hpp"

#include <impl/mat/m_blaop.hpp>

namespace mat {

	template<typename T> class Matrix;
	template<typename T> class MatrixView;

	template<typename TL, typename TR> class ArrayMult: public MatrixOperationBase<ArrayMult<TL, TR> > {
			Matrix<TL> L;
			Matrix<TR> R;

		public:
			ArrayMult( const Matrix<TL>& l, const Matrix<TR>& r ) :
					L(), R() {

				L.refTo( l );
				R.refTo( r );

				__assert_msg( L.n_rows == R.n_rows, INVALID_MATRIX_DIMENSION );
				__assert_msg( L.n_columns == R.n_columns, INVALID_MATRIX_DIMENSION );


			}

			template<typename T3> void apply( Matrix<T3>& M ) const {

				M.set_size( L.n_rows, R.n_columns );

				impl::arrayops::mult(M.data, L.data, R.data, L.n_elements);

			}
	};

	template<typename T1, typename T2> ArrayMult<T1, T2> amult( const Matrix<T1>& L, const Matrix<T2>& R ) {
		ArrayMult<T1, T2> mop( L, R );
		return mop;
	}

	template<class MatOp, typename T3> ArrayMult<double, T3> amult( const MatrixOperationBase<MatOp>& LOp, const Matrix<T3>& R ) {

		Matrix<double> M;
		M = LOp;
		ArrayMult<double, T3> mop( M, R );
		return mop;
	}

	template<typename T1, class MatOp> ArrayMult<T1, double> amult( const Matrix<T1>& L, const MatrixOperationBase<MatOp>& ROp ) {

		Matrix<double> M;
		M = ROp;
		ArrayMult<T1, double> mop( L, M );
		return mop;
	}

	template<class MatOp1, class MatOp2> ArrayMult<double, double> amult( const MatrixOperationBase<MatOp1>& LOp, const MatrixOperationBase<MatOp2>& ROp ) {

		Matrix<double> ML, MR;
		ML = LOp;
		MR = ROp;
		ArrayMult<double, double> mop( ML, MR );
		return mop;
	}
}

#endif /* AMULT_HPP_ */
