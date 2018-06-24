//
//  DCEL.cpp
//  FortuneAlgo
//
//  Created by Dmytro Kotsur on 06/05/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#include "DCEL.hpp"




namespace DCEL {
    
   
    Vertex::Vertex(const Point2D &pos, HalfEdgePtr incident_edge) : point(pos), edge(incident_edge)  { }
    
    
    HalfEdge::HalfEdge(int _l_index, int _r_index, VertexPtr _vertex) :
        l_index(_l_index), r_index(_r_index), vertex(_vertex) {}
    
    
    HalfEdgePtr HalfEdge::vertexNextCCW() {
        return twin->prev;
    }
    
    
    HalfEdgePtr HalfEdge::vertexNextCW() {
        return next->twin;
    }
    
    
    std::pair<HalfEdgePtr, HalfEdgePtr> make_twins(int left_index, int right_index) {
        
        HalfEdgePtr h = std::make_shared<HalfEdge>(left_index, right_index);
        HalfEdgePtr h_twin = std::make_shared<HalfEdge>(right_index, left_index);
        
        h->twin = h_twin;
        h_twin->twin = h;
        
        return std::make_pair(h, h_twin);
    }
    
    
    std::pair<HalfEdgePtr, HalfEdgePtr> make_twins(const std::pair<int,int> &indices) {
        
        return make_twins(indices.first, indices.second);
    }
    
    
    void connect_halfedges(HalfEdgePtr p1, HalfEdgePtr p2) {
        p1->next = p2;
        p2->prev = p1;
    }
    
}
