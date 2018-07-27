#include "Geometry.h"

using namespace Geometry;

double Geometry::distance(const Point &a, const Point &b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

Point Geometry::intersection(const Line &l1, const Line &l2) {
	if(l1.a == l2.a && l1.b == l2.b) return {0, l1.b};
	else {
		double x = (l2.b - l1.b)/(l1.a - l2.a);
		return {x, l1.a * x + l1.b};
	};
}

Vector Point::operator-(const Point &p2) const {
	return {std::atan2(y - p2.y, x - p2.x), distance(*this, p2)};
}

Vector::Vector(const Point &p) {
	theta = std::atan2(p.y, p.x);
	size = distance(p, {0,0});
}

static const Point upper_corner(10, 10);
static const Point lower_corner(10, 0);
static const Line goal_line(upper_corner, lower_corner);

Point test1() {
	Point p3{0, 0};
	Point p4{2, 3};
	Line robot_line(p3, p4);
	Point inter = intersection(goal_line, robot_line);
	return inter;
}


