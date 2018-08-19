//
//  Point2D.h
//  KCGLib
//
//  Created by Kotsur on 12.03.14.
//  Copyright (c) 2014 Dmytro Kotsur. All rights reserved.
//

#ifndef KCGLib_Point2D_h
#define KCGLib_Point2D_h

#include <iostream>
#include <vector>
#include <limits>
#include <cmath>

#define POINT_EPSILON 1.0e-6

class Point2D {

	struct Point2D_XY_Compare {
		bool operator()(const Point2D &p1, const Point2D &p2) {
			return (p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y));
		}
	};

public:
    double x, y;
    
    const static double Inf;
	static Point2D_XY_Compare xy_compare;
    
    Point2D(double x = 0.0, double y = 0.0);
    Point2D(const Point2D &point);
    
    friend double dotProduct(const Point2D &p1, const Point2D &p2);
    friend double crossProduct(const Point2D &p1, const Point2D &p2);
    
    friend Point2D operator+(const Point2D &p1, const Point2D &p2);
    friend Point2D operator-(const Point2D &p1, const Point2D &p2);
    friend Point2D operator/(const Point2D &p1, const Point2D &p2);
    friend Point2D operator*(const Point2D &p, double value);
    friend Point2D operator*(double value, const Point2D &p);
    friend Point2D operator/(const Point2D &p, double value);
	friend Point2D operator-(const Point2D &p);

    friend std::ostream &operator<<(std::ostream &stream, const Point2D &p);
    friend std::vector<Point2D> &operator<<(std::vector<Point2D> &v, const Point2D &p);
    
    Point2D &operator-=(const Point2D &p);
    Point2D &operator+=(const Point2D &p);
    Point2D &operator*=(double value);
    Point2D &operator/=(double value);
    
    Point2D normalized();
    void normalize();
    double norm();
    double norm2();
    
    Point2D getRotated90CW();
    Point2D getRotated90CCW();

	static bool isLeftTurn(const Point2D &p1, const Point2D &p2, const Point2D &p3);
	static bool isRightTurn(const Point2D &p1, const Point2D &p2, const Point2D &p3);
    
    double operator[](int i);
    
    void setX(double x);
    void setY(double y);
    
    bool isVertical();
    bool isHorizontal();
    bool isValid();
    
};

double dotProduct(const Point2D &p1, const Point2D &p2);
double crossProduct(const Point2D &p1, const Point2D &p2);

bool equal(const Point2D &p1, const Point2D &p2, double EPSILON = POINT_EPSILON);
bool equal(double v1, double v2, double EPSILON = POINT_EPSILON);


#endif
