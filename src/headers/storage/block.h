#ifndef BLOCK_H
#define BLOCK_H
#include <cstdint>

// 2^32 * 8 KB = 69 TB memory. Sufficient
typedef uint32_t BlockNumber;
// 2^32 bytes of offset. 2^16 will be only 65 KB which is less;
typedef uint32_t BlockOffset;
class BlockItemId {
  BlockNumber block_id;
  BlockOffset block_offset;
};

#endif
