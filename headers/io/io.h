//
// Created by Sameer Raj on 02/08/24.
//

#ifndef IO_H
#define IO_H
#include "../storage/page.h"
#include "../storage/block.h"

namespace io {
    Page loadPageFromDisk(BlockNumber blockNumber, std::uint64_t fileNumber);
    void writePageToDisk(Page page, BlockNumber blockNumber, std::uint64_t fileNumber);
}
#endif //IO_H
