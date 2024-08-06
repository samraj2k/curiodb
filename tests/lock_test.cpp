#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "lock/lock.h"

class ReadWriteLockTest : public ::testing::Test {
protected:
    ReadWriteLock rwLock;
};

TEST_F(ReadWriteLockTest, MultipleReadersNoWriter) {
    constexpr int numReaders = 5;
    std::atomic readCount(0);

    std::vector<std::thread> readers;
    readers.reserve(numReaders);
    for (int i = 0; i < numReaders; ++i) {
        readers.emplace_back([this, &readCount, numReaders]() {
            rwLock.getReadLock();
            ++readCount;
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
            EXPECT_EQ(readCount.load(), numReaders);
            rwLock.releaseReadLock();
        });
    }

    for (auto& t : readers) {
        t.join();
    }
}

TEST_F(ReadWriteLockTest, WriterExclusivity) {
    std::atomic<bool> writerActive(false);
    std::atomic<int> readCount(0);

    std::thread writer([this, &writerActive]() {
        rwLock.getWriteLock();
        writerActive = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        writerActive = false;
        rwLock.releaseWriteLock();
    });

    std::vector<std::thread> readers;
    readers.reserve(3);
    for (int i = 0; i < 3; ++i) {
        readers.emplace_back([this, &writerActive, &readCount]() {
            rwLock.getReadLock();
            ++readCount;
            EXPECT_FALSE(writerActive);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            rwLock.releaseReadLock();
            --readCount;
        });
    }

    writer.join();
    for (auto& t : readers) {
        t.join();
    }
}

TEST_F(ReadWriteLockTest, WriterPriority) {
    std::atomic<bool> writerActive(false);
    std::atomic<int> readCount(0);

    std::thread writer([this, &writerActive]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        rwLock.getWriteLock();
        writerActive = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        writerActive = false;
        rwLock.releaseWriteLock();
    });

    std::vector<std::thread> readers;
    readers.reserve(5);
    for (int i = 0; i < 5; ++i) {
        readers.emplace_back([this, &writerActive, &readCount]() {
            rwLock.getReadLock();
            ++readCount;
            EXPECT_FALSE(writerActive);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            rwLock.releaseReadLock();
            --readCount;
        });
    }

    writer.join();
    for (auto& t : readers) {
        t.join();
    }

    EXPECT_EQ(readCount.load(), 0);
}