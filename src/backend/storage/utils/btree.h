#include "page.h"
#include <cstdint>
#include <vector>

#ifndef BTREE_H
#define BTREE_H

class BTreeNodeHeader {
public:
  /**
   * stores whether root, internal, leaf, deleted
   * 00000001 -> root
   * 00000010 -> internal
   * 00000100 -> leaf
   * 00001000 -> deleted
   * Total size: 2 byte
   */
  std::uint16_t flags;
  /**
   * Tree traversal
   */
  PageNumber parent_link;
  /**
   * Page number of left node on same level
   */
  PageNumber left_link;
  /**
   * Page number of right node on same level
   */
  PageNumber right_link;
};

class BTreeNodeItemHeader {
public:
  PageOffset start_address;
  std::uint16_t size;
};

class BTreeNodeItem {
public:
  BTreeNodeItemHeader header;
  /**
   * UTF-8 representation -> 4 bytes
   */
  std::vector<uint32_t> key;
  /**
   * While writing to the disk this will not waste space
   */
  PageNumber child_left;
  PageNumber child_right;
  /**
   * In case of leaf node, data page number
   */
  PageNumber data;
};

class BTreeNode {
public:
  BTreeNodeHeader header;
  std::vector<BTreeNodeItem> nodeItems;
};

#endif
