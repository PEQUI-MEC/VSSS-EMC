#include "Geometry.h"

using namespace Geometry;

double Geometry::distance(const Point &a, const Point &b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

double Geometry::distance_x(const Point &a, const Point &b) {
	return abs(a.x - b.x);
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
	return {x + v.size * std::sin(v.theta),
			y + v.size * std::cos(v.theta)};
}

double Geometry::degree_to_rad(double degree) {
	return degree * 0.0174533;
}

Point Geometry::from_cv_point(cv::Point cv_point) {
	return {cv_point.x * (1.7 / 640.0),
			1.3 - cv_point.y * (1.3 / 480.0)};
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

//	Exemplos de uso de Geometry
Point tests() {
	Point p3{0, 0};
	Point p4{2, 3};
	Line robot_line(p3, p4);

//	Teste de ponto na direção de vetor
	Vector v1 = p4 - p3;
	Point x = p3 + v1.unitary() * 2;
//	ou
	Point x2 = p3 + v1.with_size(2);

//	Teste de intersecção
	Line test_line(x, p3);
	Point inter = intersection(test_line, robot_line);
	return inter;
}

Pose::Pose(cv::Point cv_position, double cv_orientation) :
		position(from_cv_point(cv_position)),
		orientation(-cv_orientation) {}
