//
// Created by Sameer Raj on 02/08/24.
//
#include "../../headers/io/io.h"
#include "../../headers/configs/constants.h"
#include <fstream>

#include <string>

namespace io {

    Page loadPageFromDisk(const BlockNumber blockNumber, const std::uint64_t fileNumber) {
        const uint64_t fileNumberSuffix = blockNumber / BLOCKS_PER_FILE;
        const uint64_t pageNumber = blockNumber % BLOCKS_PER_FILE;
        const uint64_t pageOffset = pageNumber * PAGE_SIZE;
        const std::string fileName = std::to_string(fileNumber) + "_" + std::to_string(fileNumberSuffix);

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

    void writePageToDisk(const Page page, const BlockNumber blockNumber, const std::uint64_t fileNumber) {
        const uint64_t fileNumberSuffix = blockNumber / BLOCKS_PER_FILE;
        const uint64_t pageNumber = blockNumber % BLOCKS_PER_FILE;
        const uint64_t pageOffset = pageNumber * PAGE_SIZE;
        const std::string fileName = std::to_string(fileNumber) + "_" + std::to_string(fileNumberSuffix);

        std::ofstream file(fileName, std::ios::binary | std::ios::in | std::ios::out);
        if (!file) {
            // If the file doesn't exist, create it
            file.open(fileName, std::ios::binary | std::ios::out);
        }

        if (!file) {
            // Handle file open error
            return;
        }

        file.seekp(pageOffset);
        file.write(page, PAGE_SIZE);

        if (!file) {
            // Handle write error
        }
        file.close();
    }

}

