//
// Created by Sameer Raj on 02/08/24.
//
#include "../../headers/storage/io.h"
#include "../../headers/configs/constants.h"
#include <fstream>

#include <string>

namespace io {
    Page loadPageFromDisk(BlockNumber blockNumber, std::uint64_t fileNumber) {
        uint64_t fileNumberSuffix = blockNumber / BLOCKS_PER_FILE;
        uint64_t pageNumber = blockNumber % BLOCKS_PER_FILE;
        uint64_t pageOffset = pageNumber * PAGE_SIZE;
        std::string fileName = std::to_string(fileNumber) + "_" + std::to_string(fileNumberSuffix);

        std::ifstream file(fileName, std::ios::binary);
        if (!file) {
            return nullptr;
        }

        Page page = new char[PAGE_SIZE];
        file.seekg(pageOffset);
        file.read(page, PAGE_SIZE);

        if (!file) {
            // Handle read error
            delete[] page;
            return nullptr;
        }
        file.close();
        return page;
    }

}

