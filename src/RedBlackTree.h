#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H

#include "Book.h"
#include <memory>

enum Color { RED, BLACK };

template <typename T>
class RBNode {
public:
    T data;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;

    RBNode(const T& data) 
        : data(data), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

template <typename T>
class RedBlackTree {
private:
    RBNode<T>* root;
    RBNode<T>* nil; // Sentinel node

    // Helper functions
    void leftRotate(RBNode<T>* x);
    void rightRotate(RBNode<T>* x);
    void insertFixup(RBNode<T>* z);
    void deleteFixup(RBNode<T>* x);
    void transplant(RBNode<T>* u, RBNode<T>* v);
    RBNode<T>* minimum(RBNode<T>* node);
    RBNode<T>* maximum(RBNode<T>* node);
    void inorderHelper(RBNode<T>* node) const;
    // void inorderTraversal(T func) const;
    template <typename Func>
    void inorderHelperFunc(RBNode<T>* node, Func func) const;
    void destroyTree(RBNode<T> *node);
    void inorderHelper(RBNode<T>* node, const T& data)const;

public:
    RedBlackTree();
    ~RedBlackTree();
    
    void insert(const T& data);
    void remove(const T& data);
    RBNode<T>* search(const T& data);
    void inorderTraversal() const;
    bool isEmpty() const { return root == nil; }
    void clear() {
        destroyTree(root);
        root = nil;
        nil->parent = nullptr;
    }
    template <typename Func>
    void inorderTraversal(Func func) const;
    template <typename Func>
    RBNode<T>* findFirst(Func func);
};

// Implementation
template <typename T, typename Func>
RBNode<T>* findFirstHelper(RBNode<T>* node, RBNode<T>* nil, Func func) {
    if (node == nil) {
        return nullptr;
    }

    if (RBNode<T>* leftMatch = findFirstHelper(node->left, nil, func)) {
        return leftMatch;
    }

    if (func(node->data)) {
        return node;
    }

    return findFirstHelper(node->right, nil, func);
}

template <typename T>
RedBlackTree<T>::RedBlackTree() {
    nil = new RBNode<T>(T());
    nil->color = BLACK;
    nil->left = nil->right = nil;
    nil->parent = nullptr;
    root = nil;
}

template <typename T>
RedBlackTree<T>::~RedBlackTree() {
    destroyTree(root);
    delete nil;
}

template <typename T>
void RedBlackTree<T>::destroyTree(RBNode<T>* node) {
    if (node != nil) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

template <typename T>
void RedBlackTree<T>::leftRotate(RBNode<T>* x) {
    RBNode<T>* y = x->right;
    x->right = y->left;
    
    if (y->left != nil) {
        y->left->parent = x;
    }
    
    y->parent = x->parent;
    
    if (x->parent == nullptr) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    
    y->left = x;
    x->parent = y;
}

template <typename T>
void RedBlackTree<T>::rightRotate(RBNode<T>* x) {
    RBNode<T>* y = x->left;
    x->left = y->right;
    
    if (y->right != nil) {
        y->right->parent = x;
    }
    
    y->parent = x->parent;
    
    if (x->parent == nullptr) {
        root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    
    y->right = x;
    x->parent = y;
}

template <typename T>
void RedBlackTree<T>::insert(const T& data) {
    RBNode<T>* z = new RBNode<T>(data);
    RBNode<T>* y = nullptr;
    RBNode<T>* x = root;
    
    while (x != nil) {
        y = x;
        if (z->data < x->data) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    
    z->parent = y;
    
    if (y == nullptr) {
        root = z;
    } else if (z->data < y->data) {
        y->left = z;
    } else {
        y->right = z;
    }
    
    z->left = nil;
    z->right = nil;
    z->color = RED;
    
    insertFixup(z);
}

template <typename T>
void RedBlackTree<T>::insertFixup(RBNode<T>* z) {
    while (z->parent != nullptr && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode<T>* y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(z->parent->parent);
            }
        } else {
            RBNode<T>* y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(z->parent->parent);
            }
        }
    }
    root->color = BLACK;
}

template <typename T>
RBNode<T>* RedBlackTree<T>::minimum(RBNode<T>* node) {
    while (node->left != nil) {
        node = node->left;
    }
    return node;
}

template <typename T>
RBNode<T>* RedBlackTree<T>::maximum(RBNode<T>* node) {
    while (node->right != nil) {
        node = node->right;
    }
    return node;
}

template <typename T>
void RedBlackTree<T>::transplant(RBNode<T>* u, RBNode<T>* v) {
    if (u->parent == nullptr) {
        root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

template <typename T>
void RedBlackTree<T>::remove(const T& data) {
    RBNode<T>* z = search(data);
    if (z == nullptr) {
        return;
    }
    
    RBNode<T>* y = z;
    RBNode<T>* x;
    Color yOriginalColor = y->color;
    
    if (z->left == nil) {
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == nil) {
        x = z->left;
        transplant(z, z->left);
    } else {
        y = minimum(z->right);
        yOriginalColor = y->color;
        x = y->right;
        
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    
    delete z;
    
    if (yOriginalColor == BLACK) {
        deleteFixup(x);
    }
}

template <typename T>
void RedBlackTree<T>::deleteFixup(RBNode<T>* x) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            RBNode<T>* w = x->parent->right;
            
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(x->parent);
                w = x->parent->right;
            }
            
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotate(w);
                    w = x->parent->right;
                }
                
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                leftRotate(x->parent);
                x = root;
            }
        } else {
            RBNode<T>* w = x->parent->left;
            
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(x->parent);
                w = x->parent->left;
            }
            
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(w);
                    w = x->parent->left;
                }
                
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rightRotate(x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
}

template <typename T>
RBNode<T>* RedBlackTree<T>::search(const T& data) {
    RBNode<T>* current = root;
    
    while (current != nil && !(current->data == data)) {
        if (data < current->data) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return (current == nil) ? nullptr : current;
}

template <typename T>
void RedBlackTree<T>::inorderTraversal() const {
    inorderHelper(root);
    std::cout << std::endl;
}

template <typename T>
void RedBlackTree<T>::inorderHelper(RBNode<T>* node) const {
    if (node != nil) {
        inorderHelper(node->left);
        node->data.display();
        inorderHelper(node->right);
    }
}

template <typename T>
template <typename Func>
void RedBlackTree<T>::inorderTraversal(Func func) const {
    inorderHelperFunc(root, func);
}

template <typename T>
template <typename Func>
void RedBlackTree<T>::inorderHelperFunc(RBNode<T>* node, Func func) const {
    if (node != nil) {
        inorderHelperFunc(node->left, func);
        func(node->data);
        inorderHelperFunc(node->right, func);
    }
}

template <typename T>
template <typename Func>
RBNode<T>* RedBlackTree<T>::findFirst(Func func) {
    return findFirstHelper(root, nil, func);
}
#endif
