#ifndef VSSS_GEOMETRY_H
#define VSSS_GEOMETRY_H

#include <cmath>
#include <iostream>

namespace Geometry {
	struct Point;

	struct Vector {
		double theta;
		double size;

		Vector(const double theta, const double size): theta(theta), size(size) {}
		explicit Vector(const Point &p);
	};

	struct Point {
		double x;
		double y;

		friend std::ostream& operator<<(std::ostream& os, const Point& p) {
			os << "{ " << p.x << ", " << p.y << " }";
			return os;
		}

		constexpr Point(double x, double y) noexcept : x(x), y(y) {};
		Vector operator-(const Point &p2) const;
	};

	struct Pose {
		Point position;
		Vector orientation;
	};

	struct Line {
		double a = 0;
		double b = 0;

		constexpr Line(Point p1, Point p2) noexcept {
			if(p2.x == p1.x) {
				a = (p2.y - p1.y)/(p2.x - p1.x + 0.00001);
			} else {
				a = (p2.y - p1.y)/(p2.x - p1.x);
			}
			b = p1.y - a * p1.x;
		}
	};

	struct Segment {
		Point a;
		Point b;
	};

	constexpr Point middle(const Point& p1, const Point &p2) noexcept {
		return {(p1.x + p2.x)/2, (p1.y + p2.y)/2};
	}

	double distance(const Point &a, const Point &b);
	Point intersection(const Line &l1, const Line &l2);
}

#endif //VSSS_GEOMETRY_H
