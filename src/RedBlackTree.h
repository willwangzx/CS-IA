#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum Color { RED, BLACK };

enum class RBTreeStepKind {
    Compare,
    InsertRed,
    DeleteTarget,
    Successor,
    Transplant,
    RotateLeft,
    RotateRight,
    Recolor,
    RootBlack,
    Complete
};

template <typename T>
struct RBTreeVisualNode {
    int id = -1;
    int parentId = -1;
    int leftId = -1;
    int rightId = -1;
    int depth = 0;
    bool isNil = false;
    Color color = BLACK;
    T data{};
};

template <typename T>
struct RBTreeVisualSnapshot {
    std::vector<RBTreeVisualNode<T>> nodes;
    int rootId = -1;
    int nodeCount = 0;
    int height = 0;
    int blackHeight = 0;
    bool rootBlack = true;
    bool noRedRed = true;
    bool uniformBlackHeight = true;
};

template <typename T>
struct RBTreeTraceStep {
    RBTreeStepKind kind = RBTreeStepKind::Complete;
    std::string title;
    std::string detail;
    RBTreeVisualSnapshot<T> snapshot;
    int primaryNodeId = -1;
    int secondaryNodeId = -1;
};

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
    void leftRotate(RBNode<T>* x, std::vector<RBTreeTraceStep<T>>* trace = nullptr);
    void rightRotate(RBNode<T>* x, std::vector<RBTreeTraceStep<T>>* trace = nullptr);
    void insertFixup(RBNode<T>* z, std::vector<RBTreeTraceStep<T>>* trace = nullptr);
    void deleteFixup(RBNode<T>* x, std::vector<RBTreeTraceStep<T>>* trace = nullptr);
    void transplant(RBNode<T>* u, RBNode<T>* v, std::vector<RBTreeTraceStep<T>>* trace = nullptr);
    RBNode<T>* minimum(RBNode<T>* node);
    RBNode<T>* maximum(RBNode<T>* node);
    void inorderHelper(RBNode<T>* node) const;
    // void inorderTraversal(T func) const;
    template <typename Func>
    void inorderHelperFunc(RBNode<T>* node, Func func) const;
    void destroyTree(RBNode<T> *node);
    void insertImpl(const T& data, std::vector<RBTreeTraceStep<T>>* trace);
    bool removeImpl(const T& data, std::vector<RBTreeTraceStep<T>>* trace);
    void appendTrace(std::vector<RBTreeTraceStep<T>>* trace, RBTreeStepKind kind,
                     const std::string& title, const std::string& detail = "",
                     RBNode<T>* primary = nullptr, RBNode<T>* secondary = nullptr) const;
    RBTreeVisualSnapshot<T> buildVisualSnapshot(
        std::unordered_map<const RBNode<T>*, int>* nodeIds = nullptr) const;
    int fillVisualSnapshot(RBNode<T>* node, int parentId, int depth,
                           RBTreeVisualSnapshot<T>& snapshot,
                           std::unordered_map<const RBNode<T>*, int>* nodeIds,
                           bool includeNilLeaves) const;
    int heightOf(RBNode<T>* node) const;
    bool hasNoRedRedViolation(RBNode<T>* node) const;
    std::pair<bool, int> validateBlackHeight(RBNode<T>* node) const;

public:
    RedBlackTree();
    ~RedBlackTree();

    RedBlackTree(const RedBlackTree&) = delete;
    RedBlackTree& operator=(const RedBlackTree&) = delete;
    
    void insert(const T& data);
    std::vector<RBTreeTraceStep<T>> insertWithTrace(const T& data);
    void remove(const T& data);
    std::vector<RBTreeTraceStep<T>> removeWithTrace(const T& data);
    RBNode<T>* search(const T& data);
    void inorderTraversal() const;
    bool isEmpty() const { return root == nil; }
    void clear() {
        destroyTree(root);
        nil->left = nil->right = nil;
        nil->parent = nullptr;
        root = nil;
        nil->color = BLACK;
    }
    template <typename Func>
    void inorderTraversal(Func func) const;
    template <typename Func>
    RBNode<T>* findFirst(Func func);
    RBNode<T>* lowerBound(const T& key) const;
    RBNode<T>* successor(RBNode<T>* node) const;
    RBTreeVisualSnapshot<T> visualSnapshot() const;
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
void RedBlackTree<T>::leftRotate(RBNode<T>* x, std::vector<RBTreeTraceStep<T>>* trace) {
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

    appendTrace(trace, RBTreeStepKind::RotateLeft,
                "Rotate left", "Pivot around the selected node.", y, x);
}

template <typename T>
void RedBlackTree<T>::rightRotate(RBNode<T>* x, std::vector<RBTreeTraceStep<T>>* trace) {
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

    appendTrace(trace, RBTreeStepKind::RotateRight,
                "Rotate right", "Pivot around the selected node.", y, x);
}

template <typename T>
void RedBlackTree<T>::insert(const T& data) {
    insertImpl(data, nullptr);
}

template <typename T>
std::vector<RBTreeTraceStep<T>> RedBlackTree<T>::insertWithTrace(const T& data) {
    std::vector<RBTreeTraceStep<T>> trace;
    insertImpl(data, &trace);
    appendTrace(&trace, RBTreeStepKind::Complete,
                "Insert complete", "The tree satisfies the red-black invariants.");
    return trace;
}

template <typename T>
void RedBlackTree<T>::insertImpl(const T& data, std::vector<RBTreeTraceStep<T>>* trace) {
    RBNode<T>* z = new RBNode<T>(data);
    RBNode<T>* y = nullptr;
    RBNode<T>* x = root;
    
    while (x != nil) {
        appendTrace(trace, RBTreeStepKind::Compare,
                    "Compare", "Walk down the search path for the insertion point.", x, y);
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

    appendTrace(trace, RBTreeStepKind::InsertRed,
                "Insert red node", "New red-black tree nodes are inserted red first.", z, y);
    
    insertFixup(z, trace);
}

template <typename T>
void RedBlackTree<T>::insertFixup(RBNode<T>* z, std::vector<RBTreeTraceStep<T>>* trace) {
    while (z->parent != nullptr && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode<T>* y = z->parent->parent->right;
            appendTrace(trace, RBTreeStepKind::Compare,
                        "Inspect uncle", "Parent is red, so the uncle determines the fixup case.", z, y);
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                appendTrace(trace, RBTreeStepKind::Recolor,
                            "Recolor parent, uncle, grandparent",
                            "A red uncle creates a recoloring case.", z->parent->parent, y);
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(z, trace);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                appendTrace(trace, RBTreeStepKind::Recolor,
                            "Recolor before rotation",
                            "A black uncle creates a rotation case.", z->parent, z->parent->parent);
                rightRotate(z->parent->parent, trace);
            }
        } else {
            RBNode<T>* y = z->parent->parent->left;
            appendTrace(trace, RBTreeStepKind::Compare,
                        "Inspect uncle", "Parent is red, so the uncle determines the fixup case.", z, y);
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                appendTrace(trace, RBTreeStepKind::Recolor,
                            "Recolor parent, uncle, grandparent",
                            "A red uncle creates a recoloring case.", z->parent->parent, y);
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(z, trace);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                appendTrace(trace, RBTreeStepKind::Recolor,
                            "Recolor before rotation",
                            "A black uncle creates a rotation case.", z->parent, z->parent->parent);
                leftRotate(z->parent->parent, trace);
            }
        }
    }
    root->color = BLACK;
    appendTrace(trace, RBTreeStepKind::RootBlack,
                "Force root black", "The root is always black after insertion.", root);
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
void RedBlackTree<T>::transplant(RBNode<T>* u, RBNode<T>* v, std::vector<RBTreeTraceStep<T>>* trace) {
    if (u->parent == nullptr) {
        root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
    appendTrace(trace, RBTreeStepKind::Transplant,
                "Transplant subtree", "Replace one subtree with another during deletion.",
                v != nil ? v : nullptr, u->parent);
}

template <typename T>
void RedBlackTree<T>::remove(const T& data) {
    removeImpl(data, nullptr);
}

template <typename T>
std::vector<RBTreeTraceStep<T>> RedBlackTree<T>::removeWithTrace(const T& data) {
    std::vector<RBTreeTraceStep<T>> trace;
    const bool removed = removeImpl(data, &trace);
    appendTrace(&trace, RBTreeStepKind::Complete,
                removed ? "Delete complete" : "Delete skipped",
                removed ? "The tree satisfies the red-black invariants."
                        : "No matching node was found.");
    return trace;
}

template <typename T>
bool RedBlackTree<T>::removeImpl(const T& data, std::vector<RBTreeTraceStep<T>>* trace) {
    RBNode<T>* z = root;
    while (z != nil) {
        appendTrace(trace, RBTreeStepKind::Compare,
                    "Compare", "Walk down the search path for the deletion target.", z);
        if (data < z->data) {
            z = z->left;
        } else if (z->data < data) {
            z = z->right;
        } else {
            break;
        }
    }

    if (z == nullptr) {
        return false;
    }
    if (z == nil) {
        return false;
    }

    appendTrace(trace, RBTreeStepKind::DeleteTarget,
                "Delete target found", "This is the node selected for removal.", z);
    
    RBNode<T>* y = z;
    RBNode<T>* x;
    Color yOriginalColor = y->color;
    
    if (z->left == nil) {
        x = z->right;
        transplant(z, z->right, trace);
    } else if (z->right == nil) {
        x = z->left;
        transplant(z, z->left, trace);
    } else {
        y = minimum(z->right);
        appendTrace(trace, RBTreeStepKind::Successor,
                    "Find successor", "Two-child deletion uses the in-order successor.", z, y);
        yOriginalColor = y->color;
        x = y->right;
        
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(y, y->right, trace);
            y->right = z->right;
            y->right->parent = y;
        }
        
        transplant(z, y, trace);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
        appendTrace(trace, RBTreeStepKind::Recolor,
                    "Copy deleted node color",
                    "The successor takes the removed node's color.", y);
    }
    
    delete z;
    
    if (yOriginalColor == BLACK) {
        deleteFixup(x, trace);
    }
    return true;
}

template <typename T>
void RedBlackTree<T>::deleteFixup(RBNode<T>* x, std::vector<RBTreeTraceStep<T>>* trace) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            RBNode<T>* w = x->parent->right;
            appendTrace(trace, RBTreeStepKind::Compare,
                        "Inspect sibling", "Delete fixup compares sibling color and children.", x, w);
            
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                appendTrace(trace, RBTreeStepKind::Recolor,
                            "Recolor red sibling case",
                            "A red sibling is converted to a black-sibling case.", x->parent, w);
                leftRotate(x->parent, trace);
                w = x->parent->right;
            }
            
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                appendTrace(trace, RBTreeStepKind::Recolor,
                            "Recolor black sibling",
                            "A black sibling with black children pushes the extra black upward.", x, w);
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    appendTrace(trace, RBTreeStepKind::Recolor,
                                "Prepare far-child rotation",
                                "Recolor before rotating the sibling.", x, w);
                    rightRotate(w, trace);
                    w = x->parent->right;
                }
                
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                appendTrace(trace, RBTreeStepKind::Recolor,
                            "Resolve delete fixup",
                            "Recolor and rotate to restore black height.", x->parent, w);
                leftRotate(x->parent, trace);
                x = root;
            }
        } else {
            RBNode<T>* w = x->parent->left;
            appendTrace(trace, RBTreeStepKind::Compare,
                        "Inspect sibling", "Delete fixup compares sibling color and children.", x, w);
            
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                appendTrace(trace, RBTreeStepKind::Recolor,
                            "Recolor red sibling case",
                            "A red sibling is converted to a black-sibling case.", x->parent, w);
                rightRotate(x->parent, trace);
                w = x->parent->left;
            }
            
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                appendTrace(trace, RBTreeStepKind::Recolor,
                            "Recolor black sibling",
                            "A black sibling with black children pushes the extra black upward.", x, w);
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    appendTrace(trace, RBTreeStepKind::Recolor,
                                "Prepare far-child rotation",
                                "Recolor before rotating the sibling.", x, w);
                    leftRotate(w, trace);
                    w = x->parent->left;
                }
                
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                appendTrace(trace, RBTreeStepKind::Recolor,
                            "Resolve delete fixup",
                            "Recolor and rotate to restore black height.", x->parent, w);
                rightRotate(x->parent, trace);
                x = root;
            }
        }
    }
    x->color = BLACK;
    appendTrace(trace, RBTreeStepKind::RootBlack,
                "Finish delete fixup", "The replacement node/root is black.", x != nil ? x : root);
}

template <typename T>
RBNode<T>* RedBlackTree<T>::search(const T& data) {
    RBNode<T>* current = root;

    while (current != nil) {
        if (data < current->data) {
            current = current->left;
        } else if (current->data < data) {
            current = current->right;
        } else {
            return current;
        }
    }

    return nullptr;
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

template <typename T>
RBNode<T>* RedBlackTree<T>::lowerBound(const T& key) const {
    RBNode<T>* current = root;
    RBNode<T>* candidate = nullptr;

    while (current != nil) {
        if (current->data < key) {
            current = current->right;
        } else {
            candidate = current;
            current = current->left;
        }
    }

    return candidate;
}

template <typename T>
RBNode<T>* RedBlackTree<T>::successor(RBNode<T>* node) const {
    if (node == nullptr || node == nil) return nullptr;

    if (node->right != nil) {
        RBNode<T>* succ = node->right;
        while (succ->left != nil) {
            succ = succ->left;
        }
        return succ;
    }

    RBNode<T>* parent = node->parent;
    while (parent != nullptr && node == parent->right) {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

template <typename T>
void RedBlackTree<T>::appendTrace(std::vector<RBTreeTraceStep<T>>* trace,
                                  RBTreeStepKind kind,
                                  const std::string& title,
                                  const std::string& detail,
                                  RBNode<T>* primary,
                                  RBNode<T>* secondary) const {
    if (trace == nullptr) {
        return;
    }

    std::unordered_map<const RBNode<T>*, int> nodeIds;
    RBTreeTraceStep<T> step;
    step.kind = kind;
    step.title = title;
    step.detail = detail;
    step.snapshot = buildVisualSnapshot(&nodeIds);

    auto findId = [&nodeIds, this](RBNode<T>* node) {
        if (node == nullptr || node == nil) {
            return -1;
        }
        auto it = nodeIds.find(node);
        return it != nodeIds.end() ? it->second : -1;
    };

    step.primaryNodeId = findId(primary);
    step.secondaryNodeId = findId(secondary);
    trace->push_back(step);
}

template <typename T>
RBTreeVisualSnapshot<T> RedBlackTree<T>::visualSnapshot() const {
    return buildVisualSnapshot(nullptr);
}

template <typename T>
RBTreeVisualSnapshot<T> RedBlackTree<T>::buildVisualSnapshot(
    std::unordered_map<const RBNode<T>*, int>* nodeIds) const {
    RBTreeVisualSnapshot<T> snapshot;

    if (root == nil) {
        return snapshot;
    }

    snapshot.height = heightOf(root);
    snapshot.rootBlack = (root->color == BLACK);
    snapshot.noRedRed = hasNoRedRedViolation(root);

    const std::pair<bool, int> blackHeight = validateBlackHeight(root);
    snapshot.uniformBlackHeight = blackHeight.first;
    snapshot.blackHeight = blackHeight.second;

    const bool includeNilLeaves = snapshot.height <= 5;
    snapshot.rootId = fillVisualSnapshot(root, -1, 0, snapshot, nodeIds, includeNilLeaves);
    return snapshot;
}

template <typename T>
int RedBlackTree<T>::fillVisualSnapshot(
    RBNode<T>* node, int parentId, int depth, RBTreeVisualSnapshot<T>& snapshot,
    std::unordered_map<const RBNode<T>*, int>* nodeIds, bool includeNilLeaves) const {
    if (node == nil) {
        if (!includeNilLeaves) {
            return -1;
        }

        RBTreeVisualNode<T> visualNil;
        visualNil.id = static_cast<int>(snapshot.nodes.size());
        visualNil.parentId = parentId;
        visualNil.depth = depth;
        visualNil.isNil = true;
        visualNil.color = BLACK;
        snapshot.nodes.push_back(visualNil);
        return visualNil.id;
    }

    RBTreeVisualNode<T> visualNode;
    visualNode.id = static_cast<int>(snapshot.nodes.size());
    visualNode.parentId = parentId;
    visualNode.depth = depth;
    visualNode.isNil = false;
    visualNode.color = node->color;
    visualNode.data = node->data;
    snapshot.nodes.push_back(visualNode);
    ++snapshot.nodeCount;

    if (nodeIds != nullptr) {
        (*nodeIds)[node] = visualNode.id;
    }

    const int leftId = fillVisualSnapshot(node->left, visualNode.id, depth + 1,
                                          snapshot, nodeIds, includeNilLeaves);
    const int rightId = fillVisualSnapshot(node->right, visualNode.id, depth + 1,
                                           snapshot, nodeIds, includeNilLeaves);

    snapshot.nodes[visualNode.id].leftId = leftId;
    snapshot.nodes[visualNode.id].rightId = rightId;
    return visualNode.id;
}

template <typename T>
int RedBlackTree<T>::heightOf(RBNode<T>* node) const {
    if (node == nil) {
        return 0;
    }
    return 1 + std::max(heightOf(node->left), heightOf(node->right));
}

template <typename T>
bool RedBlackTree<T>::hasNoRedRedViolation(RBNode<T>* node) const {
    if (node == nil) {
        return true;
    }
    if (node->color == RED) {
        if (node->left->color == RED || node->right->color == RED) {
            return false;
        }
    }
    return hasNoRedRedViolation(node->left) && hasNoRedRedViolation(node->right);
}

template <typename T>
std::pair<bool, int> RedBlackTree<T>::validateBlackHeight(RBNode<T>* node) const {
    if (node == nil) {
        return {true, 1};
    }

    const std::pair<bool, int> left = validateBlackHeight(node->left);
    const std::pair<bool, int> right = validateBlackHeight(node->right);
    const bool valid = left.first && right.first && left.second == right.second;
    const int currentBlackHeight = left.second + (node->color == BLACK ? 1 : 0);
    return {valid, currentBlackHeight};
}
#endif
