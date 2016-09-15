/*
 * timer.hpp
 *
 *  Created on: Sep 16, 2012
 *      Author: gustavo
 */

#ifndef IMPL_TIMER_HPP
#define IMPL_TIMER_HPP

#include <sys/time.h>
#include <stdio.h>

namespace impl {
	namespace util {

		struct timer {
			private:
				struct timeval tstart;
				struct timeval tend;
				double sec;
				double us;

			public:
				double ms;


			public:
				void start() {
					gettimeofday(&tstart, NULL);
				}
				void stop() {
					gettimeofday(&tend, NULL);
					compute_time();
				}
				void print() {
					printf("time: %f (ms)\n", ms);
				}

			private:
				void compute_time() {
					sec = tend.tv_sec - tstart.tv_sec;
					us = tend.tv_usec - tstart.tv_usec;
					ms = (sec * 1000.0 + us / 1000.0);
				}

		};

	}
}

#endif /* IMPL_TIMER_HPP */
