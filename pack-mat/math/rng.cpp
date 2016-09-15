/*
 * rng.cpp
 *
 *  Created on: Oct 13, 2015
 *      Author: gustavo
 */

#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include "rng.hpp"

namespace math {
	namespace rng {

		/**
		 * Initialize a new seed for random numbers
		 */
		void rand_init() {
			struct timeval tv;
			struct timezone tz;
			gettimeofday(&tv, &tz);
			srand(tv.tv_usec);
		}

		/**
		 * Algorithm 1
		 * @return
		 */
		double rand_normal() {
			double R1 = (double) rand() / ((double) RAND_MAX + 1);
			double R2 = (double) rand() / ((double) RAND_MAX + 1);
			double rn = (double) sqrt(-2.0 * log(R1)) * cos(2.0 * M_PI * R2);
			return rn;
		}

		/**
		 * Algorithm 2
		 * @return
		 */
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

		/**
		 * A normal distributed random number generator.
		 *
		 * @return a random number with mean 0 and standard deviation 1.
		 */
		double randn() {
			return rand_normal();
		}

	}
}
