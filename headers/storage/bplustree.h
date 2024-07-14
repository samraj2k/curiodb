#include "page.h"
#ifndef BPLUSTREE_H
#define BPLUSTREE_H

struct BTreeFlags {
    uint8_t isEmpty : 1;
    uint8_t isLeaf : 1;
    uint8_t isRoot : 1;
    uint8_t reserved : 5; // Reserved for future use
};

class BTreeNode: public Page {
public:
    void init_new_page() override {

    }
    BTreeFlags flags;
    PageNumber left_link;
    PageNumber right_link;
};
#endif
