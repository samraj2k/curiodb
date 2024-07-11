#include "../../headers/storage/page.h"
#include "../../headers/storage/item.h"

#include <cassert>

namespace Page {

    Page initPage(const size_t special_data_length) {
        assert(special_data_length < PAGE_SIZE - sizeOfHeader());

        // create new page
        Page page = new char[PAGE_SIZE];
        auto page_header = reinterpret_cast<PageHeader>(page);

        // populate attributes
        page_header->special_data_space = PAGE_SIZE - special_data_length;
        page_header->free_space_start = sizeOfHeader();
        page_header->free_space_end = page_header -> special_data_space;
        return page;
    }

    void addItem(Page page, const ItemSize size_of_item, Item item) {
        assert(isAddable(page, size_of_item));
        auto page_header = reinterpret_cast<PageHeader>(page);

        // decrease the free space
        page_header -> free_space_end -= size_of_item;

        // update info about new item, which starts from free_space_start
        ItemIdData item_id_data = {page_header -> free_space_end, size_of_item};
        memcpy(page + page_header -> free_space_start, &item_id_data, sizeof(ItemIdData));

        // move the free_space_start
        page_header -> free_space_start += sizeof(ItemIdData);

        // copy the item to the page
        std::memcpy(page + item_id_data.data_offset, item, size_of_item);

    }

}
