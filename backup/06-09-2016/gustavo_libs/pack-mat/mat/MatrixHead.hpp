/*
 * MatrixHead.hpp
 *
 *  Created on: May 3, 2013
 *      Author: gustavo
 */

#ifndef MATRIXHEAD_HPP_
#define MATRIXHEAD_HPP_

namespace mat {

	class MatrixHead {
		public:

			static long id;

		public:
			// =============================================================
			// = Ready Only Attributes
			// =============================================================
			const int n_rows;
			const int n_columns;
			const int stride;

			MatrixHead() {
				++id;
			}

			long n_elements() const {
				return n_rows * n_columns;
			}

	};

	long MatrixHead::id = 0;

}

#endif /* MATRIXHEAD_HPP_ */
