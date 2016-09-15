/*
 * MatrixView.hpp
 *
 *  Created on: Feb 16, 2013
 *      Author: gustavo
 */

#ifndef MATRIXVIEW_HPP_
#define MATRIXVIEW_HPP_

#include <iomanip>	// for std::setprecision()
#include <sstream>	// for std::stringstream
#include <typeinfo>	// for typeid
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

/* -------------------------------------------------------------------------- */
//#include <impl/util/msg.h>
//#include <impl/mat/basic_operations.hpp>
//#include <impl/array/arrayops.hpp>
//#include <impl/math/math_macros.h>

/* -------------------------------------------------------------------------- */
#include "impl/util/mem.hpp"
using namespace impl;

/* -------------------------------------------------------------------------- */
#include "msg/common_msg.hpp"
#include "internal/debug.h"
#include "internal/MatRef.hpp"

/* -------------------------------------------------------------------------- */
#include "MatrixBase.hpp"
#include "MatrixView.hpp"
#include "Range.hpp"
#include "Size.hpp"

/* -------------------------------------------------------------------------- */
//#include "functions/MatrixFunctionBase.hpp"
//#include "operations/MatrixOperation.hpp"
namespace mat {

	template<typename T> class Matrix;

	template<typename T> class MatrixView: public MatrixBase<MatrixView<T> > {

		public:

			typedef T ElementType;

			// =============================================================
			// = Read only =================================================
			// =============================================================
			const Matrix<T>& M;

			const int row_aux;
			const int column_aux;

			const int nrows;
			const int ncols;
			const int n_elements;
			const int stride;
			T* const data;

		public:
			// =============================================================
			// = Basic Constructors
			// =============================================================
			MatrixView( const Matrix<T>& MIn, int r, int c, int nrows, int ncolumns );
			MatrixView( const MatrixView<T>& MIn, int r, int c, int nrows, int ncolumns );

		public:
			// =============================================================
			// = Copy constructors
			// =============================================================
			MatrixView( const MatrixView<T>& M );
			template<typename T2> explicit MatrixView( const MatrixView<T2>& M );

		public:
			// =============================================================
			// = Destructor
			// =============================================================
			~MatrixView();

		private:
			// =============================================================
			// = Private Auxiliary Methods
			// =============================================================
			T * __mem_ptr();

		public:
			// =============================================================
			// = Utility Methods
			// =============================================================
			void print( const char * extra_text = 0 ) const;

		public:
			// =============================================================
			// = Information Methods
			// =============================================================
			template<typename Tval> bool fill( Tval val );
			template<typename Tval> bool fill_diagonal( Tval val );

		public:
			// =============================================================
			// = Sub Matrices
			// =============================================================
			MatrixView<T> row( int r ) const;
			MatrixView<T> rows( int s, int e ) const;
			MatrixView<T> col( int r ) const;
			MatrixView<T> cols( int s, int e ) const;
			MatrixView<T> submat( int sr, int er, int sc, int ec ) const;

		public:
			// =============================================================
			// = Data Access Methods
			// =============================================================
			inline T& operator()( int idx ) const;
			inline T& operator()( int row, int col ) const;

		public:
			// =============================================================
			// = Assignment with primitive types
			// =============================================================
			MatrixView<T>& operator =( double val );
			MatrixView<T>& operator +=( double val );
			MatrixView<T>& operator -=( double val );
			MatrixView<T>& operator *=( double val );
			MatrixView<T>& operator /=( double val );

		public:
			// =============================================================
			// = Assignment with Matrix
			// =============================================================
			MatrixView<T>& operator =( const Matrix<T>& M );
			template<typename TM> MatrixView<T>& operator =( const Matrix<TM>& M );
			template<typename TM> MatrixView<T>& operator +=( const Matrix<TM>& M );
			template<typename TM> MatrixView<T>& operator -=( const Matrix<TM>& M );
			template<typename TM> MatrixView<T>& operator *=( const Matrix<TM>& M );
			template<typename TM> MatrixView<T>& operator /=( const Matrix<TM>& M );
			template<typename TM> MatrixView<T>& operator %=( const Matrix<TM>& M );

		public:
			// =============================================================
			// = Assignment with MatrixView
			// =============================================================
			MatrixView<T>& operator =( const MatrixView<T>& V );
			MatrixView<T>& operator +=( const MatrixView<T>& V );
			MatrixView<T>& operator -=( const MatrixView<T>& V );
			MatrixView<T>& operator *=( const MatrixView<T>& V );
			MatrixView<T>& operator /=( const MatrixView<T>& V );

			template<typename TMV> MatrixView<T>& operator =( const MatrixView<TMV>& MV );
			template<typename TMV> MatrixView<T>& operator +=( const MatrixView<TMV>& MV );
			template<typename TMV> MatrixView<T>& operator -=( const MatrixView<TMV>& MV );
			template<typename TMV> MatrixView<T>& operator *=( const MatrixView<TMV>& MV );
			template<typename TMV> MatrixView<T>& operator /=( const MatrixView<TMV>& MV );

		public:
			// =============================================================
			// = Assignment with Functions and Operations
			// =============================================================
//			template<class TMOPB> MatrixView<T>& operator =( const MatrixOperationBase<TMOPB>& mop );

	};

	// =====================================================================
	// = overload '<<' operator
	// =====================================================================
	template<typename T> std::ostream& operator<<( std::ostream& out, const MatrixView<T>& M ) {

		if (M.n_elements == 0) {
			out << "<empty>\n";
			return out;
		}
		for (int r = 0; r < M.nrows; ++r) {
			for (int c = 0; c < M.ncols; ++c) {
				out << M.data[(r * M.stride + c)] << " ";
			}
			out << std::endl;
		}
		return out;
	}

	// =====================================================================
	// = Basic Constructors
	// =====================================================================
	template<typename T> MatrixView<T>::MatrixView( const Matrix<T>& MIn, int r, int c, int nrows, int ncolumns ) :
			M( MIn ), //
			row_aux( r ), //
			column_aux( c ), //
			nrows( nrows ), //
			ncols( ncolumns ), //
			n_elements( nrows * ncolumns ), //
			stride( MIn.stride ), //
			data() {

		__extra_debug();

		util::mem::access::rwp( data ) = &util::mem::access::rwp( M.data )[r * stride + c];
	}
	template<typename T> MatrixView<T>::MatrixView( const MatrixView<T>& MIn, int r, int c, int nrows, int ncolumns ) :
			M( MIn.M ), //
			row_aux( MIn.row_aux + r ), //
			column_aux( MIn.column_aux + c ), //
			nrows( nrows ), //
			ncols( ncolumns ), //
			n_elements( nrows * ncolumns ), //
			stride( MIn.stride ), //
			data() {

		__extra_debug();

		util::mem::access::rwp( data ) = &util::mem::access::rwp( MIn.data )[r * stride + c];

	}

	// =====================================================================
	// = Copy constructors
	// =====================================================================
//		template<typename T> MatrixView<T>::MatrixView( const MatrixView<T>& Orig ) {
//
//		}

	// =====================================================================
	// = Destructor
	// =====================================================================
	template<typename T> MatrixView<T>::~MatrixView() {
		__extra_debug();
	}

	// =====================================================================
	// = Private Auxiliary Methods
	// =====================================================================
	template<typename T> T * MatrixView<T>::__mem_ptr() {
		return util::mem::access::rwp<T>( data );
	}

	// =====================================================================
	// = Utility Methods
	// =====================================================================
	template<typename T> template<typename Tval> bool MatrixView<T>::fill( Tval val ) {

		for (int r = 0; r < nrows; ++r) {
			for (int c = 0; c < ncols; ++c) {
				const_cast<T*>( data )[r * stride + c] = val;
			}
		}
		return true;

	}
	template<typename T> template<typename Tval> bool MatrixView<T>::fill_diagonal( Tval val ) {

		int mn = (nrows < ncols) ? nrows : ncols;
		for (int i = 0; i < mn; ++i) {
			util::mem::access::rwp( data )[i * stride + i] = val;

		}
		return true;
	}

	// =====================================================================
	// = Information Methods
	// =====================================================================
	template<typename T> void MatrixView<T>::print( const char * extra_text ) const {
		if (extra_text) std::cout << extra_text << std::endl;
		std::cout << *this;
	}

	// =====================================================================
	// = Sub Matrices
	// =====================================================================
	template<typename T> MatrixView<T> MatrixView<T>::row( int r ) const {
		MatrixView<T> M( *this, r, 0, 1, ncols );
		return M;
	}
	template<typename T> MatrixView<T> MatrixView<T>::rows( int sr, int er ) const {
		MatrixView<T> M( *this, sr, 0, er - sr + 1, ncols );
		return M;
	}
	template<typename T> MatrixView<T> MatrixView<T>::col( int c ) const {
		MatrixView<T> M( *this, 0, c, nrows, 1 );
		return M;
	}
	template<typename T> MatrixView<T> MatrixView<T>::cols( int sc, int ec ) const {
		MatrixView<T> M( *this, 0, sc, nrows, ec - sc + 1 );
		return M;
	}
	template<typename T> MatrixView<T> MatrixView<T>::submat( int sr, int er, int sc, int ec ) const {
		MatrixView<T> M( *this, sr, sc, er - sr + 1, ec - sc + 1 );
		return M;
	}

	// =====================================================================
	// = Data Access Methods
	// =====================================================================
	template<typename T> T& MatrixView<T>::operator()( int idx ) const {
		return util::mem::access::rw( data[idx] );
	}
	template<typename T> T& MatrixView<T>::operator()( int row, int col ) const {
		return util::mem::access::rw( data[row * stride + col] );
	}

	// =====================================================================
	// = Assignment with primitive types
	// =====================================================================
	template<typename T> MatrixView<T>& MatrixView<T>::operator =( double val ) {

		this->fill( val );
		return *this;

	}
	template<typename T> MatrixView<T>& MatrixView<T>::operator +=( double val ) {

		for (int r = 0; r < nrows; ++r) {
			for (int c = 0; c < ncols; ++c) {
				const_cast<T*>( data )[r * M.ncols + c] += val;
			}
		}
		return *this;
	}
	template<typename T> MatrixView<T>& MatrixView<T>::operator -=( double val ) {

		this->operator +=( -val );
		return *this;
	}
	template<typename T> MatrixView<T>& MatrixView<T>::operator *=( double val ) {

		for (int r = 0; r < nrows; ++r) {
			for (int c = 0; c < ncols; ++c) {
				const_cast<T*>( data )[r * stride + c] *= val;
			}
		}
		return *this;
	}
	template<typename T> MatrixView<T>& MatrixView<T>::operator /=( double val ) {

		this->operator *=( 1 / val );
		return *this;
	}

	// =====================================================================
	// = Assignment with Matrix
	// =====================================================================
	template<typename T> MatrixView<T>& MatrixView<T>::operator =( const Matrix<T>& M ) {
		impl::matops::copy( nrows, ncols, __mem_ptr(), stride, M.data, M.stride );
//		impl::matops::m_move( __mem_ptr(), Orig.data, n_elements );
		return *this;

	}
	template<typename T> template<typename TM> MatrixView<T>& MatrixView<T>::operator =( const Matrix<TM>& M ) {
		impl::matops::copy( nrows, ncols, M.data, M.stride, __mem_ptr(), stride );
		return *this;

	}

	// =====================================================================
	// = Assignment with MatrixView
	// =====================================================================
	template<typename T> template<typename TV> MatrixView<T>& MatrixView<T>::operator =( const MatrixView<TV>& V ) {
		for (int r = 0; r < nrows; ++r) {
			for (int c = 0; c < ncols; ++c) {

				util::mem::access::rw( this->data[r * stride + c] ) = V.data[r * V.stride + c];

			}
		}
		return *this;

	}
	template<typename T> template<typename TV> MatrixView<T>& MatrixView<T>::operator +=( const MatrixView<TV>& V ) {

		for (int r = 0; r < nrows; ++r) {
			for (int c = 0; c < ncols; ++c) {
				util::mem::access::rw( this->data[r * stride + c] ) += V.data[r * V.stride + c];
			}
		}
		return *this;
	}
	template<typename T> template<typename TV> MatrixView<T>& MatrixView<T>::operator -=( const MatrixView<TV>& V ) {

		for (int r = 0; r < nrows; ++r) {
			for (int c = 0; c < ncols; ++c) {
				util::mem::access::rw( this->data[r * stride + c] ) -= V.data[r * V.stride + c];
			}
		}
		return *this;
	}
	template<typename T> template<typename TV> MatrixView<T>& MatrixView<T>::operator *=( const MatrixView<TV>& V ) {

		for (int r = 0; r < nrows; ++r) {
			for (int c = 0; c < ncols; ++c) {
				util::mem::access::rw( this->data[r * stride + c] ) *= V.data[r * V.stride + c];
			}
		}
		return *this;
	}
	template<typename T> template<typename TV> MatrixView<T>& MatrixView<T>::operator /=( const MatrixView<TV>& V ) {

		for (int r = 0; r < nrows; ++r) {
			for (int c = 0; c < ncols; ++c) {
				util::mem::access::rw( this->data[r * stride + c] ) /= V.data[r * V.stride + c];
			}
		}
		return *this;
	}

	// =====================================================================
	// = Assignment with Functions and Operations
	// =====================================================================
//		template<typename T> template<class MOPB> MatrixView<T>& MatrixView<T>::operator =( const MatrixOperationBase<MOPB>& mop ) {
//			mop.get_derived().apply( *this );
//			return *this;
//		}
//	template<typename T> template<class MO> MatrixView<T>& MatrixView<T>::operator =( const MatrixOperationBase<MO>& mo ) {
//		mo.get_derived().apply( *this );
//		return *this;
//	}

}

#endif /* MATRIXVIEW_HPP_ */
