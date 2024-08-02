//
// Created by Sameer Raj on 14/07/24.
//

#ifndef LOCK_H
#define LOCK_H
#include <condition_variable>


struct ReadWriteLock {
    std::mutex mtx;
    std::atomic_bool isWriterWaiting{false};
    int readerCount{0};
    std::atomic_bool isWriteLocked{false};
    std::condition_variable readCv;
    std::condition_variable writeCv;
};

namespace lock {
    inline void getReadLock(ReadWriteLock& lock) {
        std::unique_lock lck(lock.mtx);
        lock.readCv.wait(lck, [&lock] { return !lock.isWriteLocked && !lock.isWriterWaiting; });
        ++lock.readerCount;
    }

    inline void releaseReadLock(ReadWriteLock& lock) {
        std::unique_lock lck(lock.mtx);
        --lock.readerCount;
        if(lock.isWriterWaiting) {
            lock.writeCv.notify_one();
        } else {
            lock.readCv.notify_all();
        }
    }

    inline void getWriteLock(ReadWriteLock& lock) {
        std::unique_lock lck(lock.mtx);
        lock.isWriterWaiting = true;
        lock.writeCv.wait(lck, [&lock] { return lock.readerCount == 0 && !lock.isWriteLocked; });
        lock.isWriterWaiting = false;
        lock.isWriteLocked = true;
    }

    inline void releaseWriteLock(ReadWriteLock& lock) {
        std::unique_lock lck(lock.mtx);
        lock.isWriteLocked = false;
        lock.readCv.notify_all();
    }
}
#endif //LOCK_H
