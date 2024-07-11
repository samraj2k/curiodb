#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <atomic>
#include <immintrin.h>
// https://rigtorp.se/spinlock/
// source:
// https://probablydance.com/2019/12/30/measuring-mutexes-spinlocks-and-how-bad-the-linux-scheduler-really-is/
// Picked up the implementation which i understood
/*
When a core needs to read some data, it first checks if the cache line
containing that data is present in its cache. If not, it loads the cache line in
the Shared state from main memory into its cache. This allows multiple cores to
have read-only copies of the same data in their caches simultaneously without
any communication between them.

However, if a core needs to modify the data, it must first acquire exclusive
ownership of that cache line by transitioning it to the Modified or Exclusive
state. This typically involves invalidating the copies in other caches through
inter-cache communication to maintain coherence.

So yes, the Shared state in the MESI protocol allows multiple cores to read the
same data from their local caches without communication until a modification is
required, at which point the protocol ensures coherence by transferring
exclusive ownership.
*/
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
