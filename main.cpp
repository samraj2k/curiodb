#include <iostream>
#include "headers/storage/page.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    Page::Page page = Page::initPage(0);
    char* testA = "test";
    char* testB = "mysameissameerrajustlonger";
    int sizeB = 26;
    int sizeA = 4;
    Page::addItem(page, sizeA, testA);

    Page::addItem(page, sizeB, testB);

    int totalSize = sizeA + sizeB;
    char last_four[totalSize + 1];
    memcpy(last_four, &page[Page::PAGE_SIZE - totalSize], totalSize);
    last_four[totalSize] = '\0';

    printf("Last 4 characters: %s\n", last_four);
}
