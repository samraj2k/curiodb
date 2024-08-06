//
// Created by Sameer Raj on 14/07/24.
//

#ifndef LOCK_H
#define LOCK_H
#include <condition_variable>


struct ReadWriteLock {
    std::mutex mtx;
    int waitingWritersCount{0};
    int readerCount{0};
    std::atomic_bool isWriteLocked{false};
    std::condition_variable readCv;
    std::condition_variable writeCv;

     void getReadLock() {
        std::unique_lock lck(mtx);
        readCv.wait(lck, [this] { return !isWriteLocked && !waitingWritersCount; });
        ++readerCount;
    }

     void releaseReadLock() {
        std::unique_lock lck(mtx);
        --readerCount;
        if(waitingWritersCount) {
            writeCv.notify_one();
        } else {
            readCv.notify_all();
        }
    }

     void getWriteLock() {
        std::unique_lock lck(mtx);
        ++waitingWritersCount;
        writeCv.wait(lck, [this] { return readerCount == 0 && !isWriteLocked; });
        isWriteLocked = true;
        --waitingWritersCount;
    }

     void releaseWriteLock() {
        std::unique_lock lck(mtx);
        isWriteLocked = false;
        readCv.notify_all();
        if(waitingWritersCount) {
            writeCv.notify_one();
        }
    }
};

#endif //LOCK_H