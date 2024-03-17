#ifndef BUFFER_H
#define BUFFER_H

#include "../utils/commondef.h"
#include "block.h"
#include <cstdint>

class BufferTag {
  // Which database?
  Oid dbId;
  // Which table/index
  Oid relationId;
  // Which block
  // Block means disk reference, page means cached block
  BlockNumber blockNumber;
};

// Think of this as metadata of the pages cached.
//
class BufferDescriptor {
  BufferTag tag;
  uint32_t bufferId;
  // store pin count etc.
  // implemented spin lock for modifications
  _atomic_int32 state;
  // implement a light weight lock/latch for concurrent buffer access below
};

#endif
