/*
 * rng.h
 *
 *  Created on: Aug 1, 2013
 *      Author: gtl
 */

#ifndef IMPL_RNG_HPP_
#define IMPL_RNG_HPP_

#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

namespace impl {
	namespace math {
		namespace rng {

			void rand_init() {
				struct timeval tv;
				struct timezone tz;
				gettimeofday(&tv, &tz);
				srand(tv.tv_usec);
			}

			double rand_normal() {

				double R1 = (double) rand() / ((double) RAND_MAX + 1);
				double R2 = (double) rand() / ((double) RAND_MAX + 1);
				double rn = (double) sqrt(-2.0 * log(R1)) * cos(2.0 * M_PI * R2);
				return rn;
			}

			double rand_normal2() {

				double a, b, c;
				double epsilon = 10e-6;

				b = rand() / (RAND_MAX + 1.0f);
				if (b < epsilon) b = epsilon;
				c = rand() / (RAND_MAX + 1.0f);
				a = -2.0 * log(b);
				if (a < 0.0) a = 0.0;
				else a *= cos(2.0 * M_PI * c);
				return a;

			}

		}
	}
}
#endif /* IMPL_RNG_HPP_ */
