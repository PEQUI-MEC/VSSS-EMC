/*
 * otsu.hpp
 *
 *  Created on: Sep 14, 2012
 *      Author: gustavo
 */

#ifndef IMPL_OTSU_HPP
#define IMPL_OTSU_HPP

#include "histogram.hpp"

#include "../util/mem.h"

namespace impl {
	namespace img {

		template<typename T> T otsu(int size, T * hist) {

			int Th;
			T sum, total;
			int n;

			sum = 0;
			total = 0;
			for (n = 0; n < size; n++) {
				sum += n * hist[n];
				total += hist[n];
			}

			T wB, wF;
			T sumB, sumF;
			double uB, uF;
			double var;
			double var_max;

			Th = 0;

			var_max = 0;
			wB = 0;
			sumB = 0;
			for (n = 0; n < size; n++) {

				wB += hist[n];
				if (wB == 0) continue;

				wF = total - wB;
				if (wF == 0) break;

				sumB += n * hist[n];
				sumF = sum - sumB;

				uB = sumB / wB;
				uF = sumF / wF;

				var = wB * wF * (uB - uF) * (uB - uF);

				if (var > var_max) {
					var_max = var;
					Th = n;
				}
			}

			return Th;
		}

		template<typename T> T otsu(int m, int n, T * Mat, int stm) {


			int hsize = impl::img::histogram_size(1, 0, 255);
			char * hist = mem_allocn(hsize, char);




		}

	}
}

#endif /* IMPL_OTSU_HPP */
