#include <string>
#include <vector>

class BTreeNodeMem;

class BTreeNodeItem {
public:
  // BTreeNodeItemHeader header;
  /**
   * UTF-8 representation -> 4 bytes
   */
  std::string key;
  /**
   * While writing to the disk this will not waste space
   */
  BTreeNodeMem *child_left;
  BTreeNodeMem *child_right;
  /**
   * In case of leaf node, data page number
   */
  std::string data;

  BTreeNodeItem(std::string key, std::string value, BTreeNodeMem *child_left,
                BTreeNodeMem *child_right) {
    this->key = key;
    this->data = value;
    this->child_left = child_left;
    this->child_right = child_right;
  }

  BTreeNodeItem(std::string key, BTreeNodeMem *child_left,
                BTreeNodeMem *child_right) {
    this->key = key;
    this->child_left = child_left;
    this->child_right = child_right;
  }
};

class BTreeNodeMem {
public:
  /**
   * Node level
   1 for internal
   0 of leaf
   */
  int8_t flags;
  BTreeNodeMem *parent;
  BTreeNodeMem *left_link;
  BTreeNodeMem *right_link;
  std::vector<BTreeNodeItem *> nodeItems;

  BTreeNodeMem() {}
  BTreeNodeMem(int8_t flag, BTreeNodeMem *parent, BTreeNodeMem *left_link,
               BTreeNodeMem *right_link,
               std::vector<BTreeNodeItem *> nodeItems) {
    this->flags = flag;
    this->parent = parent;
    this->left_link = left_link;
    this->right_link = right_link;
    this->nodeItems = nodeItems;
  }
  int getNodeCurrentCapacity() { return nodeItems.size(); }
  int getNodeMaxCapacity() { return 4; }
};
