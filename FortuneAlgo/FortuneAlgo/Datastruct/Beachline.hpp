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


namespace beachline {

    class BLNode {
    public:
        
        // Height of the tree
        int height;
        
        // Pointer to a position of a sweepline
        double *sweepline;
        
        // Pointer to a vector of input points
        std::vector<Point2D> *points;
        
        // Indices of the points
        std::pair<int, int> indices;
        
        // Pointers to left, right children and parent node
        BLNode *left, *right, *parent;
        
        // Pointer to a circle event
        void *circle_event;
        
        // Constructor
        BLNode(const std::pair<int,int>& _indices,
               double* _sweepline = nullptr,
               std::vector<Point2D>* _points = nullptr,
               BLNode* _left = nullptr,
               BLNode* _right = nullptr,
               BLNode* _parent = nullptr,
               int _height = 1) : indices(_indices), left(_left), right(_right),
                                  parent(_parent), height(_height),
                                  sweepline(_sweepline), points(_points),
                                  next(nullptr), prev(nullptr), circle_event(nullptr) {}
        
        // Pointers to a next and previous arc-nodes
        BLNode *next, *prev;
        
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
        double value() {
            if (points == nullptr)
                return std::numeric_limits<double>::infinity();
            if (is_leaf()) {
                return (*points)[indices.first].x;
            } else {
                Point2D p1 = (*points)[indices.first], p2 = (*points)[indices.second];
                
                std::vector<Point2D> ips = findIntersectionPoints(p1, p2, *sweepline);
                if (ips.size() == 2) {
                    if (p1.x < p2.x) {
                        return ips[0].x;
                    } else {
                        return ips[1].x;
                    }
                } else {
                    return ips[0].x;
                }
            }
        }
    };
    
    
    /**
     Connect as a list
     */
    void connect(BLNode *prev, BLNode *next) {
        prev->next = next;
        next->prev = prev;
    }


    /**
     Check if the node is a root node
     */
    bool is_root(BLNode *node) {
        return node->parent == nullptr;
    }


    /**
     Get height of the node
     */
    int get_height(BLNode *node) {
        if (node == nullptr) return 0;
        return node->height;
    }


    /**
     Update height of the node
     */
    void update_height(BLNode *node) {
        if (node == nullptr)
            return;
        node->height = std::max(get_height(node->left), get_height(node->right)) + 1;
    }


    /**
     Get balance of the node (difference between the height of left and right subtrees)
     */
    int get_balance(BLNode *node) {
        return get_height(node->left) - get_height(node->right);
    }


    /**
     Performs rotation of a tree around `node` such that it goes to the left subtree
     */
    BLNode* rotate_left(BLNode* node) {
        
        if (node == nullptr || node->right == nullptr)
            return nullptr;
        
        // get right node, which becomes a new root node
        BLNode *rnode = node->right;
        
        // establish connections with a root node if threre is one
        if (!is_root(node)) {
            if (node->parent->left == node) {
                node->parent->left = rnode;
            } else {
                node->parent->right = rnode;
            }
        }
        rnode->parent = node->parent;
        
        // connect right subtree of the left child as a left subtree of `node`
        node->right = rnode->left;
        if (rnode->left != nullptr) {
            rnode->left->parent = node;
        }
        
        // connect `node` as a right child of it's child
        rnode->left = node;
        node->parent = rnode;
        
        // update height attribute
        update_height(node);
        update_height(rnode);
        update_height(rnode->parent);
        
        return rnode;
    }


    /**
     Performs rotation of a tree around `node` such that it goes to the right subtree
     */
    BLNode* rotate_right(BLNode* node) {
        
        if (node == nullptr || node->left == nullptr)
            return nullptr;
        
        // left node becomes root node of subtree
        BLNode *lnode = node->left;
        
        // establish connections with a root node if threre is one
        if (!is_root(node)) {
            if (node->parent->left == node) {
                node->parent->left = lnode;
            } else {
                node->parent->right = lnode;
            }
        }
        lnode->parent = node->parent;
        
        // connect right subtree of the left child as a left subtree of `node`
        node->left = lnode->right;
        if (lnode->right != nullptr) {
            lnode->right->parent = node;
        }
        
        // connect `node` as a right child of it's child
        lnode->right = node;
        node->parent = lnode;
        
        // update height attribute
        update_height(node);
        update_height(lnode);
        update_height(lnode->parent);
        
        return lnode;
    }


    /**
     Find a leaf in a tree such that x is under the parabolic arc,
     which corresponds to this leaf.
     */
    BLNode *find(BLNode *root, double x) {
        if (root == nullptr) {
            return nullptr;
        }
        BLNode *node = root;
        while (!node->is_leaf()) {
            if (node->value() < x) {
                node = node->right;
            } else {
                node = node->left;
            }
        }
        return node;
    }


    /**
     Replace a leaf `node` with a new subtree, which has root `new_node`.
     The function rebalances the tree and returns the pointer to a new root node.
     */
    BLNode* replace(BLNode* node, BLNode *new_node) {
        
        if (node == nullptr) {
            return new_node;
        }
        
        // Find x-coordinate
        double x = new_node->value();
        
        // Get a parent node
        BLNode *parent_node = node->parent;
        
        // Remove leaf, because it's replaced by a new subtree
        delete node;
        
        // Insert the node
        new_node->parent = parent_node;
        if (parent_node != nullptr) {
            if (parent_node->value() > x) {
                parent_node->left = new_node;
            } else {
                parent_node->right = new_node;
            }
        }
        
        // Rebalance the tree
        node = new_node;
        while (parent_node != nullptr) {
            update_height(parent_node);
            int balance = get_balance(parent_node);
            if (balance > 1) { // left subtree is higher than right subtree by more than 1
                if (parent_node->left != nullptr && x > parent_node->value()) { // @TODO ensure that
                    parent_node->left = rotate_left(parent_node->left);
                }
                parent_node = rotate_right(parent_node);
            } else if (balance < -1) { // right subtree is lower than left subtree by more than 1
                if (parent_node->right != nullptr && x < parent_node->value()) {
                    parent_node->right = rotate_right(parent_node->right);
                }
                parent_node = rotate_left(parent_node);
            }
            node = parent_node;
            parent_node = parent_node->parent;
        }
        return node;
    }
    
    
    /**
     Remove a disappearing arc related to a circle event.
     The function rebalances the tree and returns the pointer to a new root node.
     */
    BLNode* remove(BLNode* &leaf) {
        
        // General idea behind this code:
        // This function removes the leaf and it's parent corresponding to one breakpoint.
        // It moves up in a tree and rebalaces it. If function encounters second breakpoint,
        // it replaces this breakpoint with a new one. This is possible because when the circle
        // event appears, two breakpoints coincide and thus they should be represented by one.
        
        if (leaf == nullptr)
            return nullptr;
        
        BLNode *parent = leaf->parent, *grandparent = parent->parent;
        std::pair<int,int> bp1(leaf->prev->get_id(), leaf->get_id());
        std::pair<int,int> bp2(leaf->get_id(), leaf->next->get_id());
        std::pair<int,int> other_bp;
        
        assert(leaf->next != nullptr);
        assert(leaf->prev != nullptr);
        assert(parent != nullptr);
        assert(grandparent != nullptr);
        
        assert(parent->has_indices(bp1) || parent->has_indices(bp2));
        
        if (parent->has_indices(bp1)) {
            other_bp = bp2;
        } else if (parent->has_indices(bp2)) {
            other_bp = bp1;
        }
        
        BLNode *other_subtree = nullptr;
        if (parent->left == leaf)
            other_subtree = parent->right;
        else
            other_subtree = parent->left;
        
        other_subtree->parent = grandparent;
        if (grandparent->left == parent) {
            grandparent->left = other_subtree;
        } else {
            grandparent->right = other_subtree;
        }
        
        BLNode *new_root = grandparent;
        // Go up and rebalance the whole tree
        while (grandparent != nullptr) {
            if (grandparent->has_indices(other_bp))
                grandparent->indices = std::make_pair(leaf->prev->get_id(), leaf->next->get_id());
            // update height of a node
            update_height(grandparent);
            // calculate balance factor of a node
            int balance = get_balance(grandparent);
            if (balance > 1) { // left subtree is higher than right subtree by more than 1
                if (grandparent->left != nullptr && get_balance(grandparent->left) < 0) {
                    grandparent->left = rotate_left(grandparent->left);
                }
                grandparent = rotate_right(grandparent);
            } else if (balance < -1) { // right subtree is lower than left subtree by more than 1
                if (grandparent->right != nullptr && get_balance(grandparent->right) > 0) {
                    grandparent->right = rotate_right(grandparent->right);
                }
                grandparent = rotate_left(grandparent);
            }
            new_root = grandparent;
            grandparent = grandparent->parent;
        }
        
        // Connect previous with next leaf
        connect(leaf->prev, leaf->next);
        
        delete leaf;
        delete parent;
        
        leaf = nullptr;
        
        return new_root;
    }
    
    
    BLNode* make_subtree(int index, int index_behind, double *sweepline,
                         std::vector<Point2D> *points) {
        
        // create nodes corresponding to branching points
        BLNode *node1 = new BLNode(std::make_pair(index_behind, index), sweepline, points);
        BLNode *node2 = new BLNode(std::make_pair(index, index_behind), sweepline, points);
        
        // create leaf nodes
        BLNode *leaf1 = new BLNode(std::make_pair(index_behind, index_behind), sweepline, points);
        BLNode *leaf2 = new BLNode(std::make_pair(index, index), sweepline, points);
        BLNode *leaf3 = new BLNode(std::make_pair(index_behind, index_behind), sweepline, points);
        
        // adjust connections
        node1->right = node2;
        node2->parent = node1;
        
        node1->left = leaf1;
        leaf1->parent = node1;
        
        node2->left = leaf2;
        leaf2->parent = node2;
        
        node2->right = leaf3;
        leaf3->parent = node2;
        
        connect(leaf1, leaf2);
        connect(leaf2, leaf3);
        
        // reset height of a node
        update_height(node2);
        update_height(node1);
        
        // return the result
        return node1;
    }
    
    
    BLNode *make_simple_subtree(int index, int index_behind, double *sweepline,
                                std::vector<Point2D> *points) {
        
        BLNode *node, *leaf1, *leaf2;
        
        if ((*points)[index].x < (*points)[index_behind].x) {
            node = new BLNode(std::make_pair(index, index_behind), sweepline, points);
            leaf1 = new BLNode(std::make_pair(index, index), sweepline, points);
            leaf2 = new BLNode(std::make_pair(index_behind, index_behind), sweepline, points);
        } else {
            node = new BLNode(std::make_pair(index_behind, index), sweepline, points);
            leaf1 = new BLNode(std::make_pair(index_behind, index_behind), sweepline, points);
            leaf2 = new BLNode(std::make_pair(index, index), sweepline, points);
        }
        
        node->left = leaf1;
        node->right = leaf2;
        leaf1->parent = node;
        leaf2->parent = node;
        
        connect(leaf1, leaf2);
        update_height(node);
        
        return node;
    }
    
    
    
    
    
    

//    /**
//     Delete node from tree
//     */
//    template<class T>
//    void remove(BLNode<T>* &root, BLNode<T> *node) {
//
//        BLNode<T> *update_node = nullptr;
//
//        if (node->left == nullptr && node->right == nullptr) {
//            if (node->parent == nullptr) {
//                root = nullptr;
//                return;
//            } else {
//                if (node->parent->left == node)
//                    node->parent->left = nullptr;
//                if (node->parent->right == node)
//                    node->parent->right = nullptr;
//                update_node = node->parent;
//            }
//        } else if (node->left == nullptr) {
//            if (node->parent != nullptr) {
//                if (node->parent->left == node)
//                    node->parent->left = node->right;
//                if (node->parent->right == node)
//                    node->parent->right = node->right;
//            }
//            node->right->parent = node->parent;
//            update_node = node->right;
//        } else if (node->right == nullptr) {
//            if (node->parent != nullptr) {
//                if (node->parent->left == node)
//                    node->parent->left = node->left;
//                if (node->parent->right == node)
//                    node->parent->right = node->left;
//            }
//            node->left->parent = node->parent;
//            update_node = node->left;
//        } else {
//
//            // find a node with minimal key in the right subtree of `node`
//            BLNode<T> *min_node = node->right;
//            while (min_node->left != nullptr) {
//                min_node = min_node->left;
//            }
//
//            update_node = min_node;
//
//            // connect right subtree of `min_node` with the parent of `min_node`
//            if (min_node->parent->left == min_node)
//                min_node->parent->left = min_node->right;
//            if (min_node->parent->right == min_node)
//                min_node->parent->right = min_node->right;
//            if (min_node->right != nullptr)
//                min_node->right->parent = min_node->parent;
//
//            // insert `min_node` instead of `node`
//            min_node->parent = node->parent;
//            if (node->parent != nullptr) {
//                if (node->parent->left == node)
//                    node->parent->left = min_node;
//                if (node->parent->right == node)
//                    node->parent->right = min_node;
//            }
//
//            if (node->left)
//                node->left->parent = min_node;
//
//            if (node->right)
//                node->right->parent = min_node;
//
//            min_node->left = node->left;
//            min_node->right = node->right;
//        }
//
//        // update heights of the `min_node_parent` and rebalance if necessary
//        BLNode<T> *new_root = nullptr;
//        while (update_node != nullptr) {
//            // update height of a node
//            update_height(update_node);
//            // calculate balance factor of a node
//            int balance = get_balance(update_node);
//            if (balance > 1) { // left subtree is higher than right subtree by more than 1
//                if (update_node->left != nullptr && get_balance(update_node->left) < 0) {
//                    update_node->left = rotate_left(update_node->left);
//                }
//                update_node = rotate_right(update_node);
//            } else if (balance < -1) { // right subtree is lower than left subtree by more than 1
//                if (update_node->right != nullptr && get_balance(update_node->right) > 0) {
//                    update_node->right = rotate_right(update_node->right);
//                }
//                update_node = rotate_left(update_node);
//            }
//            new_root = update_node;
//            update_node = update_node->parent;
//        }
//        root = new_root;
//    }


//    /**
//     Print tree
//     */
//    template<class T>
//    void print_tree(BLNode<T> *root, int width = 5) {
//
//        if (root == nullptr)
//            return;
//
//        std::vector<std::vector<BLNode<T>*>> layers(root->height);
//
//        layers[0].push_back(root);
//        int size = 2;
//        for (int i = 1; i < root->height; ++i) {
//            layers[i].resize(size);
//            for (int j = 0; j < layers[i-1].size(); ++j) {
//                if (layers[i-1][j] != nullptr) {
//                    layers[i][2*j] = layers[i-1][j]->left;
//                    layers[i][2*j+1] = layers[i-1][j]->right;
//                }
//            }
//            size *= 2;
//        }
//
//        size /= 2;
//        for (int i = 0; i < root->height; ++i) {
//            for (int j = 0; j < layers[i].size(); ++j) {
//                if (layers[i][j] != nullptr)
//                    std::cout << std::setw(width * size) << layers[i][j]->data;
//                else
//                    std::cout << std::setw(width * size) << "  ";
//            }
//            std::cout << std::endl;
//            size /= 2;
//        }
//    }
    
}


#endif /* Beachline_hpp */
