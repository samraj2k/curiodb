#include "utils/btreememory.h"
#include <cstddef>
#include <string>
#include <vector>

BTreeNodeMem *insertRoot(int flag = 0) {
  std::vector<BTreeNodeItem *> nodeItemVector;
  BTreeNodeMem *node =
      new BTreeNodeMem(flag, 0, NULL, NULL, NULL, nodeItemVector);
  return node;
}

int insertDataItemInANode(BTreeNodeMem *node, BTreeNodeItem *nodeItem) {
  std::vector<BTreeNodeItem *> nodeItems = (*node).nodeItems;
  std::vector<BTreeNodeItem *>::iterator it = std::lower_bound(
      nodeItems.begin(), nodeItems.end(), nodeItem,
      [](BTreeNodeItem *a, BTreeNodeItem *b) { return a->key < b->key; });
  nodeItems.insert(it, nodeItem);

  return it - nodeItems.begin();
}

void appendInParent(BTreeNodeMem *node, BTreeNodeMem *left, BTreeNodeMem *right,
                    std::string key) {
  if (node->parent == NULL) {
    node->parent = insertRoot(1);
  }
  BTreeNodeItem *nodeItem = new BTreeNodeItem(key, NULL, left, right);
  int index = insertDataItemInANode(node->parent, nodeItem);
  if (index - 1 >= 0) {

    ((node->parent)->nodeItems[index - 1])->child_right = left;
  }
  if (index + 1 < node->parent->nodeItems.size()) {
    ((node->parent)->nodeItems[index + 1])->child_left = right;
  }
  return;
}

bool splitNodeAndUpdateParent(BTreeNodeMem *node) {
  BTreeNodeMem *newNode = new BTreeNodeMem();
  // leaf
  newNode->flags = node->flags;
  newNode->left_link = node;
  newNode->parent = node->parent;
  BTreeNodeMem *right_link_temp = node->right_link;
  node->right_link = newNode;
  newNode->right_link = right_link_temp;
  std::vector<BTreeNodeItem *> nodeItems = node->nodeItems;
  // one extra element will be left, to send above
  int elementsInSecondNode = (nodeItems.size() + (node->flags == 0)) / 2;
  std::vector<BTreeNodeItem *> secondHalf(elementsInSecondNode, NULL);
  while (elementsInSecondNode--) {
    secondHalf[elementsInSecondNode] = nodeItems.back();
    nodeItems.pop_back();
  }
  appendInParent(node->parent, node, newNode, nodeItems.back()->key);
  if (node->flags != 0) {
    nodeItems.pop_back();
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
    std::vector<BTreeNodeItem *> nodeItems = node->nodeItems;
    bool isSplit;
    if (key >= nodeItems.back()->key) {
      isSplit = insert(node->right_link, key, value);
    } else {
      // somewhere in the middle
      for (int i = 0; i < nodeItems.size(); i++) {
        if (key < nodeItems[i]->key) {
          isSplit = insert(node->left_link, key, value);
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
