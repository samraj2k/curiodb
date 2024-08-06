#include <iostream>
#include <gtest/gtest.h>
#include "headers/storage/page.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    Page page = page::initPage(0);
    char* testA = "test";
    char* testB = "mysameissameerrajustlonger";
    int sizeB = 26;
    int sizeA = 4;
    page::addItem(page, sizeA, testA);

    page::addItem(page, sizeB, testB);

    int totalSize = sizeA + sizeB;
    char last_four[totalSize + 1];
    memcpy(last_four, &page[PAGE_SIZE - totalSize], totalSize);
    last_four[totalSize] = '\0';

    printf("Last 4 characters: %s\n", last_four);

    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
