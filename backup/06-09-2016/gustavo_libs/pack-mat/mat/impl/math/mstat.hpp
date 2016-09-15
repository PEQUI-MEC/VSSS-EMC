/*
 * mstat.hpp
 *
 *  Created on: Oct 15, 2012
 *      Author: gustavo
 */

#ifndef IMPL_MSTAT_HPP
#define IMPL_MSTAT_HPP

#include "mbop.hpp"
#include "mblaop.hpp"

namespace impl {
	namespace math {

		template<typename Tin, typename Tout> inline void m_mean(int rows, int columns, const Tin * A, int sta, Tout * U, int stu) {

			m_sum(rows, columns, A, sta, U, stu);
			m_scale(1, columns, U, stu, 1.0 / rows);

		}

		template<typename T> inline void m_mean_row(int rows, int columns, const T * A, int sta, T * U, int stu) {

			m_sum_row(rows, columns, A, sta, U, stu);
			m_scale(rows, 1, U, stu, 1.0 / (T) columns);

		}

	}
}

#endif /* IMPL_MSTAT_HPP */
