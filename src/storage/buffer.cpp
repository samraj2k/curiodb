//
// Created by Sameer Raj on 15/07/24.
//

#include "../../headers/storage/buffer.h"
#include "../../headers/storage/page.h"
#include <vector>


static BufferMap buffer_map;
static std::vector<BufferId> free_list;
static std::vector<BufferDescriptor> bufferDescriptor(BUFFER_SLOTS);
static std::vector<Page> bufferPool(BUFFER_SLOTS);

namespace buffer {
    static void initBufferMap(int size = BUFFER_SLOTS) {
        buffer_map.reserve(size);
    }


}