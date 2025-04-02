#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <functional>
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

    static int computeHeight(AVLNode<Key,Value>* node) {
        if (node == nullptr) return 0;
        return 1 + std::max(computeHeight(node->getLeft()), computeHeight(node->getRight()));
    }

    // Add helper functions here
    AVLNode<Key,Value>* avlInsert(AVLNode<Key,Value>* node, 
        const std::pair<const Key, Value>& new_item, 
        bool &taller);
    // Recursive AVL removal. 'shorter' is set to true if the subtree’s height decreases.
    // 'success' indicates if a node was actually removed.
    AVLNode<Key,Value>* avlRemove(AVLNode<Key,Value>* node, 
            const Key& key, 
            bool &shorter, 
            bool &success);
    // After deletion, rebalance the node if necessary.
    AVLNode<Key,Value>* balanceAfterDeletion(AVLNode<Key,Value>* node, bool &shorter);
    // Rotations.
    AVLNode<Key,Value>* rotateLeft(AVLNode<Key,Value>* x);
    AVLNode<Key,Value>* rotateRight(AVLNode<Key,Value>* y);
};



/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    //std::cout << "Inserting " << new_item.first << std::endl;
    bool taller = false;
    // Cast the root pointer to an AVLNode pointer.
    this->root_ = avlInsert(static_cast<AVLNode<Key,Value>*>(this->root_), new_item, taller);
    if (this->root_ != NULL)
         this->root_->setParent(NULL);

}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    //std::cout << "Removing key " << key << std::endl;
    bool shorter = false, success = false;
    this->root_ = avlRemove(static_cast<AVLNode<Key,Value>*>(this->root_), key, shorter, success);
    if (this->root_ != NULL)
         this->root_->setParent(NULL);
    if(!BinarySearchTree<Key, Value>::isBalanced()){
        std::cout << "Removing key error" << key << std::endl;
        BinarySearchTree<Key, Value>::print();
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
AVLNode<Key,Value>* AVLTree<Key, Value>::avlInsert(AVLNode<Key,Value>* node, 
                                                   const std::pair<const Key, Value>& new_item, 
                                                   bool &taller)
{
    if (node == NULL) {
         taller = true;
         return new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
    }
    if (new_item.first < node->getKey()) {
         AVLNode<Key,Value>* leftChild = avlInsert(static_cast<AVLNode<Key,Value>*>(node->getLeft()), new_item, taller);
         node->setLeft(leftChild);
         leftChild->setParent(node);
         if (taller) {
              node->updateBalance(1); // left subtree grew taller
              if (node->getBalance() == 0)
                  taller = false;
              else if (node->getBalance() == 1)
                  taller = true;
              else if (node->getBalance() == 2) {
                  // Rebalance: Left-Left or Left-Right case.
                  if (static_cast<AVLNode<Key,Value>*>(node->getLeft())->getBalance() < 0)
                      node->setLeft(rotateLeft(static_cast<AVLNode<Key,Value>*>(node->getLeft())));
                  node = rotateRight(node);
                  taller = false;
              }
         }
    } else if (new_item.first > node->getKey()) {
         AVLNode<Key,Value>* rightChild = avlInsert(static_cast<AVLNode<Key,Value>*>(node->getRight()), new_item, taller);
         node->setRight(rightChild);
         rightChild->setParent(node);
         if (taller) {
              node->updateBalance(-1); // right subtree grew taller
              if (node->getBalance() == 0)
                  taller = false;
              else if (node->getBalance() == -1)
                  taller = true;
              else if (node->getBalance() == -2) {
                  // Rebalance: Right-Right or Right-Left case.
                  if (static_cast<AVLNode<Key,Value>*>(node->getRight())->getBalance() > 0)
                      node->setRight(rotateRight(static_cast<AVLNode<Key,Value>*>(node->getRight())));
                  node = rotateLeft(node);
                  taller = false;
              }
         }
    } else {
         // Key already exists; update value.
         node->setValue(new_item.second);
         taller = false;
    }
    return node;
}

template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* x)
{
    AVLNode<Key,Value>* y = static_cast<AVLNode<Key,Value>*>(x->getRight());
    x->setRight(y->getLeft());
    if (y->getLeft() != NULL)
         y->getLeft()->setParent(x);
    y->setParent(x->getParent());
    if (x->getParent() == NULL) {
         // x was root; the caller will update the tree root.
    } else if (x == x->getParent()->getLeft()) {
         x->getParent()->setLeft(y);
    } else {
         x->getParent()->setRight(y);
    }
    y->setLeft(x);
    x->setParent(y);
    // Update balance factors.
    x->setBalance(computeHeight(x->getLeft()) - computeHeight(x->getRight()));
    y->setBalance(computeHeight(y->getLeft()) - computeHeight(y->getRight()));


    return y;
}


template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* y)
{
    AVLNode<Key,Value>* x = static_cast<AVLNode<Key,Value>*>(y->getLeft());
    y->setLeft(x->getRight());
    if (x->getRight() != NULL)
         x->getRight()->setParent(y);
    x->setParent(y->getParent());
    if (y->getParent() == NULL) {
         // y was root.
    } else if (y == y->getParent()->getLeft()) {
         y->getParent()->setLeft(x);
    } else {
         y->getParent()->setRight(x);
    }
    x->setRight(y);
    y->setParent(x);
    x->setBalance(computeHeight(x->getLeft()) - computeHeight(x->getRight()));
    y->setBalance(computeHeight(y->getLeft()) - computeHeight(y->getRight()));
    
    return x;
}


template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key, Value>::avlRemove(AVLNode<Key,Value>* node, 
                                                   const Key& key, 
                                                   bool &shorter, 
                                                   bool &success)
{
    if (node == nullptr) 
    {
        shorter = false; 
        success = false; 
        return nullptr; 
    }

    // RECURSIVE CASE 1: Search in the left subtree if key < node->key.
    if (key < node->getKey()) 
    {
        // Remove from left subtree.
        AVLNode<Key,Value>* leftChild = avlRemove(node->getLeft(), key, shorter, success);
        node->setLeft(leftChild);
        if (leftChild != nullptr) 
        {
            leftChild->setParent(node);
        }

        // If removal in the left subtree made it shorter, update the balance and rebalance.
        if (shorter) 
        {
            // Left subtree got shorter => subtract 1 from balance.
            node->updateBalance(-1);  
            node = balanceAfterDeletion(node, shorter);
        }
    }
    // RECURSIVE CASE 2: Search in the right subtree if key > node->key.
    else if (key > node->getKey()) 
    {
        // Remove from right subtree.
        AVLNode<Key,Value>* rightChild = avlRemove(node->getRight(), key, shorter, success);
        node->setRight(rightChild);
        if (rightChild != nullptr) 
        {
            rightChild->setParent(node);
        }

        // If removal in the right subtree made it shorter, update the balance and rebalance.
        if (shorter) 
        {
            // Right subtree got shorter => add 1 to balance.
            node->updateBalance(1); 
            node = balanceAfterDeletion(node, shorter);
        }
    }
    // RECURSIVE CASE 3: key == node->getKey() => we found the node to remove.
    else 
    {
        success = true;

        // CASE 3A: The node has at most one child (leaf or single child).
        if (node->getLeft() == nullptr || node->getRight() == nullptr) 
        {
            AVLNode<Key,Value>* temp;
            if (node->getLeft() != nullptr)
                temp = node->getLeft();
            else
                temp = node->getRight();
            delete node;  // Remove the current node.
            node = temp;  // The subtree’s new root is the non-null child, or null if none.
            shorter = true; 
        }
        // CASE 3B: The node has two children => must swap with predecessor (or successor).
        else 
        {
            // Find the in-order predecessor (maximum node in the left subtree).
            AVLNode<Key,Value>* pred = node->getLeft();
            while (pred->getRight() != nullptr) 
            {
                pred = pred->getRight();
            }

            // Swap the current node's key/value with the predecessor's key/value.
            this->nodeSwap(node, pred);
            std::swap(node, pred);
            // Now remove the predecessor (which is currently holding the original node's data)
            // from the left subtree.
            AVLNode<Key,Value>* newLeft = avlRemove(node->getLeft(), pred->getKey(), shorter, success);
            node->setLeft(newLeft);
            if (newLeft != nullptr)
            {
                newLeft->setParent(node);
            }

            // If the removal in the left subtree made it shorter, rebalance from here upward.
            if (shorter) 
            {
                node->updateBalance(-1);
                node = balanceAfterDeletion(node, shorter);
            }
        }
    }
    return node;
}

template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key, Value>::balanceAfterDeletion(AVLNode<Key,Value>* node, bool &shorter)
{
    if (node->getBalance() == 2) {
         AVLNode<Key,Value>* leftChild = static_cast<AVLNode<Key,Value>*>(node->getLeft());
         if (leftChild->getBalance() >= 0) {
              node = rotateRight(node);
              if (node->getBalance() == -1)
                  shorter = false;
         } else {
              node->setLeft(rotateLeft(leftChild));
              node = rotateRight(node);
         }
    } else if (node->getBalance() == -2) {
         AVLNode<Key,Value>* rightChild = static_cast<AVLNode<Key,Value>*>(node->getRight());
         if (rightChild->getBalance() <= 0) {
              node = rotateLeft(node);
              if (node->getBalance() == 1)
                  shorter = false;
         } else {
              node->setRight(rotateRight(rightChild));
              node = rotateLeft(node);
         }
    }
    return node;
}

#endif