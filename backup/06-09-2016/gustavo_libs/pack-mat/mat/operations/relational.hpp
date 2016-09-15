/*
 * relational.hpp
 *
 *  Created on: Jul 9, 2013
 *      Author: gustavo
 */

#ifndef RELATIONAL_HPP_
#define RELATIONAL_HPP_

#include "MatrixOperationBase.hpp"
#include "../msg/common_msg.hpp"
#include <impl/mat/m_blaop.hpp>

namespace mat {

	template<typename T> class Matrix;
	template<typename T> class MatrixView;

	template<typename TL, typename TR> class MatRelationalOp: public MatrixOperationBase<MatRelationalOp<TL, TR> > {

			enum relational_operator {
				EQUAL_TO, NOT_EQUAL_TO, GREATER_THAN, LESS_THAN, GREATER_THAN_OR_EQUAL_TO, LESS_THAN_OR_EQUAL_TO
			};

			const Matrix<TL>& ML;
			const Matrix<TR>& MR;
			enum relational_operator op;
		public:
			MatRelationalOp(const Matrix<TL>& ML, const Matrix<TR>& MR, enum relational_operator op) :
					ML(ML), MR(MR), op(op) {

				if (ML.size() != 1 && MR.size() != 1) {
					__assert_msg( (ML.n_rows == MR.n_rows) && (ML.n_columns == MR.n_columns), INVALID_MATRIX_DIMENSION);
				}

			}

			template<class Tout> void apply(Matrix<Tout>& A) const {

				if (ML.size() > MR.size()) {
					A.set_size(ML.n_rows, ML.n_columns);
				} else {
					A.set_size(MR.n_rows, MR.n_columns);
				}

				switch (op) {
				case EQUAL_TO:
					break;

				}
			}
	};

}

#endif /* RELATIONAL_HPP_ */
