//
//  Circle.hpp
//  FortuneAlgo
//
//  Created by Dmytro Kotsur on 20/04/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#ifndef Circle_hpp
#define Circle_hpp


#include "Point2D.h"

#define CIRCLE_CENTER_EPSILON 1.0e-7

/**
 
 Find a center of a circle with given three points.
 Returns false if points are collinear.
 Otherwise returns true and updates x- and y-coordinates of the `center` of circle.
 
 */
bool findCircleCenter(const Point2D &p1, const Point2D &p2, const Point2D &p3, Point2D &center);


#endif /* Circle_hpp */
