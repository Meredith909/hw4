#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream> 
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkEqualPaths(Node* node, int depth, int &firstLeafDepth);
bool equalPaths(Node * root)
{
    // Add your code below
    // The equalPaths function that the test harness will call.

    // We initialize firstLeafDepth to -1 to indicate that no leaf has been encountered yet.
    int firstLeafDepth = -1;
    // Start the recursive check from the root at depth 0.
    return checkEqualPaths(root, 0, firstLeafDepth);
}

bool checkEqualPaths(Node* node, int depth, int &firstLeafDepth) {
    // Base case: if the node is NULL, there is nothing to check, return true.
    if (!node)
        return true;
    
    // Check if the node is a leaf (no left or right children)
    if (!node->left && !node->right) {
        // If this is the first leaf encountered, record its depth.
        if (firstLeafDepth == -1) {
            firstLeafDepth = depth;
            return true;
        }
        // For subsequent leaves, check if their depth matches the first leaf's depth.
        return (depth == firstLeafDepth);
    }
    
    // Recursively check both left and right subtrees.
    bool leftEqual = checkEqualPaths(node->left, depth + 1, firstLeafDepth);
    bool rightEqual = checkEqualPaths(node->right, depth + 1, firstLeafDepth);
    
    // Return true if both subtrees have equal leaf path lengths.
    return leftEqual && rightEqual;
}

