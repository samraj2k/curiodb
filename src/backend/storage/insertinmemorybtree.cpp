#include "utils/btreememory.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
BTreeNodeMem *insertRoot(int flag = 0) {
  std::vector<BTreeNodeItem *> nodeItemVector;
  BTreeNodeMem *node = new BTreeNodeMem(flag, NULL, NULL, NULL, nodeItemVector);
  return node;
}
int keyCompare(string a, string b) {
  int aVal = std::stoi(a);
  int bVal = std::stoi(b);
  return aVal - bVal;
}
int insertDataItemInANode(BTreeNodeMem *node, BTreeNodeItem *nodeItem) {
  std::vector<BTreeNodeItem *> &nodeItems = (*node).nodeItems;
  std::vector<BTreeNodeItem *>::iterator it =
      std::lower_bound(nodeItems.begin(), nodeItems.end(), nodeItem,
                       [](BTreeNodeItem *a, BTreeNodeItem *b) {
                         int aVal = std::stoi(a->key);
                         int bVal = std::stoi(b->key);
                         return aVal < bVal;
                         // return a->key < b->key;
                       });
  int indexInserted = it - nodeItems.begin();
  nodeItems.insert(it, nodeItem);
  return indexInserted;
}

void appendInParent(BTreeNodeMem *left, BTreeNodeMem *right, std::string key) {
  // till  here both left and right parent are pointing same.
  // if the root does not exist

  if (left->parent == NULL) {
    left->parent = insertRoot(1);
    right->parent = left->parent;
  }
  BTreeNodeItem *nodeItem = new BTreeNodeItem(key, "", left, right);
  int index = insertDataItemInANode(left->parent, nodeItem);
  if (index - 1 >= 0) {
    ((left->parent)->nodeItems[index - 1])->child_right = left;
  }
  if (index + 1 < left->parent->nodeItems.size()) {
    ((left->parent)->nodeItems[index + 1])->child_left = right;
  }
  return;
}

bool splitNodeAndUpdateParent(BTreeNodeMem *node) {
  BTreeNodeMem *right_link_temp = node->right_link;
  std::vector<BTreeNodeItem *> &nodeItems = node->nodeItems;
  // one extra element will be left, to send above
  int elementsInSecondNode = (nodeItems.size() + (node->flags == 0)) / 2;
  std::vector<BTreeNodeItem *> secondHalf(elementsInSecondNode, NULL);
  while (elementsInSecondNode--) {
    secondHalf[elementsInSecondNode] = nodeItems.back();
    nodeItems.pop_back();
  }
  BTreeNodeMem *newNode = new BTreeNodeMem(node->flags, node->parent, node,
                                           node->right_link, secondHalf);
  node->right_link = newNode;
  if (node->flags != 0) {
    appendInParent(node, newNode, nodeItems.back()->key);
    nodeItems.pop_back();
    for (auto to : newNode->nodeItems) {
      to->child_left->parent = newNode;
      to->child_right->parent = newNode;
    }
  } else {
    appendInParent(node, newNode, newNode[0].nodeItems[0]->key);
  }

  return true;
}

bool insertInLeaf(BTreeNodeMem *node, std::string key, std::string value) {
  BTreeNodeItem *newItem = new BTreeNodeItem(key, value, NULL, NULL);
  int insertedIndex = insertDataItemInANode(node, newItem);
  if ((*node).getNodeCurrentCapacity() > (*node).getNodeMaxCapacity()) {
    return splitNodeAndUpdateParent(node);
  } else {
    return false;
  }
}

bool checkInternalNodeAndSplit(BTreeNodeMem *node) {
  if ((*node).getNodeCurrentCapacity() > (*node).getNodeMaxCapacity()) {
    return splitNodeAndUpdateParent(node);
  } else {
    return false;
  }
}

bool insert(BTreeNodeMem *node, std::string key, std::string value) {
  if (node == NULL) {
    node = insertRoot();
  }
  if (node->flags == 0) {
    return insertInLeaf(node, key, value);
  } else {
    std::vector<BTreeNodeItem *> &nodeItems = node->nodeItems;
    bool isSplit;
    if (keyCompare(key, nodeItems.back()->key) >= 0) {
      isSplit = insert(nodeItems.back()->child_right, key, value);
    } else {
      // somewhere in the middle
      for (int i = 0; i < nodeItems.size(); i++) {
        if (keyCompare(key, nodeItems[i]->key) < 0) {
          isSplit = insert(nodeItems[i]->child_left, key, value);
          break;
        }
      }
    }
    if (isSplit) {
      return checkInternalNodeAndSplit(node);
    } else
      return false;
  }
}

void print(BTreeNodeMem *node) {
  if (node == NULL) {
    return;
  }
  for (int i = 0; i < node->nodeItems.size(); i++) {
    print(node->nodeItems[i]->child_left);
    std::cout << node->nodeItems[i]->key << " ";
  }
  print(node->nodeItems.back()->child_right);
}
