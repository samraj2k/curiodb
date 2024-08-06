//
// Created by Sameer Raj on 12/07/24.
//
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <cstddef>

using ObjectSize = size_t;
using FlagBit = uint8_t;
// in bytes
constexpr uint64_t FILE_SIZE = 1024 * 1024 * 1024;
constexpr uint64_t PAGE_SIZE = 1024 * 8;
constexpr uint64_t BLOCKS_PER_FILE = FILE_SIZE / PAGE_SIZE;
constexpr int BUFFER_SLOTS = 16000;
constexpr int BG_WRITER_INTERVAL = 1000;
constexpr int MAX_FLUSH_PAGES = 5000;

#endif //CONSTANTS_H
