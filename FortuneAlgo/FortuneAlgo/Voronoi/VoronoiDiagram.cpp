//
//  VoronoiDiagram.cpp
//  FortuneAlgo
//
//  Created by Dmytro Kotsur on 06/05/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#include "VoronoiDiagram.hpp"
#include "Beachline.hpp"
#include "Parabola.hpp"
#include "Circle.hpp"
#include "DCEL.hpp"

#include <queue>


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
    bl::BLNodePtr arc;
    
    
    Event(int _index = -1, int _type = Event::SKIP, const Point2D &_point = Point2D(0.0, 0.0)) :
    index(_index), type(_type), point(_point), arc(nullptr) {}
    
};


typedef std::shared_ptr<Event> EventPtr;


struct Point2DComparator {
    bool operator()(const Point2D &p1, const Point2D &p2) {
        return (p1.y == p2.y && p1.x > p2.x) || p1.y > p2.y;
    }
};


struct Point2DComparator2 {
    bool operator()(const Point2D &p1, const Point2D &p2) {
        return (p1.y == p2.y && p1.x < p2.x) || p1.y < p2.y;
    }
};


struct EventPtrComparator {
    Point2DComparator point_cmp;
    bool operator()(const EventPtr &e1, const EventPtr &e2) {
        return point_cmp(e1->point, e2->point);
    }
};


EventPtr checkCircleEvent(bl::BLNodePtr n1, bl::BLNodePtr n2, bl::BLNodePtr n3,
                          const std::vector<Point2D> &points, double sweepline) {
    
    if (n1 == nullptr || n2 == nullptr || n3 == nullptr)
        return nullptr;
    
    Point2D p1 = points[n1->get_id()];
    Point2D p2 = points[n2->get_id()];
    Point2D p3 = points[n3->get_id()];
    Point2D center, bottom;
    
    if (p2.y > p1.y && p2.y > p3.y)
        return nullptr;
    
    if (!findCircleCenter(p1, p2, p3, center))
        return nullptr;
    
    bottom = center;
    bottom.y += (center - p2).norm();
    
    // check circle event
    if (fabs(bottom.y - sweepline) < POINT_EPSILON || sweepline < bottom.y) {
        // create a circle event structure
        EventPtr e = std::make_shared<Event>(-1, Event::CIRCLE, bottom);
        // initialize attributes
        e->center = center;
        e->arc = n2;
        // add reference in the corresponding node
        n2->circle_event = e;
        return e;
    }
    
    return nullptr;
}


void build_voronoi(const std::vector<Point2D> &points,
                   std::vector<bl::HalfEdgePtr> &halfedges,
                   std::vector<bl::VertexPtr> &vertices,
                   std::vector<bl::HalfEdgePtr> &faces) {
    
    // create a priority queue
    std::priority_queue<EventPtr, std::vector<EventPtr>, EventPtrComparator> pq;
    
    // initialize it with all site events
    for (size_t i = 0; i < points.size(); ++i) {
        pq.push(std::make_shared<Event>(static_cast<int>(i), Event::SITE, points[i]));
    }
    
    // initialize vector of halfedges for faces
    faces.resize(points.size(), nullptr);
    
    // create a beachline tree
    bl::BLNodePtr root;
    double sweepline = 0L; // current position of the sweepline
    
    // process events
    while (!pq.empty()) {
        
        // extract new event from the queue
        EventPtr e = pq.top(); pq.pop();
        
        // set position of a sweepline
        sweepline = e->point.y;
        
        if (e->type == Event::SITE) { // handle site event
            
            int point_i = e->index;
            if (root == nullptr) { // init empty beachline tree
                root = std::make_shared<bl::BLNode>(std::make_pair(point_i, point_i), &sweepline, &points);
            } else { // if it's not empty
                
                bl::BLNodePtr arc = bl::find(root, e->point.x);
                bl::BLNodePtr subtree, left_leaf, right_leaf;
                
                if (arc->circle_event != nullptr) {
                    EventPtr circle_e = arc->circle_event;
                    circle_e->type = Event::SKIP; // ignore corresponding event
                }
                
                // check number of intersection points
                int isp_num = intersectionPointsNum(points[arc->get_id()], e->point, sweepline);
                
                // different subtrees depending on the number of intersection points
                if (isp_num == 1) {
                    subtree = bl::make_simple_subtree(point_i, arc->get_id(), &sweepline, &points, halfedges);
                    left_leaf = subtree->left;
                    right_leaf = subtree->right;
                } else if (isp_num == 2) {
                    subtree = bl::make_subtree(point_i, arc->get_id(), &sweepline, &points, halfedges);
                    left_leaf = subtree->left;
                    right_leaf = subtree->right->right;
                } else {
                    continue;
                }
                
                if (arc->prev != nullptr)
                    bl::connect(arc->prev, left_leaf);
                
                if (arc->next != nullptr)
                    bl::connect(right_leaf, arc->next);
                
                // Replace old leaf with a subtree and rebalance it
                root = bl::replace(arc, subtree);
                
                // Check circle events
                EventPtr circle_event = checkCircleEvent(left_leaf->prev, left_leaf, left_leaf->next, points, sweepline);
                if (circle_event != nullptr) {
                    pq.push(circle_event);
                }
                circle_event = checkCircleEvent(right_leaf->prev, right_leaf, right_leaf->next, points, sweepline);
                if (circle_event != nullptr) {
                    pq.push(circle_event);
                }
            }
            
        } else if (e->type == Event::CIRCLE) { // handle circle event
            
            bl::BLNodePtr arc = e->arc, prev_leaf, next_leaf;

            // get breakpoint nodes
            std::pair<bl::BLNodePtr, bl::BLNodePtr> breakpoints = bl::breakpoints(arc);
            
            // recheck if it's a false alarm 1
            if (breakpoints.first == nullptr || breakpoints.second == nullptr) {
                continue;
            }
            
            // recheck if it's a false alarm 2
            double v1 = breakpoints.first->value(), v2 = breakpoints.second->value();
            
            if (fabs(v1 - v2) > POINT_EPSILON) {
                continue;
            }
            
            // create a new vertex and insert into doubly-connected edge list
            bl::VertexPtr vertex = std::make_shared<bl::Vertex>(e->center);
            bl::HalfEdgePtr h_first = breakpoints.first->edge;
            bl::HalfEdgePtr h_second = breakpoints.second->edge;
            
            // store vertex of Voronoi diagram
            vertices.push_back(vertex);
            
            // remove circle event corresponding to next leaf
            if (arc->prev != nullptr && arc->prev->circle_event != nullptr) {
                EventPtr circle_e = arc->prev->circle_event;
                circle_e->type = Event::SKIP; // ignore corresponding event
            }
            
            // remove circle event corresponding to prev leaf
            if (arc->next != nullptr && arc->next->circle_event != nullptr) {
                EventPtr circle_e = arc->next->circle_event;
                circle_e->type = Event::SKIP; // ignore corresponding event
            }
            
            // store pointers to the next and previous leaves
            prev_leaf = arc->prev;
            next_leaf = arc->next;
            
            if (prev_leaf == nullptr || next_leaf == nullptr) {
                int a = 100; a++;
            }
            
            // They should not be null
            assert(prev_leaf != nullptr);
            assert(next_leaf != nullptr);
            
            // get node associated with a new edge
            bl::BLNodePtr new_edge_node;
            if (arc->parent == breakpoints.first)
                new_edge_node = breakpoints.second;
            else
                new_edge_node = breakpoints.first;
            
            // remove arc from the beachline
            root = bl::remove(arc);
            
            // make a new pair of halfedges
            std::pair<bl::HalfEdgePtr, bl::HalfEdgePtr> twin_nodes = bl::make_twins(prev_leaf->get_id(), next_leaf->get_id());
            new_edge_node->edge = twin_nodes.first;
            // new_edge_node->edge = twin_nodes.first;

            // connect halfedges
            bl::connect_halfedges(h_second, h_first->twin);
            bl::connect_halfedges(h_first, twin_nodes.first);
            bl::connect_halfedges(twin_nodes.second, h_second->twin);
            
            // halfedges are pointing into a vertex  -----> O <-----
            // not like this <---- O ----->
            // counterclockwise
            h_first->vertex = vertex;
            h_second->vertex = vertex;
            twin_nodes.second->vertex = vertex;
            
            halfedges.push_back(twin_nodes.first);
            halfedges.push_back(twin_nodes.second);
            
            // check new circle events
            if (prev_leaf != nullptr && next_leaf != nullptr) {
                EventPtr circle_event = checkCircleEvent(prev_leaf->prev, prev_leaf, next_leaf, points, sweepline);
                if (circle_event != nullptr) {
                    pq.push(circle_event);
                }
                circle_event = checkCircleEvent(prev_leaf, next_leaf, next_leaf->next, points, sweepline);
                if (circle_event != nullptr) {
                    pq.push(circle_event);
                }
            }
        }
    }
    
    // Fill edges corresponding to faces
    for (size_t i = 0; i < halfedges.size(); ++i) {
        bl::HalfEdgePtr he = halfedges[i];
        if (he->prev == nullptr || faces[he->l_index] == nullptr) {
            faces[he->l_index] = he;
        }
    }
}
