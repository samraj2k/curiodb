#ifndef PAGE_H
#define PAGE_H

#include <cstdint>
#include "item.h"
#include "../configs/constants.h"

// A page is a block of memory that is used to store data
// Suppose it is 8 KB, so a char pointer having 1024 * 8 bytes

using PageOffset = std::uint16_t;

using Page = char*;

struct ItemIdData {
    PageOffset dataOffset;;  // offset to tuple (from start of page)
    ItemSize dataLength;  // byte length of tuple
};

struct PageHeaderData {
    PageOffset freeSpaceStart;
    PageOffset freeSpaceEnd;
    PageOffset specialDataSpace;
    ItemIdData itemIds[];
};
using PageHeader = PageHeaderData*;

namespace page {

    inline ObjectSize sizeOfHeader() {
        return sizeof(PageHeaderData);
    }

    inline ObjectSize sizeOfItemIdData() {
        return sizeof(ItemIdData);
    }

    inline uint16_t numOfItems(Page page) {
        auto header = (PageHeader)page;
        return (header->freeSpaceStart - sizeOfHeader()) / sizeof(ItemIdData);
    }

    inline uint16_t getEmptySpace(Page page) {
        auto header = (PageHeader)page;
        return header->freeSpaceEnd - header->freeSpaceStart;
    }

    inline bool isAddable(Page page, size_t sizeOfItem) {
        return getEmptySpace(page) + sizeof(ItemIdData) >= sizeOfItem;
    }

    Page initPage(ObjectSize specialDataLength);
    void addItem(Page page, ItemSize sizeOfItem, Item item);

}

#endif //PAGE_H
