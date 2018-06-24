//
//  Beachline.h
//  FortuneAlgo
//
//  Created by Dmytro Kotsur on 20/04/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#ifndef Beachline_hpp
#define Beachline_hpp


#include <time.h>
#include <iostream>
#include <limits>
#include <iomanip>
#include <vector>
#include <random>
#include <cassert>

#include "Parabola.hpp"
#include "DCEL.hpp"


class Event;


namespace beachline {
    
    using namespace DCEL;
    
    class BLNode;
    typedef std::shared_ptr<BLNode> BLNodePtr;

    class BLNode {
    public:
        
        // Height of the tree
        int height;
        
        // Pointer to a position of a sweepline
        double *sweepline;
        
        // Pointer to a vector of input points
        const std::vector<Point2D> *points;
        
        // Indices of the points
        std::pair<int, int> indices;
        
        // Pointers to left, right children and parent node
        BLNodePtr left, right, parent;
        
        // Pointer to a circle event for a leaf node or halfedge for an internal node
        std::shared_ptr<Event> circle_event;
        std::shared_ptr<HalfEdge> edge;
        
        // Constructor
        BLNode(const std::pair<int,int>& _indices,
               double* _sweepline = nullptr,
               const std::vector<Point2D>* _points = nullptr,
               BLNodePtr _left = nullptr,
               BLNodePtr _right = nullptr,
               BLNodePtr _parent = nullptr,
               int _height = 1);
        
        // Pointers to a next and previous arc-nodes
        BLNodePtr next, prev;
        
        // Leaf is defined as <p_i,p_i>
        inline bool is_leaf() {
            return indices.first == indices.second;
        }
        
        // Returns id of the node (only for leafs)
        inline int get_id() {
            return indices.first;
        }
        
        // Check if indices are equal
        inline bool has_indices(int a, int b) {
            return indices.first == a && indices.second == b;
        }
        
        // Check if indices are equal
        inline bool has_indices(const std::pair<int,int> &p) {
            return indices.first == p.first && indices.second == p.second;
        }
        
        // Return x-coordinate of:
        //  - in case of leaf node - corresponding focus of parabola;
        //  - in case of internal node - breakpoint;
        double value();
        
    };
    
    
    /**
     Connect as a list
     */
    void connect(BLNodePtr prev, BLNodePtr next);


    /**
     Check if the node is a root node
     */
    bool is_root(BLNodePtr node);


    /**
     Get height of the node
     */
    int get_height(BLNodePtr node);


    /**
     Update height of the node
     */
    void update_height(BLNodePtr node);


    /**
     Get balance of the node (difference between the height of left and right subtrees)
     */
    int get_balance(BLNodePtr node);


    /**
     Performs rotation of a tree around `node` such that it goes to the left subtree
     */
    BLNodePtr rotate_left(BLNodePtr node);


    /**
     Performs rotation of a tree around `node` such that it goes to the right subtree
     */
    BLNodePtr rotate_right(BLNodePtr node);


    /**
     Find a leaf in a tree such that x is under the parabolic arc,
     which corresponds to this leaf.
     */
    BLNodePtr find(BLNodePtr root, double x);
    

    /**
     Replace a leaf `node` with a new subtree, which has root `new_node`.
     The function rebalances the tree and returns the pointer to a new root node.
     */
    BLNodePtr replace(BLNodePtr node, BLNodePtr new_node);
    
    
    /**
     Remove a disappearing arc related to a circle event.
     The function rebalances the tree and returns the pointer to a new root node.
     */
    BLNodePtr remove(BLNodePtr leaf);;
    
    
    /**
     Returns breakpoints for a given arc
     */
    std::pair<BLNodePtr, BLNodePtr> breakpoints(BLNodePtr leaf);
    
    
    BLNodePtr make_subtree(int index, int index_behind, double *sweepline,
                           const std::vector<Point2D> *points,
                           std::vector<HalfEdgePtr> &edges);
    
    
    BLNodePtr make_simple_subtree(int index, int index_behind, double *sweepline,
                                  const std::vector<Point2D> *points,
                                  std::vector<HalfEdgePtr> &edges);
    
    
    bool _validate(BLNodePtr node);
    
    
    bool _check_balance(BLNodePtr node);


    /**
     Print tree
     */
    void print_tree(BLNodePtr root, int width = 7);
    
}


#endif /* Beachline_hpp */
