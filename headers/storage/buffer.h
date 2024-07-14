//
// Created by Sameer Raj on 13/07/24.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <atomic>
#include <unordered_map>

#include "block.h"
#include "lock.h"

using BufferId = unsigned int;
// 128 KB for 8KB buffer
constexpr int BUFFER_SLOTS = 16000;

struct BufferTag {
    BlockNumber blockNumber;
    unsigned int fileNumber;
};

struct BufferDescriptor {
    BufferTag tag;
    BufferId id;
    std::atomic<unsigned int> pinCount;
    bool isDirty;
    MonitorRWLock contentLock;
};

using BufferMap = std::unordered_map<BufferTag, BufferId>;

namespace Buffer {

}
#endif //BUFFER_H
