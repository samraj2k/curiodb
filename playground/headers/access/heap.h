#ifndef HEAP_H
#define HEAP_H
#include "../storage/block.h"
#include "../storage/page.h"
#include <cstdint>
#include <vector>

class HeapTupleHeader {
public:
  BlockItemId tup_id;
  uint16_t num_attributes;
  uint8_t state;
  // this size will help to jump from page start to data value
  uint8_t header_size;
  std::vector<uint8_t> null_bitmap;

  // After this data will start, which is not part of this header.
  // But part of this page.
};

#endif
