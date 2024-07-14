#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <atomic>
#include <immintrin.h>

// source:
// https://probablydance.com/2019/12/30/measuring-mutexes-spinlocks-and-how-bad-the-linux-scheduler-really-is/
// Picked up the implementation which i understood
class Spinlock {
private:
  // lock variable
  std::atomic<bool> locked = false;

public:
  void lock() {
    while (true) {
      bool was_locked = locked.load(std::memory_order_relaxed);
      // use compare_and_swap (CAS) instead of test_and_set
      // help in cache optimization
      if (!was_locked && locked.compare_exchange_weak(
                             was_locked, true, std::memory_order_acquire))
        break;
      // pause so as CPU decreases it priority and other threds are picked up
      // else delay
      _mm_pause();
    }
  }
  void unlock() { locked.store(false, std::memory_order_release); }
};

#endif
