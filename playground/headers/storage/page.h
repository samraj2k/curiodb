#ifndef PAGE_H
#define PAGE_H
#include <cstdint>
#include <vector>

// This represents a page, which will be usually 8192 bytes
typedef char *Page;
typedef uint16_t PageOffset;

class ItemPointer {
public:
  uint16_t data_offset;
  uint16_t data_length;
  uint8_t state;
};
class PageHeader {
  // This is generic page header
  // Will be used by heap and all the indexes.
  // 8 bytes of header
public:
  /**
   // PageFull
   // Page can accomodate stuff
   */
  uint16_t flags;
  /**
   * Page number of left node on same level
   */
  PageOffset freeSpaceStart;

  PageOffset free_space_end;

  // here the BTreeNodeSpecialData will be stored.
  // The reason it is not in the begining because its length may vary
  PageOffset special_data_space;
  // points to headers of the index or heap.
  // Incase of heap, will point to HeapTupleHeader
  std::vector<ItemPointer> itemPointers;
};
#endif
