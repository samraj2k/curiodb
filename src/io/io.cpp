//
// Created by Sameer Raj on 02/08/24.
//
#include "io/io.h"
#include "configs/constants.h"
#include <fstream>

#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

namespace io {

    Page loadPageFromDisk(const BlockNumber blockNumber, const std::uint64_t fileNumber) {
        const uint64_t fileNumberSuffix = blockNumber / BLOCKS_PER_FILE;
        const uint64_t pageNumber = blockNumber % BLOCKS_PER_FILE;
        const uint64_t pageOffset = pageNumber * PAGE_SIZE;
        const std::string fileName = std::to_string(fileNumber) + "_" + std::to_string(fileNumberSuffix);

        int fd = open(fileName.c_str(), O_RDONLY);
        if (fd == -1) {
            return nullptr;
        }

        void* mappedFile = mmap(nullptr, PAGE_SIZE, PROT_READ, MAP_PRIVATE, fd, pageOffset);
        if (mappedFile == MAP_FAILED) {
            close(fd);
            return nullptr;
        }

        Page page = new char[PAGE_SIZE];
        std::memcpy(page, mappedFile, PAGE_SIZE);

        munmap(mappedFile, PAGE_SIZE);
        close(fd);
        return page;
    }

    void writePageToDisk(const Page page, const BlockNumber blockNumber, const std::uint64_t fileNumber) {
        const uint64_t fileNumberSuffix = blockNumber / BLOCKS_PER_FILE;
        const uint64_t pageNumber = blockNumber % BLOCKS_PER_FILE;
        const uint64_t pageOffset = pageNumber * PAGE_SIZE;
        const std::string fileName = std::to_string(fileNumber) + "_" + std::to_string(fileNumberSuffix);

        int fd = open(fileName.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            return;
        }

        if (lseek(fd, pageOffset + PAGE_SIZE - 1, SEEK_SET) == -1) {
            close(fd);
            return;
        }
        if (write(fd, "", 1) == -1) {
            close(fd);
            return;
        }

        void* mappedFile = mmap(nullptr, PAGE_SIZE, PROT_WRITE, MAP_SHARED, fd, pageOffset);
        if (mappedFile == MAP_FAILED) {
            close(fd);
            return;
        }

        std::memcpy(mappedFile, page, PAGE_SIZE);
        msync(mappedFile, PAGE_SIZE, MS_SYNC);
        munmap(mappedFile, PAGE_SIZE);
        close(fd);
    }

}

