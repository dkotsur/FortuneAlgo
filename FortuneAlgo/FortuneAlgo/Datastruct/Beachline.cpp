//
//  Beachline.cpp
//  FortuneAlgo
//
//  Created by Dmytro Kotsur on 06/05/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#include "Beachline.hpp"

namespace beachline {

    
    BLNode::BLNode(const std::pair<int,int>& _indices,
                   double* _sweepline ,
                   const std::vector<Point2D>* _points,
                   BLNodePtr _left,
                   BLNodePtr _right,
                   BLNodePtr _parent,
                   int _height) : indices(_indices), left(_left), right(_right),
                                  parent(_parent), height(_height),
                                  sweepline(_sweepline), points(_points),
                                  next(nullptr), prev(nullptr) {}


    double BLNode::value() {
        if (points == nullptr)
            return std::numeric_limits<double>::infinity();
        if (is_leaf()) {
            return (*points)[indices.first].x;
        } else {
            Point2D p1 = (*points)[indices.first], p2 = (*points)[indices.second];
            
            std::vector<Point2D> ips = findIntersectionPoints(p1, p2, *sweepline);
            if (ips.size() == 2) {
                if (p1.y < p2.y) {
                    return ips[0].x;
                } else {
                    return ips[1].x;
                }
            } else {
                return ips[0].x;
            }
        }
    }

    /**
     Connect as a list
     */
    void connect(BLNodePtr prev, BLNodePtr next) {
        prev->next = next;
        next->prev = prev;
    }


    /**
     Check if the node is a root node
     */
    bool is_root(BLNodePtr node) {
        return node->parent == nullptr;
    }


    /**
     Get height of the node
     */
    int get_height(BLNodePtr node) {
        if (node == nullptr) return 0;
        return node->height;
    }


    /**
     Update height of the node
     */
    void update_height(BLNodePtr node) {
        if (node == nullptr)
            return;
        node->height = std::max(get_height(node->left), get_height(node->right)) + 1;
    }


    /**
     Get balance of the node (difference between the height of left and right subtrees)
     */
    int get_balance(BLNodePtr node) {
        return get_height(node->left) - get_height(node->right);
    }


    /**
     Performs rotation of a tree around `node` such that it goes to the left subtree
     */
    BLNodePtr rotate_left(BLNodePtr node) {
        
        if (node == nullptr)
            return nullptr;
        
        if (node->right == nullptr)
            return node;
        
        // get right node, which becomes a new root node
        BLNodePtr rnode = node->right;
        
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
    BLNodePtr rotate_right(BLNodePtr node) {
        
        if (node == nullptr)
            return nullptr;
        
        if (node->left == nullptr)
            return node;
        
        // left node becomes root node of subtree
        BLNodePtr lnode = node->left;
        
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
    BLNodePtr find(BLNodePtr root, double x) {
        if (root == nullptr) {
            return nullptr;
        }
        BLNodePtr node = root;
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
    BLNodePtr replace(BLNodePtr node, BLNodePtr new_node) {
        
        if (node == nullptr) {
            return new_node;
        }
        
        // Find x-coordinate
        double x = new_node->value();
        
        // Get a parent node
        BLNodePtr parent_node = node->parent;
        
        // Remove leaf, because it's replaced by a new subtree
        //        delete node;
        
        // Insert the node
        new_node->parent = parent_node;
        if (parent_node != nullptr) {
            if (parent_node->value() < x) {
                parent_node->right = new_node;
            } else {
                parent_node->left = new_node;
            }
        }
        
        // Rebalance the tree
        node = new_node;
        while (parent_node != nullptr) {
            update_height(parent_node);
            int balance = get_balance(parent_node);
            if (balance > 1) { // left subtree is higher than right subtree by more than 1
                if (parent_node->left != nullptr && !parent_node->left->is_leaf() && get_balance(parent_node->left) < 0) { // @TODO ensure that
                    parent_node->left = rotate_left(parent_node->left);
                }
                parent_node = rotate_right(parent_node);
            } else if (balance < -1) { // right subtree is lower than left subtree by more than 1
                if (parent_node->right != nullptr && !parent_node->right->is_leaf() && get_balance(parent_node->right) > 0) {
                    parent_node->right = rotate_right(parent_node->right);
                }
                parent_node = rotate_left(parent_node);
            }
            
            //_validate(parent_node);
            
            node = parent_node;
            parent_node = parent_node->parent;
        }
        
        //_check_balance(node);
        
        return node;
    }


    /**
     Remove a disappearing arc related to a circle event.
     The function rebalances the tree and returns the pointer to a new root node.
     */
    BLNodePtr remove(BLNodePtr leaf) {
        
        // General idea behind this code:
        // This function removes the leaf and it's parent corresponding to one breakpoint.
        // It moves up in a tree and rebalaces it. If function encounters second breakpoint,
        // it replaces this breakpoint with a new one. This is possible because when the circle
        // event appears, two breakpoints coincide and thus they should be represented by one.
        
        if (leaf == nullptr)
            return nullptr;
        
        BLNodePtr parent = leaf->parent, grandparent = parent->parent;
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
        
        BLNodePtr other_subtree;
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
        
        BLNodePtr new_root = grandparent;
        // Go up and rebalance the whole tree
        while (grandparent != nullptr) {
            if (grandparent->has_indices(other_bp))
                grandparent->indices = std::make_pair(leaf->prev->get_id(), leaf->next->get_id());
            // update height of a node
            update_height(grandparent);
            // calculate balance factor of a node
            int balance = get_balance(grandparent);
            if (balance > 1) { // left subtree is higher than right subtree by more than 1
                if (grandparent->left != nullptr && !grandparent->left->is_leaf() && get_balance(grandparent->left) < 0) {
                    grandparent->left = rotate_left(grandparent->left);
                }
                grandparent = rotate_right(grandparent);
            } else if (balance < -1) { // right subtree is lower than left subtree by more than 1
                if (grandparent->right != nullptr && !grandparent->right->is_leaf() && get_balance(grandparent->right) > 0) {
                    grandparent->right = rotate_right(grandparent->right);
                }
                grandparent = rotate_left(grandparent);
            }
            
            //_validate(grandparent);
            
            new_root = grandparent;
            grandparent = grandparent->parent;
        }
        
        // Connect previous with next leaf
        connect(leaf->prev, leaf->next);
        
        //_check_balance(new_root);
        
        return new_root;
    }


    /**
     Returns breakpoints for a given arc
     */
    std::pair<BLNodePtr, BLNodePtr> breakpoints(BLNodePtr leaf) {
        
        if (leaf == nullptr || leaf->next == nullptr || leaf->prev == nullptr)
            return std::make_pair<BLNodePtr>(nullptr, nullptr);
        
        BLNodePtr parent = leaf->parent, gparent = leaf->parent;
        std::pair<int,int> bp1(leaf->prev->get_id(), leaf->get_id()); // left breakpoint
        std::pair<int,int> bp2(leaf->get_id(), leaf->next->get_id()); // right breakpoint
        std::pair<int,int> other_bp;
        
        bool left_is_missing = true;
        
        if (parent->has_indices(bp1)) {
            other_bp = bp2;
            left_is_missing = false;
        } else if (parent->has_indices(bp2)) {
            other_bp = bp1;
            left_is_missing = true;
        }
        
        // Go up and rebalance the whole tree
        while (gparent != nullptr) {
            if (gparent->has_indices(other_bp)) {
                break;
            }
            gparent = gparent->parent;
        }
        
        if (left_is_missing) {
            return std::make_pair(gparent, parent);
        } else {
            return std::make_pair(parent, gparent);
        }
        
//        // BUG doesn't take into account gparent WRONG!!!
//        if (parent->parent != nullptr) {
//            if (parent->parent->left == parent) {
//                return std::make_pair(parent, gparent);
//            } else {
//                return std::make_pair(gparent, parent);
//            }
//        }
//
//        return std::make_pair(parent, gparent);
    }


    BLNodePtr make_subtree(int index, int index_behind, double *sweepline,
                           const std::vector<Point2D> *points,
                           std::vector<HalfEdgePtr> &edges) {
        
        // create nodes corresponding to branching points
        BLNodePtr node1 = std::make_shared<BLNode>(std::make_pair(index_behind, index), sweepline, points);
        BLNodePtr node2 = std::make_shared<BLNode>(std::make_pair(index, index_behind), sweepline, points);
        
        // create leaf nodes
        BLNodePtr leaf1 = std::make_shared<BLNode>(std::make_pair(index_behind, index_behind), sweepline, points);
        BLNodePtr leaf2 = std::make_shared<BLNode>(std::make_pair(index, index), sweepline, points);
        BLNodePtr leaf3 = std::make_shared<BLNode>(std::make_pair(index_behind, index_behind), sweepline, points);
        
        // adjust tree connections
        node1->right = node2;
        node2->parent = node1;
        
        node1->left = leaf1;
        leaf1->parent = node1;
        
        node2->left = leaf2;
        leaf2->parent = node2;
        
        node2->right = leaf3;
        leaf3->parent = node2;
        
        // add halfedges
        std::pair<HalfEdgePtr, HalfEdgePtr> twin_edges = make_twins(index_behind, index);
        node1->edge = twin_edges.first;//second;//first;
        node2->edge = twin_edges.second;//first;//second;
        
        edges.push_back(twin_edges.first);
        edges.push_back(twin_edges.second);
        
        // connect leaf nodes
        connect(leaf1, leaf2);
        connect(leaf2, leaf3);
        
        // reset height of a node
        update_height(node2);
        update_height(node1);
        
        // return the result
        return node1;
    }


    BLNodePtr make_simple_subtree(int index, int index_behind, double *sweepline,
                                  const std::vector<Point2D> *points,
                                  std::vector<HalfEdgePtr> &edges) {
        
        BLNodePtr node, leaf_l, leaf_r;
        
        std::pair<HalfEdgePtr, HalfEdgePtr> twin_edges = make_twins(index_behind, index);
        
        edges.push_back(twin_edges.first);
        edges.push_back(twin_edges.second);
        
        if ((*points)[index].x < (*points)[index_behind].x) {
            // Depends on the point order
            node = std::make_shared<BLNode>(std::make_pair(index, index_behind), sweepline, points);
            leaf_l = std::make_shared<BLNode>(std::make_pair(index, index), sweepline, points);
            leaf_r = std::make_shared<BLNode>(std::make_pair(index_behind, index_behind), sweepline, points);
            node->edge = twin_edges.second;//twin_edges.first;
        } else {
            node = std::make_shared<BLNode>(std::make_pair(index_behind, index), sweepline, points);
            leaf_l = std::make_shared<BLNode>(std::make_pair(index_behind, index_behind), sweepline, points);
            leaf_r = std::make_shared<BLNode>(std::make_pair(index, index), sweepline, points);
            node->edge = twin_edges.first;//twin_edges.second;
        }
        
        node->left = leaf_l;
        node->right = leaf_r;
        
        leaf_l->parent = node;
        leaf_r->parent = node;
        
        connect(leaf_l, leaf_r);
        update_height(node);
        
        return node;
    }


    bool _validate(BLNodePtr node) {
        
        if (node == nullptr)
            return true;
        
        if (node->is_leaf()) {
            if (node->left != nullptr || node->right != nullptr) {
                std::cout << "LEAF NOT A LEAF: " << node->indices.first << ", " << node->indices.second << std::endl;
                return false;
            }
        } else {
            if (node->left == nullptr || node->right == nullptr) {
                std::cout << " BP WITHOUT LEAF: " << node->indices.first << ", " << node->indices.second << std::endl;
                return false;
            }
        }
        return true;
    }

    bool _check_balance(BLNodePtr node) {
        if (node == nullptr) return true;
        if (_check_balance(node->left) && _check_balance(node->right)) {
            if (fabs(get_balance(node)) > 1) {
                
                std::cout << "+unbalanced (" << node->indices.first << ", " << node->indices.second << ")" << std::endl;
                
                return false;
            }
        }
        return true;
    }


    /**
     Print tree
     */
    void print_tree(BLNodePtr root, int width) {
        
        if (root == nullptr)
            return;
        
        std::vector<std::vector<BLNodePtr>> layers(root->height);
        
        layers[0].push_back(root);
        int size = 2;
        for (int i = 1; i < root->height; ++i) {
            layers[i].resize(size);
            for (int j = 0; j < layers[i-1].size(); ++j) {
                if (layers[i-1][j] != nullptr) {
                    layers[i][2*j] = layers[i-1][j]->left;
                    layers[i][2*j+1] = layers[i-1][j]->right;
                }
            }
            size *= 2;
        }
        
        size /= 2;
        for (int i = 0; i < root->height; ++i) {
            for (int j = 0; j < layers[i].size(); ++j) {
                if (layers[i][j] != nullptr)
                    std::cout << std::setw(width * size) << "<" << layers[i][j]->indices.first << ", " << layers[i][j]->indices.second << ">";
                else
                    std::cout << std::setw(width * size) << "      ";
            }
            std::cout << std::endl;
            size /= 2;
        }
    }

}


