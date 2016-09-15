/*
 * length.hpp
 *
 *  Created on: Oct 15, 2012
 *      Author: gustavo
 */

#ifndef IMP_VECNORM_HPP
#define IMP_VECNORM_HPP

#include "../Matrix.hpp"
#include "MatrixFunctionBase.hpp"

#include <impl/math/mstat.hpp>

namespace mat {

	namespace MatrixFunctionType {
		struct VectorLength;
	}

	template<typename T> class VectorLength: public MatrixFunctionBase<VectorLength<T> > {
		private:
			Matrix<T> M;
			int dim;
		public:
			VectorLength(const Matrix<T>& m, int dim = 1) {
				this->M.refTo(m);
				this->dim = dim;
			}

			bool apply(Matrix<T>& MM) const {
				if (dim == 1) {
					MM.set_size(1, M.ncols);
					impl::math::m_column_length(M.nrows, M.ncols, M.data, M.stride, MM.data, 1);
				} else if (dim == 2) {
					MM.set_size(M.nrows, 1);
					impl::math::m_row_length(M.nrows, M.ncols, M.data, M.stride, MM.data, MM.stride);
				}
				return true;
			}

			int rows() const {
				return M.nrows;
			}
			int columns() const {
				return M.ncols;
			}
	};

	template<typename T> VectorLength<T> veclength(Matrix<T>& M, int dim = 1) {
		return VectorLength<T>(M, dim);
	}

//	template<typename T> void veclength(Matrix<T>& M, Matrix<T>& Mod, int dim = 1) {
//		Mod = veclength(M, dim);
//	}

}

#endif /* IMP_VECNORM_HPP */
