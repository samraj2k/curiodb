#include "storage/page.h"
#include "storage/item.h"
#include <cstring>
#include <cassert>

namespace page {

    Page initPage(const ObjectSize specialDataLength) {
        assert(specialDataLength < PAGE_SIZE - sizeOfHeader());
        Page page = new char[PAGE_SIZE];
        auto pageHeader = reinterpret_cast<PageHeader>(page);

        // populate attributes
        pageHeader->specialDataSpace = PAGE_SIZE - specialDataLength;
        pageHeader->freeSpaceStart = sizeOfHeader();
        pageHeader->freeSpaceEnd = pageHeader -> specialDataSpace;
        return page;
    }

    void initPage(Page &page, const ObjectSize specialDataLength) {
        assert(specialDataLength < PAGE_SIZE - sizeOfHeader());

        page = new char[PAGE_SIZE];
        auto pageHeader = reinterpret_cast<PageHeader>(page);
        // populate attributes
        pageHeader->specialDataSpace = PAGE_SIZE - specialDataLength;
        pageHeader->freeSpaceStart = sizeOfHeader();
        pageHeader->freeSpaceEnd = pageHeader -> specialDataSpace;
    }

    void addItem(Page page, const ItemSize sizeOfItem, Item item) {
        assert(isAddable(page, sizeOfItem));
        auto pageHeader = reinterpret_cast<PageHeader>(page);

        // decrease the free space
        pageHeader -> freeSpaceEnd -= sizeOfItem;

        // update info about new item, which starts from freeSpaceStart
        ItemIdData itemIdData = {pageHeader -> freeSpaceEnd, sizeOfItem};
        std::memcpy(page + pageHeader -> freeSpaceStart, &itemIdData, sizeOfItemIdData());

        // move the freeSpaceStart
        pageHeader -> freeSpaceStart += sizeOfItemIdData();

        // copy the item to the page
        std::memcpy(page + itemIdData.dataOffset, item, sizeOfItem);

    }

}
