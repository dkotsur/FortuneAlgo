//
//  main.cpp
//  FortuneAlgo
//
//  Created by Dmytro Kotsur on 18/04/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>

#include "Point2D.h"
#include "VoronoiDiagram.hpp"
#include "Beachline.hpp"


#include "matplotlibcpp.h"

namespace bl = beachline;
namespace plt = matplotlibcpp;


/**
 Converts vector of Point2D into vector of double.
 If coord_id == 0, function returns x-coordinate
 If coord_id == 1, function returns y-coordinate
 */
std::vector<double> get_coordinate(const std::vector<Point2D> &points, int coord_id) {
    std::vector<double> c(points.size(), 0.0);
    if (coord_id == 0) {
        for (size_t i = 0; i < points.size(); ++i) {
            c[i] = points[i].x;
        }
    } else {
        for (size_t i = 0; i < points.size(); ++i) {
            c[i] = points[i].y;
        }
    }
    return c;
}


/**
 Converts vector of Vertex pointers into vector of double.
 If coord_id == 0, function returns x-coordinate
 If coord_id == 1, function returns y-coordinate
 */
std::vector<double> get_coordinate(const std::vector<bl::VertexPtr> &points, int coord_id) {
    std::vector<double> c(points.size(), 0.0);
    if (coord_id == 0) {
        for (size_t i = 0; i < points.size(); ++i) {
            c[i] = points[i]->point.x;
        }
    } else {
        for (size_t i = 0; i < points.size(); ++i) {
            c[i] = points[i]->point.y;
        }
    }
    return c;
}


void plot_circle(const Point2D &c, double r) {
    std::vector<double> x, y;
    for (double t = 0.0; t < 2 * M_PI + 0.05; t += 0.05) {
        x.push_back(sin(t) * r + c.x);
        y.push_back(cos(t) * r + c.y);
    }
    plt::plot(x, y, "m-");
}


std::vector<Point2D> readPoints(const std::string &fileName, int step=1) {
    
    std::vector<Point2D> points;
    std::ifstream in(fileName);
    
    double x, y;
    int points_n = 0, i =0;
    
    if (in) {
        in >> points_n;
        for (int p_i = 0; p_i < points_n; ++p_i) {
            in >> x >> y;
            if (p_i == i) {
                points.push_back(Point2D(x, y));
                i += step;
            }
        }
    }
    return points;
}


double random_number() {
    return double(rand()) / double(RAND_MAX);
}


std::vector<Point2D> randomPoint(int number) {
    srand(static_cast<unsigned int>(time(0)));
    std::vector<Point2D> points;
    for (int i = 0; i < number; ++i) {
        double x = random_number(), y = random_number();
        points.push_back(Point2D(x, y));
    }
    std::sort(points.begin(), points.end(), [](const Point2D &p1, const Point2D &p2) {
        return (fabs(p1.y - p2.y) < POINT_EPSILON && p1.x < p2.x) || (fabs(p1.y - p2.y) >= POINT_EPSILON && p1.y < p2.y);
    });
    for (int i = 1; i < number; ++i) {
        if ((points[i-1] - points[i]).norm() < POINT_EPSILON) {
            points[i-1].x = random_number();
        }
    }
    for (int i = number-1; i >= 0; --i) {
        int j = rand() % (i+1);
        std::swap(points[i], points[j]);
    }
    return points;
}


void initEdgePointsVis(bl::HalfEdgePtr h, std::vector<double> &x, std::vector<double> &y,
                       const std::vector<Point2D> &points) {
    
    if (h->vertex != nullptr && h->twin->vertex != nullptr) {
        
        x[0] = h->vertex->point.x;
        x[1] = h->twin->vertex->point.x;
        
        y[0] = h->vertex->point.y;
        y[1] = h->twin->vertex->point.y;
        
    } else if (h->vertex != nullptr) {
        
        x[0] = h->vertex->point.x;
        y[0] = h->vertex->point.y;
        
        Point2D norm = (points[h->l_index] - points[h->r_index]).normalized().getRotated90CCW();
        x[1] = x[0] + norm.x * 1000;
        y[1] = y[0] + norm.y * 1000;
        
    } else if (h->twin->vertex != nullptr) {
        
        x[0] = h->twin->vertex->point.x;
        y[0] = h->twin->vertex->point.y;
        
        Point2D norm = (points[h->twin->l_index] - points[h->twin->r_index]).normalized().getRotated90CCW();
        x[1] = x[0] + norm.x * 1000;
        y[1] = y[0] + norm.y * 1000;
        
    } else {
        
        Point2D p1 = points[h->l_index], p2 = points[h->r_index];
        
        Point2D norm = (p1 - p2).normalized().getRotated90CCW();
        Point2D c = 0.5 * (p1 + p2);
        
        x[0] = c.x + norm.x * 1000;
        x[1] = c.x - norm.x * 1000;
        
        y[0] = c.y + norm.y * 1000;
        y[1] = c.y - norm.y * 1000;
    }
}


int main(int argc, const char *argv[]) {
    
    // Generate random points
    std::vector<Point2D> points = randomPoint(100);
    //std::vector<Point2D> points = readPoints("/Users/dkotsur/Projects/KNU/FortuneAlgo/Data/fail_1.txt");
    
    std::vector<bl::HalfEdgePtr> halfedges, faces;
    std::vector<bl::VertexPtr> vertices;
    
    for(size_t i = 0; i < points.size(); ++i) {
        std::vector<double> _x, _y;
        _x.push_back(points[i].x); _y.push_back(points[i].y);
        plt::named_plot(std::to_string(i), _x, _y, ".");
    }
    
    // Construct Voronoi diagram
    build_voronoi(points, halfedges, vertices, faces);
    
    for (size_t i = 0; i < halfedges.size(); ++i) {
        bl::HalfEdgePtr h = halfedges[i];
        
        std::vector<double> x(2, 0.0), y(2, 0.0);
        initEdgePointsVis(h, x, y, points);
        
        plt::plot(x, y, "lightgray");
    }
    
    // Check if iterator works fine
    for (size_t i = 0; i < halfedges.size(); ++i) {
        bl::HalfEdgePtr h = halfedges[i];
        do {
            assert(halfedges[i]->l_index == h->l_index);
            h = h->next;
        } while (h != nullptr && h != halfedges[i]);
    }
    
    /**
     Iterate around the vertex CCW
     */
//    bl::HalfEdgePtr he_end = halfedges[6], he = he_end;
//    if (he->vertex != nullptr) {
//        do {
//            std::vector<double> x(2, 0.0), y(2, 0.0);
//            initEdgePointsVis(he, x, y, points);
//            plt::plot(x, y, "m-");
//            he = he->vertexNextCCW();
//        } while (he != he_end && he != nullptr);
//    }
//    printf("\n\n");
    
    /**
     Iterate around the point CCW
     */
//    he_end = faces[1]; he = he_end;
//    if (he != nullptr) {
//        do {
//            std::vector<double> x(2, 0.0), y(2, 0.0);
//            initEdgePointsVis(he, x, y, points);
//            plt::plot(x, y, "c--");
//            he = he->next;
//        } while (he != nullptr && he != he_end);
//    }
    
    plt::axis("equal");
    //plt::xlim(-0.5, 1.5);
    //plt::ylim(-0.5, 1.5);
    plt::show();
    
    return 0;
}

