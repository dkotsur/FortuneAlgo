//
//  main.cpp
//  AVLTree
//
//  Created by Dmytro Kotsur on 19/04/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>



template<class T>
class AVLNode {
public:
    
    T data;
    int height;
    AVLNode *left, *right, *parent;
    
    AVLNode(T _data, AVLNode<T>* _left = nullptr, AVLNode<T>* _right = nullptr, AVLNode<T>* _parent = nullptr, int _height = 1) :
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
int get_height(AVLNode<T> *node) {
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
    
    if (node == nullptr || node->right == nullptr)
        return;
    
    // get right node, which becomes a new root node
    AVLNode<T> *rnode = node->right;
    
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
    
    node = rnode;
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
void insert(AVLNode<T>* &root, AVLNode<T> *new_node) {
    
    if (root == nullptr) {
        root = new_node;
        return;
    }
    
    // Find a place where insert a new node
    AVLNode<T> *node = root, *parent_node = nullptr;
    while (node != nullptr) {
        parent_node = node;
        if (node->data > new_node->data) { // go to left subtree
            node = node->left;
        } else {
            node = node->right;
        }
    }
    
    // Insert a node
    new_node->parent = parent_node;
    if (parent_node->data > new_node->data) {
        parent_node->left = new_node;
    } else {
        parent_node->right = new_node;
    }
    
    node = nullptr;
    while (parent_node != nullptr) {
        update_height(parent_node);
        int balance = get_height(parent_node->left) - get_height(parent_node->right);
        if (balance > 1) { // left subtree is higher than right subtree by more than 1
            if (parent_node->left != nullptr && new_node->data > parent_node->left->data) {
                rotate_left(parent_node->left);
            }
            rotate_right(parent_node);
        } else if (balance < -1) { // right subtree is lower than left subtree by more than 1
            if (parent_node->right != nullptr && new_node->data < parent_node->right->data) {
                rotate_right(parent_node->right);
            }
            rotate_left(parent_node);
        }
        node = parent_node;
        parent_node = parent_node->parent;
    }
    
    root = node;
}


/**
 Print tree
 */
template<class T>
void print_tree(AVLNode<T> *root, int width = 3) {

    std::vector<std::vector<AVLNode<T>*>> layers(root->height);
    
    layers[0].push_back(root);
    int size = 2;
    for (int i = 1; i < root->height; ++i) {
        layers[i].resize(size);
        for (int j = 0; j < layers[i-1].size(); ++j) {
            layers[i][2*j] = layers[i-1][j]->left;
            layers[i][2*j+1] = layers[i-1][j]->right;
        }
        size *= 2;
    }
    
    size /= 2;
    for (int i = 0; i < root->height; ++i) {
        for (int j = 0; j < layers[i].size(); ++j) {
            if (layers[i][j] != nullptr)
                std::cout << std::setw(width * size) << layers[i][j]->data;
            else
                std::cout << std::setw(width * size) << "  ";
        }
        std::cout << std::endl;
        size /= 2;
    }
}



int main(int argc, const char * argv[]) {
    
    AVLNode<int> *root = nullptr;//new AVLNode<int>(5);
    
    for (int i = 1; i < 20; ++i) {
        insert(root, new AVLNode<int>(i));
       // insert(root, new AVLNode<int>(2));
    }

    print_tree(root);
}
