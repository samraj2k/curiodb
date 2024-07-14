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

enum BufferPageStatus {
  IS_DIRTY,
  CHECKPOINT_NEEDED,
  IS_LOCKED,
  IO_IN_PROGRESS
};

class BufferState {
  uint32_t pinCount;
  BufferPageStatus bufferPageStatus;
};

class BufferDescriptor {
  // Tag used to determine this buffer
  BufferTag tag;
  // Index of the buffer
  uint32_t bufferId;
  // store pin count etc.
  // implemented spin lock for modifications
  _atomic_var<BufferState> state;
  // implement a light weight lock/latch for concurrent buffer access below
};

#endif
