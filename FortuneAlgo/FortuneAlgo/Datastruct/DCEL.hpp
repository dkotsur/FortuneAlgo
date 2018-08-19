//
//  DCEL.hpp
//  FortuneAlgo
//
//  Created by Dmytro Kotsur on 26/04/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#ifndef DCEL_h
#define DCEL_h

#include "Point2D.h"


namespace DCEL {
    
    class Vertex;
    class HalfEdge;
    
    typedef std::shared_ptr<HalfEdge> HalfEdgePtr;
    typedef std::shared_ptr<Vertex> VertexPtr;
    
    
    class Vertex {
    public:
        
        Point2D point;
        HalfEdgePtr edge; // The edge points towards this vertex [-->o]
        
        Vertex(const Point2D &pos, HalfEdgePtr incident_edge = nullptr);
        
        inline double x() { return point.x; }
        inline double y() { return point.y; }
        
    };
    
    
    class HalfEdge {
    public:
        
        int l_index, r_index;
        
        VertexPtr vertex;
        HalfEdgePtr twin;
        HalfEdgePtr next;
        HalfEdgePtr prev;
        
        HalfEdge(int _l_index, int _r_index, VertexPtr _vertex = nullptr);
        
        inline VertexPtr vertex0() { return vertex; }
        inline VertexPtr vertex1() { return twin->vertex; }
        inline bool is_finite() {
            return vertex != nullptr && twin->vertex != nullptr;
        }
        
        // Iterators around vertex
        HalfEdgePtr vertexNextCCW();
        HalfEdgePtr vertexNextCW();
    };
    
    
    std::pair<HalfEdgePtr, HalfEdgePtr> make_twins(int left_index, int right_index);
    
    
    std::pair<HalfEdgePtr, HalfEdgePtr> make_twins(const std::pair<int,int> &indices);
    
    
    void connect_halfedges(HalfEdgePtr p1, HalfEdgePtr p2);
    
}


#endif /* DCEL_h */

