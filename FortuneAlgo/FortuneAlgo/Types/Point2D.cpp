//
//  Point2D.cpp
//  KCGLib
//
//  Created by Kotsur on 12.03.14.
//  Copyright (c) 2014 Dmytro Kotsur. All rights reserved.
//

#include "Point2D.h"
#include <cmath>

#if defined(_WIN64) || defined(_WIN32)
    #define isnan(x) _isnan(x)
#endif

using namespace std;

const double Point2D::Inf = std::numeric_limits<double>::infinity();

Point2D::Point2D_XY_Compare Point2D::xy_compare = Point2D::Point2D_XY_Compare();

Point2D::Point2D(double _x, double _y) : x(_x), y(_y) {
}

Point2D::Point2D(const Point2D &point) : x(point.x), y(point.y) {
}

double dotProduct(const Point2D &p1, const Point2D &p2) {
    return p1.x * p2.x + p1.y * p2.y;
}

double crossProduct(const Point2D &p1, const Point2D &p2) {
    return p1.x * p2.y - p1.y * p2.x;
}

Point2D operator+(const Point2D &p1, const Point2D &p2) {
    return Point2D(p1.x + p2.x, p1.y + p2.y);
}

Point2D operator-(const Point2D &p1, const Point2D &p2) {
    return Point2D(p1.x - p2.x, p1.y - p2.y);
}

Point2D operator/(const Point2D &p1, const Point2D &p2) {
    return Point2D(p1.x / p2.x, p1.y / p2.y);
}

Point2D operator*(const Point2D &p, double value) {
    return Point2D(p.x * value, p.y * value);
}

Point2D operator*(double value, const Point2D &p) {
    return Point2D(p.x * value, p.y * value);
}

Point2D operator/(const Point2D &p, double value) {
    return Point2D(p.x / value, p.y / value);
}

Point2D operator-(const Point2D &p) {
	return Point2D(-p.x, -p.y);
}

std::ostream &operator<<(std::ostream &stream, const Point2D &p) {
    stream << "(" << p.x << "," << p.y << ")";
    return stream;
}

std::vector<Point2D> &operator<<(std::vector<Point2D> &v, const Point2D &p) {
    v.push_back(p);
    return v;
}

Point2D &Point2D::operator-=(const Point2D &p) {
    x -= p.x;
    y -= p.y;
    return *this;
}

Point2D &Point2D::operator+=(const Point2D &p) {
    x += p.x;
    y += p.y;
    return *this;
}

Point2D &Point2D::operator*=(double value) {
    x *= value;
    y *= value;
    return *this;
}

Point2D &Point2D::operator/=(double value) {
    x /= value;
    y /= value;
    return *this;
}

double Point2D::operator[](int i) {
    if (i==0) return x;
    else return y;
}

void Point2D::setX(double x) {
    this->x = x;
}

void Point2D::setY(double y) {
    this->y = y;
}

bool Point2D::isVertical() {
    return (y == Inf && !isnan(x) && x != Inf);
}

bool Point2D::isHorizontal() {
    return (x == Inf && !isnan(y) && y != Inf);
}

bool Point2D::isValid() {
    if (x == Inf && y == Inf)
        return false;
    return (!isnan(x) && !isnan(y));
}

Point2D Point2D::normalized() {
    return (*this) / this->norm();
}

void Point2D::normalize() {
    double n = norm();
    x /= n;
    y /= n;
}

double Point2D::norm() {
    return sqrt(x * x + y * y);
}

double Point2D::norm2() {
    return x *x + y * y;
}

Point2D Point2D::getRotated90CW() {
    return Point2D(y, -x);
}

Point2D Point2D::getRotated90CCW() {
    return Point2D(-y, x);
}

bool Point2D::isLeftTurn(const Point2D &p1, const Point2D &p2, 
						 const Point2D &p3) {
	return (crossProduct(p2 - p1, p3 - p2) > 0.0);
}

bool Point2D::isRightTurn(const Point2D &p1, const Point2D &p2, 
						  const Point2D &p3) {
	return (crossProduct(p2 - p1, p3 - p2) < 0.0);
}

bool equal(const Point2D &p1, const Point2D &p2, double EPSILON) {
    return (fabs(p1.x - p2.x) < EPSILON && fabs(p1.y - p2.y) < EPSILON);
}

bool equal(double v1, double v2, double EPSILON) {
    return fabs(v1 - v2) < EPSILON;
}
