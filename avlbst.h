#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* node, int8_t diff);
    void removeFix(AVLNode<Key, Value>* node, int8_t diff);
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* x);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* y);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // Handle empty tree case
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;
    bool wentLeft = false;

    // Find insertion point
    while (curr != nullptr) {
        parent = curr;
        if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
            wentLeft = true;
        } 
        else if (curr->getKey() < new_item.first) {
            curr = curr->getRight();
            wentLeft = false;
        } 
        else {
            // Key already exists, update value and return
            curr->setValue(new_item.second);
            return;
        }
    }

    // Create new node
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    
    // Link new node to parent
    if (wentLeft) {
        parent->setLeft(newNode);
        // Update balance and fix if needed
        if (parent->getBalance() == 0) {
            parent->setBalance(1);
            insertFix(parent, 1);
        }
        else {
            // Parent had balance -1, now it's balanced at 0
            parent->setBalance(0);
        }
    } 
    else {
        parent->setRight(newNode);
        // Update balance and fix if needed
        if (parent->getBalance() == 0) {
            parent->setBalance(-1);
            insertFix(parent, -1);
        }
        else {
            // Parent had balance 1, now it's balanced at 0
            parent->setBalance(0);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* node, int8_t diff)
{
    // Base case: reached root or no more propagation needed
    if (node == nullptr || node->getBalance() == 0) {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    
    // Determine if node is left or right child of parent
    int8_t nextDiff = 0;
    if (parent) {
        if (parent->getLeft() == node) {
            nextDiff = 1;
        } else {
            nextDiff = -1;
        }
    }

    // Case 1: The node has become unbalanced
    if (node->getBalance() == 2) {
        // Left heavy
        if (node->getLeft()->getBalance() == 1) {
            // Left-left case
            rotateRight(node);
            node->setBalance(0);
            node->getParent()->setBalance(0);
        } 
        else {
            // Left-right case
            AVLNode<Key, Value>* leftChild = node->getLeft();
            AVLNode<Key, Value>* rightGrandchild = leftChild->getRight();
            rotateLeft(leftChild);
            rotateRight(node);
            
            // Update balances based on grandchild's balance
            if (rightGrandchild->getBalance() == 1) {
                node->setBalance(-1);
                leftChild->setBalance(0);
                rightGrandchild->setBalance(0);
            } 
            else if (rightGrandchild->getBalance() == -1) {
                node->setBalance(0);
                leftChild->setBalance(1);
                rightGrandchild->setBalance(0);
            } 
            else {
                node->setBalance(0);
                leftChild->setBalance(0);
                rightGrandchild->setBalance(0);
            }
        }
        return; // No need to propagate further
    } 
    else if (node->getBalance() == -2) {
        // Right heavy
        if (node->getRight()->getBalance() == -1) {
            // Right-right case
            rotateLeft(node);
            node->setBalance(0);
            node->getParent()->setBalance(0);
        } 
        else {
            // Right-left case
            AVLNode<Key, Value>* rightChild = node->getRight();
            AVLNode<Key, Value>* leftGrandchild = rightChild->getLeft();
            rotateRight(rightChild);
            rotateLeft(node);
            
            // Update balances based on grandchild's balance
            if (leftGrandchild->getBalance() == -1) {
                node->setBalance(1);
                rightChild->setBalance(0);
                leftGrandchild->setBalance(0);
            } 
            else if (leftGrandchild->getBalance() == 1) {
                node->setBalance(0);
                rightChild->setBalance(-1);
                leftGrandchild->setBalance(0);
            } 
            else {
                node->setBalance(0);
                rightChild->setBalance(0);
                leftGrandchild->setBalance(0);
            }
        }
        return; // No need to propagate further
    }
    
    // Case 2: The node is balanced but we need to propagate up
    if (parent) {
        parent->updateBalance(nextDiff);
        if (parent->getBalance() != 0) {
            insertFix(parent, nextDiff);
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (node == nullptr) {
        return;
    }

    // If node has two children, swap with predecessor
    if (node->getLeft() != nullptr && node->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        nodeSwap(node, pred);
    }

    // Now node has at most one child
    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* child = (node->getLeft() != nullptr) ? node->getLeft() : node->getRight();
    int8_t diff = 0;

    // Calculate balance factor change for parent
    if (parent != nullptr) {
        if (parent->getLeft() == node) {
            diff = -1; // Removing from left child, will decrease balance
        } else {
            diff = 1; // Removing from right child, will increase balance
        }
    }

    // Connect child to parent
    if (child != nullptr) {
        child->setParent(parent);
    }

    // Update parent's child pointer or root
    if (parent == nullptr) {
        this->root_ = child;
    } else if (parent->getLeft() == node) {
        parent->setLeft(child);
    } else {
        parent->setRight(child);
    }

    // Delete the node
    delete node;

    // Rebalance the tree
    if (parent != nullptr) {
        removeFix(parent, diff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int8_t diff)
{
    if (node == nullptr) return;

    // Calculate next node and diff for potential upward propagation
    AVLNode<Key, Value>* parent = node->getParent();
    int8_t nextDiff = 0;
    
    if (parent != nullptr) {
        if (parent->getLeft() == node) {
            nextDiff = -1;
        } else {
            nextDiff = 1;
        }
    }

    // Apply the balancing change
    node->updateBalance(diff);
    
    // Check if tree is now unbalanced
    if (node->getBalance() == 2) {
        // Left heavy
        AVLNode<Key, Value>* leftChild = node->getLeft();
        
        // Determine rotation type based on left child's balance
        if (leftChild->getBalance() >= 0) {
            // Left-left case
            rotateRight(node);
            
            if (leftChild->getBalance() == 0) {
                node->setBalance(1);
                leftChild->setBalance(-1);
                // Balance is now optimal, no need to propagate
                return;
            } else {
                node->setBalance(0);
                leftChild->setBalance(0);
                // Continue propagation
                if (parent) removeFix(parent, nextDiff);
            }
        } else {
            // Left-right case
            AVLNode<Key, Value>* rightGrandchild = leftChild->getRight();
            rotateLeft(leftChild);
            rotateRight(node);
            
            // Update balances
            if (rightGrandchild->getBalance() == 1) {
                node->setBalance(-1);
                leftChild->setBalance(0);
            } else if (rightGrandchild->getBalance() == -1) {
                node->setBalance(0);
                leftChild->setBalance(1);
            } else {
                node->setBalance(0);
                leftChild->setBalance(0);
            }
            rightGrandchild->setBalance(0);
            
            // Continue propagation
            if (parent) removeFix(parent, nextDiff);
        }
    } else if (node->getBalance() == -2) {
        // Right heavy
        AVLNode<Key, Value>* rightChild = node->getRight();
        
        // Determine rotation type based on right child's balance
        if (rightChild->getBalance() <= 0) {
            // Right-right case
            rotateLeft(node);
            
            if (rightChild->getBalance() == 0) {
                node->setBalance(-1);
                rightChild->setBalance(1);
                // Balance is now optimal, no need to propagate
                return;
            } else {
                node->setBalance(0);
                rightChild->setBalance(0);
                // Continue propagation
                if (parent) removeFix(parent, nextDiff);
            }
        } else {
            // Right-left case
            AVLNode<Key, Value>* leftGrandchild = rightChild->getLeft();
            rotateRight(rightChild);
            rotateLeft(node);
            
            // Update balances
            if (leftGrandchild->getBalance() == -1) {
                node->setBalance(1);
                rightChild->setBalance(0);
            } else if (leftGrandchild->getBalance() == 1) {
                node->setBalance(0);
                rightChild->setBalance(-1);
            } else {
                node->setBalance(0);
                rightChild->setBalance(0);
            }
            leftGrandchild->setBalance(0);
            
            // Continue propagation
            if (parent) removeFix(parent, nextDiff);
        }
    } else if (node->getBalance() == 1 || node->getBalance() == -1) {
        // Node is balanced but not perfectly - no propagation needed
        return;
    } else {
        // Node is perfectly balanced (0), continue propagation
        if (parent && diff != 0) {
            removeFix(parent, nextDiff);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getRight();
    AVLNode<Key, Value>* parent = x->getParent();
    
    // Perform rotation
    x->setRight(y->getLeft());
    if (y->getLeft() != nullptr) {
        y->getLeft()->setParent(x);
    }
    
    y->setLeft(x);
    x->setParent(y);
    y->setParent(parent);
    
    // Update parent pointers
    if (parent == nullptr) {
        this->root_ = y;
    } else if (parent->getLeft() == x) {
        parent->setLeft(y);
    } else {
        parent->setRight(y);
    }
    
    return y;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* y)
{
    AVLNode<Key, Value>* x = y->getLeft();
    AVLNode<Key, Value>* parent = y->getParent();
    
    // Perform rotation
    y->setLeft(x->getRight());
    if (x->getRight() != nullptr) {
        x->getRight()->setParent(y);
    }
    
    x->setRight(y);
    y->setParent(x);
    x->setParent(parent);
    
    // Update parent pointers
    if (parent == nullptr) {
        this->root_ = x;
    } else if (parent->getLeft() == y) {
        parent->setLeft(x);
    } else {
        parent->setRight(x);
    }
    
    return x;
}

#endif