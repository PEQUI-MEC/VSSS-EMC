/*
 * histogram.hpp
 *
 *  Created on: Sep 14, 2012
 *      Author: gustavo
 */

#ifndef IMPL_HIST_HPP
#define IMPL_HIST_HPP

#include "../math/mbop.hpp"

namespace impl {
	namespace img {

		/**
		 * @param bin - bandwidth
		 */
		template<typename T> inline int histogram_size( T bin, T min_level, T max_level ) {
			int hsize = (max_level - min_level) / bin;
			++hsize;
			return hsize;
		}

		template<typename T> inline T histogram_level( int index, T bin, T min_level ) {
			return (index * bin) + min_level;
		}

		template<typename T> inline int histogram_index( T Th, T bin, T min_level ) {
			return (int) ((Th - min_level) / bin);
		}

		template<typename T> void histogram( int rows, int cols, T * Mat, int stm, long * hist, T bin, T min_level, T max_level ) {

			int hsize = histogram_size( bin, min_level, max_level );

			int r, c, _r, i;
			for( r = 0, _r = r; r < rows; ++r, _r += cols ) {
				for( c = 0; c < cols; ++c ) {

					i = _r + c;

					if( Mat[i] <= min_level ) {

						++hist[0];

					} else if( Mat[i] > max_level ) {

						++hist[hsize - 1];

					} else {

						T val;
						val = Mat[i] - min_level;
						val = val / bin;
						i = imp__round(val);

						++hist[i];

					}

				}
			}
		}

		//template<typename T> void histogram( int rows, int cols, T * Mat, int stm, long * hist );

//		void histogram( int rows, int cols, char * Mat, int stm, long * hist ) {
//
//			int r, c, _r, i;
//			for( r = 0, _r = r; r < rows; ++r, _r += cols ) {
//				for( c = 0; c < cols; ++c ) {
//					i = _r + c;
//					++hist[Mat[i]];
//				}
//			}
//		}

		/**
		 *
		 */
		template<typename T> void histogram_equalization( int rows, int columns, T * Mat, int st_mat, int new_nlevels, long * hist, int hsize, T bin, T min_level ) {

			//int new_grays_levels = 256;
			int r, _r, c, i;
			long sum, sum_of_h[hsize];

			double constant;
			long int imsize;

			imsize = columns * rows;

			/* Accumulated histogram */
			sum = 0;
			for( r = 0; r < hsize; r++ ) {
				sum = sum + hist[r];
				sum_of_h[r] = sum;
			}

			/* constant = new # of gray levels div by area */
			constant = (new_nlevels - 1) / (double) (imsize);

			for( r = 0, _r = r * st_mat; r < rows; ++r, _r += st_mat ) {
				for( c = 0; c < columns; c++ ) {

					i = _r + c;

					int k = histogram_index( Mat[i], bin, min_level );
					Mat[i] = (T) (constant * sum_of_h[k]);
				}
			}
		}

	}

}

#endif /* IMPL_HIST_HPP */
