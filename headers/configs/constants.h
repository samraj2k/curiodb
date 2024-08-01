//
// Created by Sameer Raj on 12/07/24.
//
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <cstddef>

using ObjectSize = size_t;
using FlagBit = uint8_t;
// in bytes
const uint64_t FILE_SIZE = 1024 * 1024 * 1024;
const uint64_t PAGE_SIZE = 1024 * 8;
const uint64_t BLOCKS_PER_FILE = FILE_SIZE / PAGE_SIZE;
#endif //CONSTANTS_H
