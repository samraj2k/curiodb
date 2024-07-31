#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "../headers/storage/lock.h"

class MonitorRWLockTest : public ::testing::Test {
protected:
    MonitorRWLock rwLock;
};

TEST_F(MonitorRWLockTest, MultipleReadersNoWriter) {
    constexpr int numReaders = 5;
    std::atomic readCount(0);

    std::vector<std::thread> readers;
    readers.reserve(numReaders);
    for (int i = 0; i < numReaders; ++i) {
        readers.emplace_back([this, &readCount, numReaders]() {
            lock::getReadLock(rwLock);
            ++readCount;
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
            EXPECT_EQ(readCount.load(), numReaders);
            lock::releaseReadLock(rwLock);
        });
    }

    for (auto& t : readers) {
        t.join();
    }
}

TEST_F(MonitorRWLockTest, WriterExclusivity) {
    std::atomic<bool> writerActive(false);
    std::atomic<int> readCount(0);

    std::thread writer([this, &writerActive]() {
        lock::getWriteLock(rwLock);
        writerActive = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        writerActive = false;
        lock::releaseWriteLock(rwLock);
    });

    std::vector<std::thread> readers;
    readers.reserve(3);
    for (int i = 0; i < 3; ++i) {
        readers.emplace_back([this, &writerActive, &readCount]() {
            lock::getReadLock(rwLock);
            ++readCount;
            EXPECT_FALSE(writerActive);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            lock::releaseReadLock(rwLock);
            --readCount;
        });
    }

    writer.join();
    for (auto& t : readers) {
        t.join();
    }
}

TEST_F(MonitorRWLockTest, WriterPriority) {
    std::atomic<bool> writerActive(false);
    std::atomic<int> readCount(0);

    std::thread writer([this, &writerActive]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        lock::getWriteLock(rwLock);
        writerActive = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        writerActive = false;
        lock::releaseWriteLock(rwLock);
    });

    std::vector<std::thread> readers;
    readers.reserve(5);
    for (int i = 0; i < 5; ++i) {
        readers.emplace_back([this, &writerActive, &readCount]() {
            lock::getReadLock(rwLock);
            ++readCount;
            EXPECT_FALSE(writerActive);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            lock::releaseReadLock(rwLock);
            --readCount;
        });
    }

    writer.join();
    for (auto& t : readers) {
        t.join();
    }

    EXPECT_EQ(readCount.load(), 0);
}