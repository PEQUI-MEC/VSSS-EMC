/*
 * Range.hpp
 *
 *  Created on: Feb 28, 2013
 *      Author: gustavo
 */

#ifndef RANGE_HPP_
#define RANGE_HPP_

namespace mat {

	template<typename T = int> class Range {

		private:
			bool all_flag;
			T Start;
			T Inc;
			T End;

		public:
			Range( T start, T inc, T end ) :
					Start( start ), Inc( inc ), End( end ), all_flag( false ) {
			}
			Range( T start, T end ) :
					Start( start ), Inc( 1 ), End( end ), all_flag( false ) {
			}

		private:
			Range() :
					Start(), Inc(), End(), all_flag( true ) {
			}

		public:
			static Range<T>& all() {
				static Range<T> __range_all;
				return __range_all;
			}

			bool is_all() const {
				return all_flag;
			}

			T get_start() const {
				return Start;
			}
			T get_inc() const {
				return Inc;
			}
			T get_end() const {
				return End;
			}
			T get_size() const {
				return ((End - Start) / Inc) + 1;
			}
	};
}
#endif /* RANGE_HPP_ */
