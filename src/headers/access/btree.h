#ifndef BTREE_H
#define BTREE_H

#include "../storage/block.h"
#include <cstdint>

class BtreeNodeItemHeader {
  // keeping it empty, no header required.
public:
};

class BTreeNodeSpecialData {
public:
  // Make 16 bytes header.
  uint32_t flags;
  BlockNumber bt_node_left;
  BlockNumber bt_node_right;
  uint32_t level;
};
#endif

class BtreeMetaPageData {
public:
  BlockItemId bt_root;
}
