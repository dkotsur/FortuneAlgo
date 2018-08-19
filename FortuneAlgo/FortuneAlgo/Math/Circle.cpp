//
//  Circle.cpp
//  FortuneAlgo
//
//  Created by Dmytro Kotsur on 20/04/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#include "Circle.hpp"
#include <iostream>


bool findCircleCenter(const Point2D &p1, const Point2D &p2, const Point2D &p3, Point2D &center) {
    
    // get normalized vectors
    Point2D u1 = (p1 - p2).normalized(), u2 = (p3 - p2).normalized();
    
    double cross = crossProduct(u1, u2);
    
    // check if vectors are collinear
    if (fabs(cross) < CIRCLE_CENTER_EPSILON) {
        return false;
    }
    
    // get cental points
    Point2D pc1 = 0.5 * (p1 + p2), pc2 = 0.5 * (p2 + p3);
    
    // get free components
    double b1 = dotProduct(u1, pc1), b2 = dotProduct(u2, pc2);
    
    // calculate the center of a circle
    center.x = (b1 * u2.y - b2 * u1.y) / cross;
    center.y = (u1.x * b2 - u2.x * b1) / cross;
    
    return true;
}

/*
int main(int argc, char *argv[]) {
    
    Point2D center;
    
    std::cout << findCircleCenter(Point2D(0.0, 0.0), Point2D(4.0, 1.0), Point2D(8.0, 0.0), center) << std::endl;
    std::cout << center << std::endl;
    
    return 0;
}
 */
