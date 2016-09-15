/*
 * label.hpp
 *
 *  Created on: Sep 22, 2012
 *      Author: gustavo
 */

#ifndef IMPL_LABEL_HPP
#define IMPL_LABEL_HPP

#include <math.h>

#include "../util/mem.h"
#include "../math/math_macros.h"
#include "../math/mbop.hpp"

/* = Auxiliary Functions to label_he_chao_suzuki_wu ================= */
inline int * __create_table( int size ) {
	int i;
	int * table = mem_allocn(size, int);
	for (i = 0; i < size; i++)
		table[i] = -1;
	return table;
}

inline static int __label_find( int x, int * Parent ) {
	while (Parent[x] != -1)
		x = Parent[x];
	return x;

}

inline static void __label_union( int x1, int x2, int * Parent ) {
	x1 = __label_find( x1, Parent );
	x2 = __label_find( x2, Parent );
	if (x1 != x2) Parent[x2] = x1;
}
/* ================================================================== */

namespace impl {
	namespace img {

		/***************************************************************************
		 * This function is implemented based on work:
		 *
		 *	Lifeng He, Yuyan Chao, Kenji Suzuki, and Kesheng Wu. 2009.
		 *	Fast connected-component labeling. Pattern Recogn. 42, 9
		 *	(September 2009), 1977-1987. DOI=10.1016/j.patcog.2008.10.013
		 *	http://dx.doi.org/10.1016/j.patcog.2008.10.013
		 *
		 * @param rows
		 * @param columns
		 * @param mat
		 * @param label
		 * @param count
		 * @param bv
		 * @return
		 */
		template<typename T, typename Tlabel> int label_he_chao_suzuki_wu( int rows, int columns, T * mat, int st_mat, Tlabel * label, int st_label, T obj_value ) {

			int msize;
			int max_comp;
			int nclass;

			int r, c, _r, i, _rl, il;
			int * Table;

			msize = rows * columns;

			max_comp = imp__ceil(rows / 2.0) * imp__ceil(columns / 2.0);

			impl::math::m_set( rows, columns, label, st_label, 0 );

			Table = __create_table( max_comp );

			{/* Step 01: Forward raster scan ======================================= */
				T left;
				T top_left;
				T top;
				T top_right;
				T c1, c2, c3, c4;
				T LABEL = 1;

				nclass = 0;

				for (r = 1, _r = r * st_mat, _rl = r * st_label; r < rows - 1; r++, _r += st_mat, _rl += st_label) {
					for (c = 1; c < columns - 1; c++) {
						i = _r + c;
						il = _rl + c;

						if (mat[i] != obj_value) continue;

						/* = mat[i] is an object ==================================== */

						left = label[il - 1];
						top = label[il - columns];
						top_left = label[il - columns - 1];
						top_right = label[il - columns + 1];

						c1 = left;
						c2 = top_left;
						c3 = top;
						c4 = top_right;

						if (c3 != 0) label[il] = c3;
						else if (c1 != 0) {
							label[il] = c1;
							if (c4 != 0) __label_union( c4, c1, Table );
						} else if (c2 != 0) {
							label[il] = c2;
							if (c4 != 0) __label_union( c2, c4, Table );
						} else if (c4) label[il] = c4;
						else {
							// new label
							label[il] = LABEL;
							++LABEL;
							++nclass;
						}
					}
				}
			}
			{ /* Step 01.5: Intermediate step to compress the parent table ========== */
				for (i = 0; i < max_comp; i++) {
					if (Table[i] == -1) continue;
					nclass--;
					Table[i] = __label_find( i, Table );
				}
			}
			{ /* Step 02: Backward verification ===================================== */
				long int x;
				for (i = 0; i < msize; i++) {
					x = label[i];
					label[i] = __label_find( x, Table );
				}
			}
			{ /* Step 03: Convert all labels to sequence 0, 1, 2, ... , *count ====== */
				int c;
				int p;
				c = 1;
				for (i = 0; i < msize; i++) {

					p = (int) label[i];

					/* Is background */
					if (p == 0) continue;

					/* Is foreground */
					if (Table[p] == -1) {
						Table[p] = c;
						c++;
					}
					label[i] = Table[p];
				}
			}
			mem_free( Table );

			return nclass;
		}

		/**
		 * Adaption of  label_he_chao_suzuki_wu to labeling all pixels different of ground_val
		 */
		template<typename T, typename Tlabel> int label_he_chao_suzuki_wu_ground_val( int rows, int columns, T * mat, int st_mat, Tlabel * label, int st_label, T ground_value ) {

			int msize;
			int max_comp;
			int nclass;

			int r, c, _r, i, _rl, il;
			int * Table;

			msize = rows * columns;

			max_comp = imp__ceil(rows / 2.0) * imp__ceil(columns / 2.0);

			impl::math::m_set( rows, columns, label, st_label, 0 );

			Table = __create_table( max_comp );

			{/* Step 01: Forward raster scan ======================================= */
				T left;
				T top_left;
				T top;
				T top_right;
				T c1, c2, c3, c4;
				T LABEL = 1;

				nclass = 0;

				for (r = 1, _r = r * st_mat, _rl = r * st_label; r < rows - 1; r++, _r += st_mat, _rl += st_label) {
					for (c = 1; c < columns - 1; c++) {
						i = _r + c;
						il = _rl + c;

						if (mat[i] == ground_value) continue;

						/* = mat[i] is an object ==================================== */

						left = label[il - 1];
						top = label[il - columns];
						top_left = label[il - columns - 1];
						top_right = label[il - columns + 1];

						c1 = left;
						c2 = top_left;
						c3 = top;
						c4 = top_right;

						if (c3 != 0) label[il] = c3;
						else if (c1 != 0) {
							label[il] = c1;
							if (c4 != 0) __label_union( c4, c1, Table );
						} else if (c2 != 0) {
							label[il] = c2;
							if (c4 != 0) __label_union( c2, c4, Table );
						} else if (c4) label[il] = c4;
						else {
							// new label
							label[il] = LABEL;
							++LABEL;
							nclass++;
						}
					}
				}
			}
			{ /* Step 01.5: Intermediate step to compress the parent table ========== */
				for (i = 0; i < max_comp; i++) {
					if (Table[i] == -1) continue;
					nclass--;
					Table[i] = __label_find( i, Table );
				}
			}
			{ /* Step 02: Backward verification ===================================== */
				long int x;
				for (i = 0; i < msize; i++) {
					x = label[i];
					label[i] = __label_find( x, Table );
				}
			}
			{ /* Step 03: Convert all labels to sequence 0, 1, 2, ... , *count ====== */
				int c;
				int p;
				c = 1;
				for (i = 0; i < msize; i++) {

					p = (int) label[i];

					/* Is background */
					if (p == 0) continue;

					/* Is foreground */
					if (Table[p] == -1) {
						Table[p] = c;
						c++;
					}
					label[i] = Table[p];
				}
			}
			mem_free( Table );

			return nclass;
		}

	}
}
#endif /* IMPL_LABEL_HPP */
