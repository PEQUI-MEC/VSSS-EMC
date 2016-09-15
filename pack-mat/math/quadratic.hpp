/*
 * quadratic.hpp
 *
 *  Created on: Jul 15, 2013
 *      Author: gustavo
 */

#ifndef QUADRATIC_HPP_
#define QUADRATIC_HPP_

namespace math {

	class Quadratic {

			double x1, y1, x2, y2, x3, y3;

		public:

			Quadratic(double x1, double y1, double x2, double y2, double x3, double y3) :
					x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3) {

			}

			void apply(double &a, double& b, double& c) {

//				double p1, p2, p3, p4, p5, p6, p7, p8, p9, delta;
//
//				double x1s = x1 * x2;
//				double x2s = x2 * x2;
//				double x3s = x3 * x3;
//
//				p1 = x2 - x3;
//				p2 = x3 - x1;
//				p3 = x1 - x2;
//				p4 = x3s - x2s;
//				p5 = x1 - x3s;
//				p6 = x2s - x1s;
//				p7 = x2s * x3 - x2 * x3s;
//				p8 = x1 * x3s - x1s * x3;
//				p9 = x1s * x2 - x1 * x2s;
//
//				delta = x1s * (x2 - x3) - x1 * (x2s - x3) + 1 * (x2s * x3 - x2 * x3s);
//
//				a = (1 / delta) * ((x2 - x3) * y1 + (x3 - x1) * y2 + (x1 - x2) * y3);
//				b = (1 / delta) * (p4 * y1 + p5 * y2 + p6 * y3);
//				c = (1 / delta) * (p7 * y1 + p8 * y2 + p9 * y3);

			}

	};

	Quadratic quadratic(double x1, double y1, double x2, double y2, double x3, double y3) {
		return Quadratic(x1, y1, x2, y2, x3, y3);
	}

}

#endif /* QUADRATIC_HPP_ */
