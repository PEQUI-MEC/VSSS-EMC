/*
 * pixel_adjacency.hpp
 *
 *  Created on: May 12, 2013
 *      Author: gustavo
 */

#ifndef IMPL_PIXEL_ADJACENCY_HPP_
#define IMPL_PIXEL_ADJACENCY_HPP_

namespace impl {
	namespace img {

		template<typename T> inline int adjacency_N4(T * mat, int stride) {
			if (*mat == 0) return 0;
			if (*(mat - stride) != 0) return 1;
			if (*(mat - 1) != 0) return 1;
			if (*(mat + 1) != 0) return 1;
			if (*(mat + stride) != 0) return 1;
			return 0;
		}

		template<typename T, typename Tout> inline int adjacency_N4(int rows, int columns,  //
				T * mat, int stm,  //
				Tout * adj, int sta) {

			for (int r = 1, _r = r * stm, _ra = r * sta; r < rows - 1; ++r, _r += stm, _ra += sta) {
				for (int c = 1; c < columns - 1; ++c) {
					adj[_ra + c] = adjacency_N4(&mat[_r + c], stm);
				}
			}

			return 0;
		}

		template<typename T> inline int adjacency_Nd(T * mat, int stride) {
			if (*mat == 0) return 0;
			if (*(mat - stride - 1) != 0) return 1;
			if (*(mat - stride + 1) != 0) return 1;
			if (*(mat + stride - 1) != 0) return 1;
			if (*(mat + stride + 1) != 0) return 1;
			return 0;
		}

		template<typename T, typename Tout> inline int adjacency_Nd(int rows, int columns,  //
				T * mat, int stm,  //
				Tout * adj, int sta) {

			for (int r = 1, _r = r * stm, _ra = r * sta; r < rows - 1; ++r, _r += stm, _ra += sta) {
				for (int c = 1; c < columns - 1; ++c) {
					adj[_ra + c] = adjacency_Nd(&mat[_r + c], stm);
				}
			}

			return 0;
		}

		template<typename T> inline int adjacency_N8(T * mat, int stride) {
			return adjacency_N4(mat, stride) || adjacency_Nd(mat, stride);
			return 0;
		}

		template<typename T, typename Tout> inline int adjacency_N8(int rows, int columns,  //
				T * mat, int stm,  //
				Tout * adj, int sta) {

			for (int r = 1, _r = r * stm, _ra = r * sta; r < rows - 1; ++r, _r += stm, _ra += sta) {
				for (int c = 1; c < columns - 1; ++c) {
					adj[_ra + c] = adjacency_N8(&mat[_r + c], stm);
				}
			}

			return 0;
		}

	}
}

#endif /* IMPL_PIXEL_ADJACENCY_HPP_ */
