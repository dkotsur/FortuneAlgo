//
//  main.cpp
//  AVLTree
//
//  Created by Dmytro Kotsur on 19/04/2018.
//  Copyright © 2018 Dmytro Kotsur. All rights reserved.
//

#include <time.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>



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
 Get balance of a node (difference between the height of left and right subtrees)
 */
template<class T>
int get_balance(AVLNode<T> *node) {
    return get_height(node->left) - get_height(node->right);
}


/**
 Performs rotation of a tree around `node` such that it goes to the left subtree
 */
template<class T>
AVLNode<T>* rotate_left(AVLNode<T>* node) {
    
    if (node == nullptr || node->right == nullptr)
        return nullptr;
    
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
    
    return rnode;
}


/**
 Performs rotation of a tree around `node` such that it goes to the right subtree
 */
template<class T>
AVLNode<T>* rotate_right(AVLNode<T>* node) {
    
    if (node == nullptr || node->left == nullptr)
        return nullptr;
    
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
    
    return lnode;
}


/**
 Find a node in the tree with a key
 */
template<class T>
AVLNode<T> *find(AVLNode<T> *root, T key) {
    AVLNode<T> *node = root;
    while (node != nullptr || node->data != key) {
        if (node->data == key)
            break;
        if (node->data < key) {
            node = node->right;
        } else {
            node = node->left;
        }
    }
    return node;
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
    
    // Rebalance the tree
    node = nullptr;
    while (parent_node != nullptr) {
        update_height(parent_node);
        int balance = get_balance(parent_node);
        if (balance > 1) { // left subtree is higher than right subtree by more than 1
            if (parent_node->left != nullptr && new_node->data > parent_node->left->data) {
                parent_node->left = rotate_left(parent_node->left);
            }
            parent_node = rotate_right(parent_node);
        } else if (balance < -1) { // right subtree is lower than left subtree by more than 1
            if (parent_node->right != nullptr && new_node->data < parent_node->right->data) {
                parent_node->right = rotate_right(parent_node->right);
            }
            parent_node = rotate_left(parent_node);
        }
        node = parent_node;
        parent_node = parent_node->parent;
    }
    
    root = node;
}


/**
 Delete node from tree
 */
template<class T>
void remove(AVLNode<T>* &root, AVLNode<T> *node) {
    
    AVLNode<T> *update_node = nullptr;
    
    if (node->left == nullptr && node->right == nullptr) {
        if (node->parent == nullptr) {
            root = nullptr;
            return;
        } else {
            if (node->parent->left == node)
                node->parent->left = nullptr;
            if (node->parent->right == node)
                node->parent->right = nullptr;
            update_node = node->parent;
        }
    } else if (node->left == nullptr) {
        if (node->parent != nullptr) {
            if (node->parent->left == node)
                node->parent->left = node->right;
            if (node->parent->right == node)
                node->parent->right = node->right;
        }
        node->right->parent = node->parent;
        update_node = node->right;
    } else if (node->right == nullptr) {
        if (node->parent != nullptr) {
            if (node->parent->left == node)
                node->parent->left = node->left;
            if (node->parent->right == node)
                node->parent->right = node->left;
        }
        node->left->parent = node->parent;
        update_node = node->left;
    } else {
        
        // find a node with minimal key in the right subtree of `node`
        AVLNode<T> *min_node = node->right;
        while (min_node->left != nullptr) {
            min_node = min_node->left;
        }
        
        update_node = min_node;
        
        // connect right subtree of `min_node` with the parent of `min_node`
        if (min_node->parent->left == min_node)
            min_node->parent->left = min_node->right;
        if (min_node->parent->right == min_node)
            min_node->parent->right = min_node->right;
        if (min_node->right != nullptr)
            min_node->right->parent = min_node->parent;
        
        // insert `min_node` instead of `node`
        min_node->parent = node->parent;
        if (node->parent != nullptr) {
            if (node->parent->left == node)
                node->parent->left = min_node;
            if (node->parent->right == node)
                node->parent->right = min_node;
        }
        
        if (node->left)
            node->left->parent = min_node;
        
        if (node->right)
            node->right->parent = min_node;
        
        min_node->left = node->left;
        min_node->right = node->right;
        
    }
        
    // update heights of the `min_node_parent` and rebalance if necessary
    AVLNode<T> *new_root = nullptr;
    while (update_node != nullptr) {
        // update height of a node
        update_height(update_node);
        // calculate balance factor of a node
        int balance = get_balance(update_node);
        if (balance > 1) { // left subtree is higher than right subtree by more than 1
            if (update_node->left != nullptr && get_balance(update_node->left) < 0) {
                update_node->left = rotate_left(update_node->left);
            }
            update_node = rotate_right(update_node);
        } else if (balance < -1) { // right subtree is lower than left subtree by more than 1
            if (update_node->right != nullptr && get_balance(update_node->right) > 0) {
                update_node->right = rotate_right(update_node->right);
            }
            update_node = rotate_left(update_node);
        }
        new_root = update_node;
        update_node = update_node->parent;
    }
    root = new_root;
}


/**
 Print tree
 */
template<class T>
void print_tree(AVLNode<T> *root, int width = 5) {

    if (root == nullptr)
        return;
    
    std::vector<std::vector<AVLNode<T>*>> layers(root->height);
    
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
                std::cout << std::setw(width * size) << layers[i][j]->data;
            else
                std::cout << std::setw(width * size) << "  ";
        }
        std::cout << std::endl;
        size /= 2;
    }
}



struct RNG {
    int operator() (int n) {
        return std::rand() / (1.0 + RAND_MAX) * n;
    }
};

int main(int argc, const char * argv[]) {
    
    AVLNode<int> *root = nullptr;
    
    std::vector<int> a;
    for (int i = 1; i <= 100000; ++i)
        a.push_back(i);
    
    int seed = static_cast<int>(time(0));
    
    std::cout << seed << std::endl;
    
    std::srand(seed);
    std::random_shuffle(a.begin(), a.end(), RNG());
    
    
    for (int i = 0; i < a.size(); ++i) {
        insert(root, new AVLNode<int>(a[i]));
        //print_tree(root, 3);
        //std::cout << std::endl;
    }

    std::random_shuffle(a.begin(), a.end(), RNG());
    
    for (int i = 0; i < a.size(); ++i) {
        //std::cout << "remove node: " << a[i] << std::endl;
        AVLNode<int> *node = find(root, a[i]);
        
        if (node == nullptr)
            continue;
        
        remove(root, node);
        //print_tree(root, 3);
    }
    
    
    
    
    
}
