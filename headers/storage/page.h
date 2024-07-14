#ifndef PAGE_H
#define PAGE_H

#include <cstdint>
#include <vector>
#include "item.h"

namespace Page {
    // A page is a block of memory that is used to store data
    // Suppose it is 8 KB, so a char pointer having 1024 * 8 bytes
    using PageNumber = std::uint32_t;
    using PageOffset = std::uint16_t;
    using TupleLength = std::uint16_t;

    using Page = char*;

    const size_t PAGE_SIZE = 1024 * 8;

    struct ItemIdData {
        PageOffset data_offset;;  // offset to tuple (from start of page)
        TupleLength data_length;  // byte length of tuple
    };

    struct PageHeaderData {
        PageOffset free_space_start;
        PageOffset free_space_end;
        PageOffset special_data_space;
        ItemIdData itemIds[];
    };
    using PageHeader = PageHeaderData*;


    inline size_t sizeOfHeader() {
        return sizeof(PageHeaderData);
    }

    inline uint16_t numOfItems(Page page) {
        auto header = (PageHeader)page;
        return (header->free_space_start - sizeOfHeader()) / sizeof(ItemIdData);
    }

    inline uint16_t getEmptySpace(Page page) {
        auto header = (PageHeader)page;
        return header->free_space_end - header->free_space_start;
    }

    inline bool isAddable(Page page, size_t size_of_item) {
        return getEmptySpace(page) + sizeof(ItemIdData) >= size_of_item;
    }

    Page initPage(size_t special_data_length);
    void addItem(Page page, ItemSize size_of_item, Item item);
}

#endif //PAGE_H
