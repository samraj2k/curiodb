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
// 128 MB for 8KB buffer
constexpr int BUFFER_SLOTS = 16000;

struct BufferTag {
    BlockNumber blockNumber;
    uint64_t fileNumber;
};

struct BufferDescriptor {
    BufferDescriptor(const BufferTag &tag, BufferId id)
        : tag(tag),
          id(id),
          pinCount(0),
          usageCount(0),
          isDirty(false),
          contentLock() {
    }

    BufferTag tag;
    BufferId id;
    std::atomic<unsigned int> pinCount;
    std::atomic<unsigned int> usageCount;
    std::atomic<bool> isDirty;
    MonitorRWLock contentLock;
};


struct BufferTagHash {
    std::size_t operator()(const BufferTag& tag) const {
        std::string combined = std::to_string(tag.blockNumber) + "#" + std::to_string(tag.fileNumber);
        return std::hash<std::string>{}(combined);
    }
};

struct BufferTagEqual {
    bool operator()(const BufferTag& lhs, const BufferTag& rhs) const {
        return lhs.blockNumber == rhs.blockNumber && lhs.fileNumber == rhs.fileNumber;
    }
};

using BufferMap = std::unordered_map<BufferTag, BufferId, BufferTagHash, BufferTagEqual>;


namespace buffer {
    void writeToDisk(BufferId bufferId);

    inline void pin(BufferDescriptor* bufferDesc) {
        ++bufferDesc->pinCount;
        ++bufferDesc->usageCount;
    }
}
#endif //BUFFER_H
