#include "Geometry.h"
#include "Field.h"

using namespace Geometry;

double Geometry::wrap(double theta) {
	theta = std::fmod(theta, 2 * M_PI);
	if (theta > M_PI) return theta - 2 * M_PI;
	else if (theta < -M_PI) return theta + 2 * M_PI;
	else return theta;
}

double Geometry::distance(const Point &a, const Point &b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

double Geometry::distance_x(const Point &a, const Point &b) {
	return std::abs(a.x - b.x);
}

double Geometry::distance_y(const Point &a, const Point &b) {
	return std::abs(a.y - b.y);
}

Point Geometry::intersection(const Line &l1, const Line &l2) {
	if(l1.a == l2.a && l1.b == l2.b) return {0, l1.b};
	else {
		double x = (l2.b - l1.b)/(l1.a - l2.a);
		return {x, l1.a * x + l1.b};
	};
}

Vector Point::operator-(const Point &p2) const {
	return {distance(*this, p2), std::atan2(y - p2.y, x - p2.x)};
}

Point Point::operator+(const Vector &v) const {
	return {x + v.size * std::cos(v.theta),
			y + v.size * std::sin(v.theta)};
}

Point Geometry::from_cv_point(const cv::Point cv_point) {
	return from_cv_point(cv_point.x, cv_point.y);
}

Point Geometry::from_cv_point(const double x, const double y) {
	return {x * (1.7 / 640.0),
			1.3 - y * (1.3 / 480.0)};
}

cv::Point Point::to_cv_point() const {
	return {static_cast<int>(x * 640 / 1.7),
			480 - static_cast<int>(y * 480 / 1.3) };
}

PyObject * Point::to_python() const {
	auto pyposition = Py_BuildValue("(dd)", x, y);
	return pyposition;
}

Point Point::from_simulator(double x, double y) {
	return {x + field::field_width/2, y + field::field_height/2};
}

Point Point::inverted_coordinates() const {
	return Point(field::field_width - x, field::field_height - y);
}

Vector::Vector(const Point &p) {
	size = distance(p, {0,0});
	theta = std::atan2(p.y, p.x);
}

Vector Vector::unitary() {
	return {1, theta};
}

Vector Vector::with_size(double new_size) {
	return {new_size, theta};
}

Vector Vector::operator*(double value) {
	return {size * value, theta};
}
