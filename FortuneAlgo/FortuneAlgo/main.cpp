//
//  main.cpp
//  FortuneAlgo
//
//  Created by Dmytro Kotsur on 18/04/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

#include "Point2D.h"
#include "Beachline.hpp"
#include "Parabola.hpp"
#include "Circle.hpp"

namespace bl = beachline;


struct Event {
    
    enum { SITE = 0, CIRCLE = 1, SKIP = 2, };
    
    
    int type;
    Point2D point;
    
    /*
     Site event attributes:
     */
    int index;
    
    /*
     Circle event attributes:
    */
    Point2D center;
    bl::BLNode *arc;
    
    
    Event(int _index = -1, int _type = Event::SKIP, const Point2D &_point = Point2D(0.0, 0.0)) :
        index(_index), type(_type), point(_point), arc(nullptr) {}
    
};



struct Point2DComparator {
    bool operator()(const Point2D &p1, const Point2D &p2) {
        return (fabs(p1.y - p2.y) < POINT_EPSILON && p1.x > p2.x) || (fabs(p1.y - p2.y) > POINT_EPSILON && p1.y > p2.y);
    }
};


struct EventComparator {
    Point2DComparator point_cmp;
    bool operator()(const Event &e1, const Event &e2) {
        return point_cmp(e1.point, e2.point);
    }
};



struct TreeNode {
    enum { BRANCH = 0, ARC = 1 };
    
    int node_type;
    virtual double value() = 0;
};

struct BranchNode : TreeNode {
    double value() {
        return 0.0;
    }
};

struct ArcNode : TreeNode {
    double value() {
        return 0.0;
    }
};


struct HalfEdge {
    
    size_t index; // Index of a cell
    
    Point2D *vertex;
    HalfEdge *twin;
    
    HalfEdge *next;
    HalfEdge *prev;
};




bool checkCircleEvent(bl::BLNode *n1, bl::BLNode *n2, bl::BLNode *n3,
                      std::vector<Point2D> &points, double sweepline,
                      Event &e) {
    
    if (n1 == nullptr || n2 == nullptr || n3 == nullptr)
        return false;
    
    Point2D p1 = points[n1->get_id()];
    Point2D p2 = points[n2->get_id()];
    Point2D p3 = points[n3->get_id()];
    Point2D center, bottom;
    
    if (!findCircleCenter(p1, p2, p3, center))
        return false;
    
    bottom = center + Point2D(0.0, (center - p1).norm());
    
    // check event
    if (fabs(bottom.y - sweepline) < POINT_EPSILON || sweepline < bottom.y) {
        e = Event(-1, Event::CIRCLE, bottom);
        e.center = center;
        e.arc = n2;
        
        return true;
    }
    
    return false;
}



int main(int argc, const char *argv[]) {
    
    std::vector<Point2D> points = {
        Point2D(2.0, 1.0),
        Point2D(4.0, 3.0),
        Point2D(7.0, 1.0)
    };
    
    std::priority_queue<Event, std::vector<Event>, EventComparator> pq;
    
    for (size_t i = 0; i < points.size(); ++i) {
        pq.push(Event(static_cast<int>(i), Event::SITE, points[i]));
    }
    
    bl::BLNode *root = nullptr;
    double sweepline = 0L;
    
    while (!pq.empty()) {
        
        // extract new event from the queue
        Event e = pq.top(); pq.pop();

        // set position of a sweepline
        sweepline = e.point.y;
        
        if (e.type == Event::SITE) { // Handle site event
            int point_i = e.index;
            if (root == nullptr) {
                root = new bl::BLNode(std::make_pair(point_i, point_i), &sweepline, &points);
            } else {
                bl::BLNode *arc = bl::find(root, e.point.x);
                bl::BLNode *subtree = nullptr, *left_leaf = nullptr, *right_leaf = nullptr;
                
                if (arc->circle_event != nullptr) {
                    Event* circle_e = static_cast<Event*>(arc->circle_event);
                    circle_e->type = Event::SKIP; // ignore corresponding event
                }
                
                // check number of intersection points
                int isp_num = intersectionPointsNum(points[arc->get_id()], e.point, sweepline);
                
                // different subtrees depending on the number of intersection points
                if (isp_num == 1) {
                    subtree = bl::make_simple_subtree(point_i, arc->get_id(), &sweepline, &points);
                    left_leaf = subtree->left;
                    right_leaf = subtree->right;
                } else if (isp_num == 2) {
                    subtree = bl::make_subtree(point_i, arc->get_id(), &sweepline, &points);
                    left_leaf = subtree->left;
                    right_leaf = subtree->right->right;
                } else {
                    std::cout << "Duplicating point!!!" << std::endl;
                    continue;
                }
                
                if (arc->prev != nullptr)
                    bl::connect(arc->prev, left_leaf);
                
                if (arc->next != nullptr)
                    bl::connect(right_leaf, arc->next);
                
                // Replace old leaf with a subtree and rebalance it
                root = bl::replace(arc, subtree);
                
                //
                // Check circle events
                //
                Event circle_event;
                if (checkCircleEvent(left_leaf->prev, left_leaf, left_leaf->next, points, sweepline, circle_event)) {
                    pq.push(circle_event);
                }
                
                if (checkCircleEvent(right_leaf->prev, right_leaf, right_leaf->next, points, sweepline, circle_event)) {
                    pq.push(circle_event);
                }
            }
            
        } else if (e.type == Event::CIRCLE) { // Handle circle event

            //
            bl::BLNode *arc = e.arc, *prev_leaf = nullptr, *next_leaf = nullptr;
            
            // remove circle event corresponding to next leaf
            if (arc->prev != nullptr && arc->prev->circle_event != nullptr) {
                Event* circle_e = static_cast<Event*>(arc->prev->circle_event);
                circle_e->type = Event::SKIP; // ignore corresponding event
            }
            
            // remove circle event corresponding to prev leaf
            if (arc->next != nullptr && arc->next->circle_event != nullptr) {
                Event* circle_e = static_cast<Event*>(arc->next->circle_event);
                circle_e->type = Event::SKIP; // ignore corresponding event
            }
            
            // store pointers to the next and previous leaves
            prev_leaf = arc->prev;
            next_leaf = arc->right;
            
            // remove arc from the beachline
            root = bl::remove(arc);
            
            // check new circle events
            if (prev_leaf != nullptr && next_leaf != nullptr) {
                Event circle_event;
                if (checkCircleEvent(prev_leaf->prev, prev_leaf, next_leaf, points, sweepline, circle_event)) {
                    pq.push(circle_event);
                }
                if (checkCircleEvent(prev_leaf, next_leaf, next_leaf->next, points, sweepline, circle_event)) {
                    pq.push(circle_event);
                }
            }
            
        }
    }
    
    
    return 0;
}

