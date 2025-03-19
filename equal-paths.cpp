#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <algorithm>
#endif

#include "equal-paths.h"
using namespace std;

// You may add any prototypes of helper functions here
void findMinMaxDepth(Node* node, int currentDepth, int& minDepth, int& maxDepth);

bool equalPaths(Node* root) {
  if (root == nullptr) {
    return true; // empty tree has equal paths
  }
  
  int minDepth = 1e9;
  int maxDepth = 0;
  
  findMinMaxDepth(root, 1, minDepth, maxDepth);
  
  // if min depth equals max depth, all paths have equal length
  return minDepth == maxDepth;
}

// helper function to find the minimum and maximum depths of leaves
void findMinMaxDepth(Node* node, int currentDepth, int& minDepth, int& maxDepth) {
  if (node == nullptr) {
    return;
  }
  
  // if this is a leaf node
  if (node->left == nullptr && node->right == nullptr) {
    minDepth = min(minDepth, currentDepth);
    maxDepth = max(maxDepth, currentDepth);
    return;
  }
  
  // recursively check both subtrees
  findMinMaxDepth(node->left, currentDepth + 1, minDepth, maxDepth);
  findMinMaxDepth(node->right, currentDepth + 1, minDepth, maxDepth);
}