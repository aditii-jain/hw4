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
    void balanceTree();
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* x);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* y);
    int getBalanceFactor(AVLNode<Key, Value>* node);
    void updateBalance(AVLNode<Key, Value>* node, const Key& key);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_); // start at root
    AVLNode<Key, Value>* parent = NULL; // track parent during traversal

    // traverse tree to find insert location
    while (curr != NULL) {
        parent = curr;
        if (curr->getKey() < new_item.first) {
            curr = curr->getRight(); // move right
        } else if (curr->getKey() > new_item.first) {
            curr = curr->getLeft(); // move left
        } else {
            curr->setValue(new_item.second); // key exists, update value
            return;
        }
    }

    // create new node
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    // set as root if tree was empty
    if (parent == nullptr) {
        this->root_ = newNode;
    } else if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode); // attach to left
    } else {
        parent->setRight(newNode); // attach to right
    }

    // walk up to root and balance
    AVLNode<Key, Value>* temp = newNode;
    while (temp != NULL) {
        updateBalance(temp, newNode->getKey()); // rotate if unbalanced
        temp = temp->getParent(); // move up
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->internalFind(key)); // find node to delete
    if (curr == NULL) return;

    AVLNode<Key, Value>* parent = curr->getParent(); // track parent for rebalancing

    // case: 2 children, swap with predecessor
    if (curr->getLeft() != NULL && curr->getRight() != NULL) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(curr));
        this->nodeSwap(pred, curr);
        parent = curr->getParent(); // update parent after swap
    }

    // case: 0 or 1 child
    AVLNode<Key, Value>* child;
    if (curr->getLeft() != NULL) {
      child = curr->getLeft();
    } else {
      child = curr->getRight();
    }

    // link child to parent
    if (child != NULL) {
        child->setParent(parent);
    }

    // reassign root or parent's child pointer
    if (parent == NULL) {
        this->root_ = child; // deleting root
    } else if (parent->getLeft() == curr) {
        parent->setLeft(child); // remove left link
    } else {
        parent->setRight(child); // remove right link
    }

    delete curr; // free mem

    // rebalance while walking up
    while (parent != NULL) {
        parent->setBalance(getBalanceFactor(parent)); // recompute balance
        int bf = parent->getBalance();

        if (bf > 1 && getBalanceFactor(parent->getLeft()) >= 0) {
            rotateRight(parent); // left left
        } else if (bf > 1 && getBalanceFactor(parent->getLeft()) < 0) {
            parent->setLeft(rotateLeft(parent->getLeft())); // left right
            rotateRight(parent);
        } else if (bf < -1 && getBalanceFactor(parent->getRight()) <= 0) {
            rotateLeft(parent); // right right
        } else if (bf < -1 && getBalanceFactor(parent->getRight()) > 0) {
            parent->setRight(rotateRight(parent->getRight())); // right left
            rotateLeft(parent);
        }

        parent = parent->getParent(); // move up
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
int AVLTree<Key, Value>::getBalanceFactor(AVLNode<Key, Value>* node) {
    if (node == NULL) return 0; // null node = balance 0
    int lh = this->height(node->getLeft()); // left height
    int rh = this->height(node->getRight()); // right height
    return lh - rh; // balance = left - right
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalance(AVLNode<Key, Value>* node, const Key& key) {
    int balance = getBalanceFactor(node); // compute balance factor

    // left heavy
    if (balance > 1 && key < node->getLeft()->getKey()) {
        rotateRight(node); // left left
    } 
    else if (balance > 1 && key > node->getLeft()->getKey()) {
        node->setLeft(rotateLeft(node->getLeft())); // left right
        rotateRight(node);
    } 
    // right heavy
    else if (balance < -1 && key > node->getRight()->getKey()) {
        rotateLeft(node); // right right
    } 
    else if (balance < -1 && key < node->getRight()->getKey()) {
        node->setRight(rotateRight(node->getRight())); // right left
        rotateLeft(node);
    }

    node->setBalance(getBalanceFactor(node)); // update balance field
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x) {
    AVLNode<Key, Value>* y = x->getRight(); // y = right child
    AVLNode<Key, Value>* beta = y->getLeft(); // beta = subtree between x and y

    y->setLeft(x); // x becomes left child of y
    x->setRight(beta); // beta becomes right child of x

    if (beta != NULL) beta->setParent(x);
    y->setParent(x->getParent());

    // fix parent links
    if (x == this->root_) {
        this->root_ = y; // update root
    } else {
        if (x->getParent()->getLeft() == x)
            x->getParent()->setLeft(y);
        else
            x->getParent()->setRight(y);
    }

    x->setParent(y);

    // update balances
    x->setBalance(getBalanceFactor(x));
    y->setBalance(getBalanceFactor(y));

    return y; // return new subtree root
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* y) {
    AVLNode<Key, Value>* x = y->getLeft(); // x = left child
    AVLNode<Key, Value>* beta = x->getRight(); // beta = subtree between x and y

    x->setRight(y); // y becomes right child of x
    y->setLeft(beta); // beta becomes left child of y

    if (beta != NULL) beta->setParent(y);
    x->setParent(y->getParent());

    // fix parent links
    if (y == this->root_) {
        this->root_ = x; // update root
    } else {
        if (y->getParent()->getLeft() == y)
            y->getParent()->setLeft(x);
        else
            y->getParent()->setRight(x);
    }

    y->setParent(x);

    // update balances
    y->setBalance(getBalanceFactor(y));
    x->setBalance(getBalanceFactor(x));

    return x; // return new subtree root
}

#endif
