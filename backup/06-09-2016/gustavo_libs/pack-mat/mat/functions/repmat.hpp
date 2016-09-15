/*
 * repmat.hpp
 *
 *  Created on: Oct 15, 2012
 *      Author: gustavo
 */

#ifndef IMP_REPMAT_HPP
#define IMP_REPMAT_HPP

#include "../Matrix.hpp"

#include <impl/math/mbop.hpp>

namespace mat {

	template<typename T> class RepMat: public MatrixFunctionBase<RepMat<T> > {
		private:
			Matrix<T> M;
			int rep_rows;
			int rep_columns;

		public:
			RepMat(const Matrix<T>& m, int rows, int columns) {
				this->M.refTo(m);
				this->rep_rows = rows;
				this->rep_columns = columns;
			}

			bool apply(Matrix<T>& MM) const {

				MM.set_size(M.nrows * rep_rows, M.ncols * rep_columns);
				impl::math::m_repmat(M.nrows, M.ncols, M.data, M.stride, rep_rows, rep_columns, MM.data, MM.stride);
				return true;
			}
	};

	template<typename T> RepMat<T> repmat(Matrix<T>& M, int rows, int columns) {
		return RepMat<T>(M, rows, columns);
	}

}

#endif /* IMP_REPMAT_HPP */
