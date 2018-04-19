//
//  main.cpp
//  AVLTree
//
//  Created by Dmytro Kotsur on 19/04/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#include <iostream>


template<class T>
class AVLNode {
public:
    
    T data;
    size_t height;
    AVLNode *left, *right, *parent;
    
    AVLNode(T _data, AVLNode* _left = nullptr, AVLNode* _right = nullptr, AVLNode* _parent = nullptr, size_t _height = 0) :
        data(_data), left(_left), right(_right), parent(_parent), height(_height) {}
};


/**
 Check if the node is leaf node
 */
template<class T>
bool is_leaf(AVLNode<T> *node) {
    return node->right == nullptr && node->left == nullptr;
}


/**
 Check if the node is root node
 */
template<class T>
bool is_root(AVLNode<T> *node) {
    return node->parent == nullptr;
}


/**
 Get height of a node
 */
template<class T>
size_t get_height(AVLNode<T> *node) {
    if (node == nullptr) return 0;
    return node->height;
}


/**
 Update height of a node
 */
template<class T>
void update_height(AVLNode<T> *node) {
    if (node == nullptr)
        return;
    node->height = std::max(get_height(node->left), get_height(node->right)) + 1;
}


/**
 Performs rotation of a tree around `node` such that it goes to the left subtree
 */
template<class T>
void rotate_left(AVLNode<T>* &node) {
    
    if (node == nullptr)
        return;
    
    
    
    
    
}

/**
 Performs rotation of a tree around `node` such that it goes to the right subtree
 */
template<class T>
void rotate_right(AVLNode<T>* &node) {
    
    if (node == nullptr || node->left == nullptr)
        return;
    
    // left node becomes root node of subtree
    AVLNode<T> *lnode = node->left;
    
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
    
    node = lnode;
}

/**
 Insert new node into AVL tree
 */
template<class T>
void insert(AVLNode<T>* root, AVLNode<T> *new_node) {
    
}



int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
