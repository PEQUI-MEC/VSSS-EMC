/*
 * rng.hpp
 *
 *  Created on: Apr 25, 2013
 *      Author: gustavo
 */

#ifndef _MATH_RNG_HPP_
#define _MATH_RNG_HPP_

#include <stdlib.h>
#include <sys/time.h>

namespace math {

	namespace rng {

		/**
		 * Initialize a new seed for random numbers
		 */
		void rand_init();

		/**
		 * A normal distributed random number generator.
		 *
		 * @return a random number with mean 0 and standard deviation 1.
		 */
		double randn();

	}

}

#endif /* _MATH_RNG_HPP_ */
