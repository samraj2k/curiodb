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

}
#endif //BUFFER_H
