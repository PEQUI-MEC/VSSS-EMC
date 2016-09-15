/*
 * Matrix.hpp
 *
 *  Created on: Mar 20, 2013
 *      Author: gustavo
 */

#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include <iomanip>	// for std::setprecision()
#include <sstream>	// for std::stringstream
#include <typeinfo>	// for typeid
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

/* -------------------------------------------------------------------------- */
//#include <impl/util/msg.h>
#include "impl/basic_operations.hpp"
#include "impl/array/arrayops.hpp"
#include "impl/math/math_macros.h"

/* -------------------------------------------------------------------------- */
#include "impl/util/mem.hpp"
using namespace impl;

/* -------------------------------------------------------------------------- */
#include "internal/config.hpp"
#include "msg/common_msg.hpp"
#include "internal/debug.h"
#include "internal/MatRef.hpp"

/* -------------------------------------------------------------------------- */
#include "MatrixBase.hpp"
#include "MatrixView.hpp"
#include "Range.hpp"
#include "Size.hpp"

/* -------------------------------------------------------------------------- */
#include "functions/MatrixFunctionBase.hpp"
#include "operations/MatrixOperationBase.hpp"

#include "functions/transpose.hpp"

namespace mat {

	template<typename T> class MatrixIndexing;

	template<typename T> class Matrix: public MatrixBase<Matrix<T> >, protected MatRef {
		public:
			typedef T ElementType;

		public:

			void refTo(const Matrix<T>& M) {
				__release_memory();
				util::mem::access::rwp(data) = M.data;
				util::mem::access::rw(nrows) = M.nrows;
				util::mem::access::rw(ncols) = M.ncols;
				util::mem::access::rw(size) = M.size;
				util::mem::access::rw(stride) = M.stride;
				makeReferenceTo(M);
			}

		public:
			// =============================================================
			// = Ready Only Attributes
			// =============================================================
			const int nrows;
			const int ncols;
			const int size;
			const int stride;
			T * const data;
			const char * const name;

			bool init_by_set_data;

		public:
			// =============================================================
			// = Basic Constructors
			// =============================================================
			Matrix();
			Matrix(int rows, int cols);
			Matrix(const Size<2>& s);

		public:
			// =============================================================
			// = Copy constructors
			// =============================================================
			Matrix(const Matrix<T>& M);
			template<typename T2> explicit Matrix(const Matrix<T2>& M);
			template<typename T2> explicit Matrix(const MatrixView<T2>& V);
			template<typename T2> explicit Matrix(const Range<T2>& range);

		public:
			// =============================================================
			// = Destructor
			// =============================================================
			~Matrix();

		private:
			// =============================================================
			// = Private Auxiliary Methods
			// =============================================================
			void __init(int rows, int cols);
			void __release_memory();
			void __clear_attributes();
			T * __mem_ptr();

		public:
			// =============================================================
			// = Construction|Configuration Methods
			// =============================================================
			void release();
			void set_size(int rows, int columns);
			void set_size(const Size<2>& s);
			void set_data(int rows, int columns, T * data);
			Matrix<T>& hack(Matrix<T>& Orig);

		public:
			// =============================================================
			// = Utility Methods
			// =============================================================
			static Matrix<T>& null();
			inline int idx(int r, int c) const;
			T * get_pointer(int r, int c);

			inline bool is_constant() const;
			inline bool is_vector() const;
			inline bool is_matrix() const;

			void normalize(T min, T max);
			template<typename Tval> bool fill(Tval val);
			template<typename Tval> bool fill_diagonal(Tval val);

			MatrixTranspose<T> t() const;

		public:
			// =============================================================
			// = Information Methods
			// =============================================================
			void print(const char * extra_text = 0, bool show_dim = true) const;
			void info() const;

		public:
			// =============================================================
			// = I/O Methods
			// =============================================================
			bool read_by_user() {

				if (this->size > 10) {
					char op = 'y';
					std::cout << "This matrix is very large. Do you want to continue?" << std::endl;
					do {
						std::cout << "[y|n]: ";
						std::cin >> op;
					} while (op != 'y' && op != 'n');
					if (op == 'n') return true;
				}

				for (int r = 0; r < nrows; ++r) {
					for (int c = 0; c < ncols; ++c) {
						std::cout << "(" << r << "," << c << "): ";
						std::cin >> data[idx(r, c)];
					}
				}

				return true;
			}
			bool save(const char * fname) const;
			bool load(const char * fname);

		public:
			// =============================================================
			// = Sub Matrices
			// =============================================================
			MatrixView<T> row(int r) const;
			MatrixView<T> rows(int s, int e) const;
			MatrixView<T> col(int r) const;
			MatrixView<T> cols(int s, int e) const;
			MatrixView<T> submat(int sr, int er, int sc, int ec) const;

		public:
			// =============================================================
			// = Data Access Methods
			// =============================================================
			inline T& operator()(int idx) const;
			inline T& operator()(int row, int col) const;

			// =============================================================
			// = Indexing Methods
			// =============================================================
			MatrixIndexing<T> operator()(const Matrix<int>& Idx) const;
			MatrixIndexing<T> operator()(const Matrix<int>& Rows, const Matrix<int>& Cols) const;
			MatrixIndexing<T> operator()(const Matrix<int>& Rows, int c) const;
			MatrixIndexing<T> operator()(int r, const Matrix<int>& Cols) const;
			MatrixIndexing<T> rows(const Matrix<int>& Cols) const;
			MatrixIndexing<T> columns(const Matrix<int>& Rows) const;

		public:
			// =============================================================
			// = Assignment with Range
			// =============================================================
			template<typename T2> Matrix<T>& operator =(const Range<T2> range);

		public:
			// ============================================================
			// = Assignment with primitive types
			// =============================================================
			Matrix<T>& operator =(double val);
			Matrix<T>& operator +=(double val);
			Matrix<T>& operator -=(double val);
			Matrix<T>& operator *=(double val);
			Matrix<T>& operator /=(double val);
			Matrix<T>& operator %=(double val);

		public:
			// =============================================================
			// = Assignment with Matrix
			// =============================================================
			Matrix<T>& operator =(const Matrix<T>& M);
			template<typename T2> Matrix<T>& operator =(const Matrix<T2>& M);
			template<typename T2> Matrix<T>& operator +=(const Matrix<T2>& M);
			template<typename T2> Matrix<T>& operator -=(const Matrix<T2>& M);
			template<typename T2> Matrix<T>& operator *=(const Matrix<T2>& M);
			template<typename T2> Matrix<T>& operator /=(const Matrix<T2>& M);
			template<typename T2> Matrix<T>& operator %=(const Matrix<T2>& M);

		public:
			// =============================================================
			// = Assignment with MatrixView
			// =============================================================
			template<typename T2> Matrix<T>& operator =(const MatrixView<T2>& V);
			template<typename T2> Matrix<T>& operator +=(const MatrixView<T2>& V);
			template<typename T2> Matrix<T>& operator -=(const MatrixView<T2>& V);
			template<typename T2> Matrix<T>& operator *=(const MatrixView<T2>& V);
			template<typename T2> Matrix<T>& operator /=(const MatrixView<T2>& V);
			template<typename T2> Matrix<T>& operator %=(const MatrixView<T2>& V);

		public:
			// =============================================================
			// = Assignment with Functions and Operations
			// =============================================================
			template<class MF> Matrix<T>& operator =(const MatrixFunctionBase<MF>& FB);
			template<class MO> Matrix<T>& operator =(const MatrixOperationBase<MO>& FB);

//				template<typename T2> inline void operator =( const MatrixOpResult<T2>& OpR ) {
//					set_data( OpR.nrows, OpR.ncols, OpR.data );
//				}
	};

	// =====================================================================
	// = overload '<<' operator
	// =====================================================================
	template<typename T> std::ostream& operator<<(std::ostream& out, const Matrix<T>& M) {

		if (M.size == 0) {
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
	template<typename T> Matrix<T>::Matrix() :
			nrows(),  //
			ncols(),  //
			size(),  //
			stride(),  //
			data(),  //
			name(),  //
			init_by_set_data(false) {

		__extra_debug_info(*this);
	}
	template<typename T> Matrix<T>::Matrix(int rows, int columns) :
			nrows(),  //
			ncols(),  //
			size(),  //
			stride(),  //
			data(),  //
			name(),  //
			init_by_set_data(false) {

		__extra_debug_info(*this);

		__init(rows, columns);

	}
	template<typename T> Matrix<T>::Matrix(const Size<2>& s) :
			nrows(),  //
			ncols(),  //
			size(),  //
			stride(),  //
			data(),  //
			name(),  //
			init_by_set_data(false) {

		__extra_debug_info(*this);

		__init(s[0], s[1]);

	}

	// =====================================================================
	// = Copy constructors
	// =====================================================================
	template<typename T> Matrix<T>::Matrix(const Matrix<T>& Orig) :
			nrows(),  //
			ncols(),  //
			size(),  //
			stride(),  //
			data(),  //
			name(),  //
			init_by_set_data(false) {

		__extra_debug_info(*this);
		cout << "Calling Copy constructor\n";

		__init(Orig.nrows, Orig.ncols);
		impl::arrayops::copy(size, __mem_ptr(), Orig.data);
	}
	template<typename T> template<typename T2> Matrix<T>::Matrix(const Matrix<T2>& Orig) :
			nrows(),  //
			ncols(),  //
			size(),  //
			stride(),  //
			data(),  //
			name(),  //
			init_by_set_data(false) {

		__extra_debug_info(*this);

		__init(Orig.nrows, Orig.ncols);
		impl::arrayops::copy(__mem_ptr(), Orig.data, size);
	}
	template<typename T> template<typename T2> Matrix<T>::Matrix(const MatrixView<T2>& Orig) :
			nrows(),  //
			ncols(),  //
			size(),  //
			stride(),  //
			data(),  //
			name(),  //
			init_by_set_data(false) {

		__extra_debug_info(*this);

		__init(Orig.nrows, Orig.ncols);
		impl::arrayops::copy(size, __mem_ptr(), Orig.data);
	}
	template<typename T> template<typename Tr> Matrix<T>::Matrix(const Range<Tr>& range) :
			nrows(),  //
			ncols(),  //
			size(),  //
			stride(),  //
			data(),  //
			name(),  //
			init_by_set_data(false) {

		__extra_debug_info(*this);
	}

	// =====================================================================
	// = Destructor
	// =====================================================================
	template<typename T> Matrix<T>::~Matrix() {

		__extra_debug_info(*this);
		__release_memory();

	}

	// =====================================================================
	// = Private Auxiliary Methods
	// =====================================================================
	template<typename T> void Matrix<T>::__init(int rows, int cols) {

		__assert_msg(rows > 0 && cols > 0, INVALID_MATRIX_DIMENSION);

		util::mem::access::rw(nrows) = rows;
		util::mem::access::rw(ncols) = cols;
		util::mem::access::rw(size) = rows * cols;
		util::mem::access::rw(stride) = cols;
		util::mem::access::rwp(data) = new T[size];
		util::mem::access::rw(init_by_set_data) = false;
	}
	template<typename T> void Matrix<T>::__release_memory() {

		if (init_by_set_data) return;

		if (!unRef()) {
			delete[] data;
		}
		__clear_attributes();

	}
	template<typename T> void Matrix<T>::__clear_attributes() {

		util::mem::access::rw(nrows) = 0;
		util::mem::access::rw(ncols) = 0;
		util::mem::access::rw(size) = 0;
		util::mem::access::rw(stride) = 0;
		util::mem::access::rwp(data) = 0;
		util::mem::access::rw(init_by_set_data) = false;

	}
	template<typename T> inline T * Matrix<T>::__mem_ptr() {
		return util::mem::access::rwp<T>(data);
	}

	// =====================================================================
	// = Construction|Configuration Methods
	// =====================================================================

	template<typename T> void Matrix<T>::release() {
		__release_memory();
	}
	template<typename T> void Matrix<T>::set_size(int rows, int columns) {

		if (nrows == rows && ncols == columns) return;
		__release_memory();
		__init(rows, columns);

	}
	template<typename T> void Matrix<T>::set_size(const Size<2>& s) {

		if (nrows == s[0] && ncols == s[1]) return;

		__release_memory();
		__init(s[0], s[1]);
	}
	template<typename T> void Matrix<T>::set_data(int rows, int cols, T * data_ptr) {

		__assert_msg(rows > 0 && cols > 0, INVALID_MATRIX_DIMENSION);

		__release_memory();

		util::mem::access::rw(nrows) = rows;
		util::mem::access::rw(ncols) = cols;
		util::mem::access::rw(size) = rows * cols;
		util::mem::access::rw(stride) = cols;
		util::mem::access::rwp(data) = data_ptr;

		util::mem::access::rw(init_by_set_data) = true;
	}
	template<typename T> Matrix<T>& Matrix<T>::hack(Matrix<T>& Orig) {

		__release_memory();
		util::mem::access::rw(nrows) = Orig.nrows;
		util::mem::access::rw(ncols) = Orig.ncols;
		util::mem::access::rw(size) = Orig.size;
		util::mem::access::rw(stride) = Orig.stride;
		util::mem::access::rwp(data) = Orig.data;
		util::mem::access::rw(init_by_set_data) = Orig.init_by_set_data;

		util::mem::access::rw(Orig.nrows) = 0;
		util::mem::access::rw(Orig.ncols) = 0;
		util::mem::access::rw(Orig.size) = 0;
		util::mem::access::rw(Orig.stride) = 0;
		util::mem::access::rwp(Orig.data) = 0;
		util::mem::access::rw(Orig.init_by_set_data) = false;

		return *this;
	}

	// =====================================================================
	// = Utility Methods
	// =====================================================================
	template<typename T> Matrix<T>& Matrix<T>::null() {
		static Matrix<T> __mat_null;
		return __mat_null;
	}
	template<typename T> int Matrix<T>::idx(int r, int c) const {
		return r * stride + c;
	}
	template<typename T> T * Matrix<T>::get_pointer(int r, int c) {
		return &data[r * stride + c];
	}
	template<typename T> bool Matrix<T>::is_constant() const {
		return nrows == 1 && ncols == 1;
	}
	template<typename T> bool Matrix<T>::is_vector() const {
		return (nrows == 1 && ncols > 1) || (nrows > 1 && ncols == 1);
	}
	template<typename T> bool Matrix<T>::is_matrix() const {
		return nrows != 1 || ncols != 1;
	}

	template<typename T> void Matrix<T>::normalize(T min, T max) {
		impl::matops::normalize(nrows, ncols, data, stride, min, max);
	}
	template<typename T> template<typename Tval> bool Matrix<T>::fill(Tval val) {

		impl::arrayops::fill(size, __mem_ptr(), val);

		return true;
	}
	template<typename T> template<typename Tval> bool Matrix<T>::fill_diagonal(Tval val) {

		impl::matops::fill_diagonal(__min(nrows, ncols), __mem_ptr(), stride, val);
		return true;
	}

	template<typename T> MatrixTranspose<T> Matrix<T>::t() const {
		MatrixTranspose<T> op(*this);
		return op;
	}

	// =====================================================================
	// = Information Methods
	// =====================================================================
	template<typename T> void Matrix<T>::info() const {
		cout << " Matrix<" << typeid(T).name() << ">(" << nrows << "," << ncols << "), mem_ptr: " << (void*) data << ", stride: " << stride << endl;
	}
	template<typename T> void Matrix<T>::print(const char * extra_text, bool show_dim) const {
		if (extra_text) std::cout << extra_text;
		if (show_dim) std::cout << "[" << nrows << "," << ncols << "]" << endl;
		std::cout << *this;
		std::cout << std::endl;
	}

	// =====================================================================
	// = I/O Methods
	// =====================================================================
	template<typename T> bool Matrix<T>::save(const char * fname) const {

		if (size < 1) return false;

		ofstream file;
		file.open(fname, ios::out | ios::binary);
		if (!file.is_open()) return false;

		file << nrows << " " << ncols << "\n";

		for (int r = 0; r < nrows; ++r) {
			for (int c = 0; c < ncols; ++c) {

				std::stringstream ss;
				//ss << std::setprecision( std::numeric_limits<T>::max() );
				ss << std::setprecision(35);
				ss << data[r * stride + c];
				file << ss.str() << " ";

			}
			file << "\n";
		}

		file.close();
		return true;
	}
	template<typename T> bool Matrix<T>::load(const char * fname) {

		ifstream file;
		file.open(fname, ios::out | ios::binary);
		if (!file.is_open()) return false;

		int rows, cols;

		string str;
		std::getline(file, str);

		string token;
		unsigned pos;

		pos = str.find(" ");
		token = str.substr(0, pos);
		rows = atoi(token.c_str());

		token = str.substr(pos + 1, str.size() - 1);
		cols = atoi(token.c_str());

		if (rows < 1 || cols < 1) {
			cout << "File format error. Invalid matrix dimension!\n";
			file.close();
			return false;
		}

		this->set_size(rows, cols);
		for (int r = 0; r < rows; ++r) {

			std::getline(file, str);
			if (!file.good() || str.size() < 1) {

				cout << "Unexpected end of file!\n";

				file.close();
				this->__release_memory();
				return false;
			}

			int p1 = 0, p2 = 0;

			for (int c = 0; c < cols - 1; ++c) {
				p2 = str.find(" ", p1);

				if (p2 < 0) {

					cout << "File format error. There is a lack of columns!\n";

					file.close();
					this->__release_memory();
					return false;
				}
				token = str.substr(p1, p2 - p1);

				data[r * stride + c] = strtod(token.c_str(), 0);

				p2++;
				p1 = p2;
			}

			if ((int) (str.size()) == p2) {

				cout << "File format error. There is a lack of columns!\n";

				file.close();
				this->__release_memory();
				return false;
			}

			token = str.substr(p1, str.size() - p2);
			data[r * stride + cols - 1] = atof(token.c_str());

		}

		file.close();

		return true;
	}

	// =====================================================================
	// = Sub Matrices
	// =====================================================================
	template<typename T> MatrixView<T> Matrix<T>::row(int r) const {
		MatrixView<T> M(*this, r, 0, 1, ncols);
		return M;
	}
	template<typename T> MatrixView<T> Matrix<T>::rows(int sr, int er) const {
		MatrixView<T> M(*this, sr, 0, er - sr + 1, ncols);
		return M;
	}
	template<typename T> MatrixView<T> Matrix<T>::col(int c) const {
		MatrixView<T> M(*this, 0, c, nrows, 1);
		return M;
	}
	template<typename T> MatrixView<T> Matrix<T>::cols(int sc, int ec) const {
		MatrixView<T> M(*this, 0, sc, nrows, ec - sc + 1);
		return M;
	}
	template<typename T> MatrixView<T> Matrix<T>::submat(int sr, int er, int sc, int ec) const {
		MatrixView<T> M(*this, sr, sc, er - sr + 1, ec - sc + 1);
		return M;
	}

	// =====================================================================
	// = Data Access Methods
	// =====================================================================
	template<typename T> T& Matrix<T>::operator()(int idx) const {
		return util::mem::access::rw(data[idx]);
	}
	template<typename T> T& Matrix<T>::operator()(int row, int col) const {
		return util::mem::access::rw(data[row * stride + col]);
	}

	// =====================================================================
	// = Indexing Methods
	// =====================================================================
	template<typename Tin> class MatrixIndexing: public MatrixFunctionBase<MatrixIndexing<Tin> > {
		private:
			const Matrix<Tin>& M;
			Matrix<int> R;
			Matrix<int> C;

			template<typename Tout> bool apply_by_idx(Matrix<Tout>& M_out) const {

				M_out.set_size(R.nrows, R.ncols);
				for (int r = 0; r < M_out.nrows; ++r) {
					for (int c = 0; c < M_out.ncols; ++c) {

						int idx = R.data[r * R.stride + c];

						__assert_msg(idx < M.size, INDEX_EXCEEDS_MATRIX_DIMENSIONS);
						M_out.data[r * M_out.stride + c] = M.data[idx];

					}
				}
				return true;
			}
			template<typename Tout> bool apply_by_coords(Matrix<Tout>& M_out) const {

				M_out.set_size(R.size, C.size);
				for (int r = 0; r < M_out.nrows; ++r) {
					for (int c = 0; c < M_out.ncols; ++c) {

						__assert_msg(R.data[r] < M.nrows, INDEX_EXCEEDS_MATRIX_DIMENSIONS);
						__assert_msg(C.data[c] < M.ncols, INDEX_EXCEEDS_MATRIX_DIMENSIONS);
						int idx = R.data[r] * M.stride + C.data[c];
						M_out.data[r * M_out.stride + c] = M.data[idx];
					}
				}
				return true;
			}

		public:
			MatrixIndexing(const Matrix<Tin>& M_in, const Matrix<int>& MIdx) :
					M(M_in) {
				R = MIdx;
			}
			MatrixIndexing(const Matrix<Tin>& M_in, const Matrix<int>& Row, const Matrix<int>& Col) :
					M(M_in) {
				R = Row;
				C = Col;
			}
			template<typename Tout> bool apply(Matrix<Tout>& M_out) const {
				if (C.size == 0) return apply_by_idx(M_out);
				return apply_by_coords(M_out);
			}
			void operator =(double val) {
				if (C.size == 0) {
					for (int i = 0; i < R.size; ++i) {
						M(R(i)) = val;
					}
				} else {
					for (int i = 0; i < R.size; ++i) {
						M(R(i), C(i)) = val;
					}
				}

			}
			template<typename Tm> void operator =(Matrix<Tm> Mv) {

				__assert_msg(Mv.size == R.size, INVALID_MATRIX_DIMENSION);

				if (C.size == 0) {
					for (int i = 0; i < R.size; ++i) {
						M(R(i)) = Mv(i);
					}
				} else {
					for (int i = 0; i < R.size; ++i) {
						M(R(i), C(i)) = Mv(i);
					}
				}

			}

	};

	template<typename T> MatrixIndexing<T> Matrix<T>::operator()(const Matrix<int>& Idx) const {
		MatrixIndexing<T> mf(*this, Idx);
		return mf;
	}
	template<typename T> MatrixIndexing<T> Matrix<T>::operator()(const Matrix<int>& R, const Matrix<int>& C) const {
		MatrixIndexing<T> mf(*this, R, C);
		return mf;
	}
	template<typename T> MatrixIndexing<T> Matrix<T>::operator()(const Matrix<int>& R, int c) const {
		Matrix<int> C(1, 1);
		C(0) = c;
		MatrixIndexing<T> mf(*this, R, C);
		return mf;
	}
	template<typename T> MatrixIndexing<T> Matrix<T>::operator()(int r, const Matrix<int>& C) const {
		Matrix<int> R(1, 1);
		R(0) = r;
		MatrixIndexing<T> mf(*this, R, C);
		return mf;
	}
	template<typename T> MatrixIndexing<T> Matrix<T>::rows(const Matrix<int>& R) const {

		Matrix<int> C(1, this->ncols);
		for (int i = 0; i < this->ncols; ++i) {
			C.data[i] = i;
		}

		MatrixIndexing<T> mf(*this, R, C);
		return mf;
	}
	template<typename T> MatrixIndexing<T> Matrix<T>::columns(const Matrix<int>& C) const {

		Matrix<int> R(1, this->nrows);
		for (int i = 0; i < this->nrows; ++i) {
			R.data[i] = i;
		}

		MatrixIndexing<T> mf(*this, R, C);
		return mf;
	}

	// =====================================================================
	// = Assignment with Range
	// =====================================================================
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator =(const Range<T2> range) {
	}

	// =====================================================================
	// = Assignment with primitive types
	// =====================================================================
	template<typename T> Matrix<T>& Matrix<T>::operator =(double val) {
		__release_memory();
		__init(1, 1);
		data[0] = (T) val;
		return *this;
	}
	template<typename T> Matrix<T>& Matrix<T>::operator +=(double val) {
		impl::arrayops::inplace_add(size, __mem_ptr(), val);
		return *this;
	}
	template<typename T> Matrix<T>& Matrix<T>::operator -=(double val) {
		impl::arrayops::inplace_sub(size, __mem_ptr(), val);
		return *this;
	}
	template<typename T> Matrix<T>& Matrix<T>::operator *=(double val) {
		impl::arrayops::inplace_mult(size, __mem_ptr(), val);
		return *this;
	}
	template<typename T> Matrix<T>& Matrix<T>::operator /=(double val) {
		impl::arrayops::inplace_div(size, __mem_ptr(), val);
		return *this;
	}
	template<typename T> Matrix<T>& Matrix<T>::operator %=(double val) {
		impl::arrayops::inplace_mod(size, __mem_ptr(), val);
		return *this;
	}

	// =====================================================================
	// = Assignment with Matrix
	// =====================================================================
	template<typename T> Matrix<T>& Matrix<T>::operator =(const Matrix<T>& rhs) {

		if (size != rhs.size) {
			__release_memory();
			__init(rhs.nrows, rhs.ncols);
		}
		impl::arrayops::copy(size, __mem_ptr(), rhs.data);

		return *this;
	}
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator =(const Matrix<T2>& rhs) {

		__release_memory();
		__init(rhs.nrows, rhs.ncols);
		impl::arrayops::copy(size, __mem_ptr(), rhs.data);

		return *this;
	}
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator +=(const Matrix<T2>& rhs) {

		__assert_msg(size == rhs.size, ARRAY_DIFFERENT_SIZES);

		impl::arrayops::inplace_add(__mem_ptr(), rhs.data, size);
		return *this;
	}
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator -=(const Matrix<T2>& rhs) {

		__assert_msg(size == rhs.size, ARRAY_DIFFERENT_SIZES);

		impl::arrayops::inplace_sub(__mem_ptr(), rhs.data, size);
		return *this;
	}
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator *=(const Matrix<T2>& rhs) {

		__assert_msg(size == rhs.size, ARRAY_DIFFERENT_SIZES);

		impl::arrayops::inplace_mult(__mem_ptr(), rhs.data, size);
		return *this;
	}
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator /=(const Matrix<T2>& rhs) {

		__assert_msg(size == rhs.size, ARRAY_DIFFERENT_SIZES);

		impl::arrayops::inplace_div(__mem_ptr(), rhs.data, size);
		return *this;
	}

	// =====================================================================
	// = Assignment with MatrixView
	// =====================================================================
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator =(const MatrixView<T2>& rhs) {

		__release_memory();
		__init(rhs.nrows, rhs.ncols);

		impl::arrayops::copy(size, __mem_ptr(), rhs.data);

		return *this;
	}
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator +=(const MatrixView<T2>& rhs) {

		__assert_msg(size == rhs.size, ARRAY_DIFFERENT_SIZES);

		impl::arrayops::inplace_add(__mem_ptr(), rhs.data, size);
		return *this;
	}
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator -=(const MatrixView<T2>& rhs) {

		__assert_msg(size == rhs.size, ARRAY_DIFFERENT_SIZES);

		impl::arrayops::inplace_sub(__mem_ptr(), rhs.data, size);
		return *this;
	}
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator *=(const MatrixView<T2>& rhs) {

//			__assert_msg( size == rhs.size, ARRAY_DIFFERENT_SIZES );
//
//			impl::arrayops::inplace_mult( __data_ptr(), rhs.data, size );
		return *this;
	}
	template<typename T> template<typename T2> Matrix<T>& Matrix<T>::operator /=(const MatrixView<T2>& rhs) {

		__assert_msg(size == rhs.size, ARRAY_DIFFERENT_SIZES);

		impl::arrayops::inplace_div(__mem_ptr(), rhs.data, size);
		return *this;
	}

	// =====================================================================
	// = Assignment with Functions and Operations
	// =====================================================================
	template<typename T> template<class MF> Matrix<T>& Matrix<T>::operator =(const MatrixFunctionBase<MF>& mf) {
		mf.get_derived().apply(*this);
		return *this;
	}
	template<typename T> template<class MO> Matrix<T>& Matrix<T>::operator =(const MatrixOperationBase<MO>& mo) {
		mo.get_derived().apply(*this);
		return *this;
	}
}

#endif /* MATRIX_HPP_ */
