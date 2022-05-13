#ifndef VSSS_GEOMETRY_H
#define VSSS_GEOMETRY_H

#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <Python.h>

namespace Geometry {
	struct Point;

	struct Vector {
		double size;
		double theta;

		Vector(const double size, const double theta) : size(size), theta(theta) {}
		constexpr Vector() noexcept : size(0), theta(0) {};
		explicit Vector(const Point &p);
		Vector operator*(double value) const;
		Vector unitary() const;
		Vector with_size(double new_size) const;
		Vector inverted_coordinates(bool invert = true) const;
		Point tangent_to_circle(Point origin, Point circle_center) const;
		double angle_to(const Vector &v) const;
	};

	struct Point {
		double x = 0;
		double y = 0;

		friend std::ostream& operator<<(std::ostream& os, const Point& p) {
			os << "{ " << p.x << ", " << p.y << " }";
			return os;
		}

		constexpr Point(double x, double y) noexcept : x(x), y(y) {};
		constexpr Point() noexcept : x(0), y(0) {};
		Vector operator-(const Point &p2) const;
		Point operator+(const Vector &v) const;
		cv::Point to_cv_point() const;
		PyObject * to_python() const;
		Point inverted_coordinates(bool invert = true) const;
		static Point from_simulator(double x, double y);
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

	constexpr double degree_to_rad(double degree) {
		return degree * M_PI / 180.0;
	}

	constexpr double rad_to_degree(double degree) {
		return degree * 180.0 / M_PI;
	}

//	Define operator* de forma associativa (value * v == v * value)
//	Vector operator*(double value, Vector v) {return v * value;}

	double distance(const Point &a, const Point &b);
	double distance_x(const Point &a, const Point &b);
	double distance_y(const Point &a, const Point &b);
	Point intersection(const Line &l1, const Line &l2);
	Point from_cv_point(cv::Point cv_point);
	Point from_cv_point(double x, double y);
	double wrap(double theta);
}

#endif //VSSS_GEOMETRY_H
