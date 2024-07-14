#ifndef COMMONDEF_H
#define COMMONDEF_H

#include "../locks/spinlock.h"
#include <cstdint>

// common identifier for all objects
// same as postgres, but defined it 64 bit unsigned integer
typedef uint64_t Oid;

class _atomic_int32 {
private:
  Spinlock spinlock;
  // ask compiler for no optimizations
  volatile uint32_t value;

public:
  void modify(uint32_t newValue) {
    spinlock.lock();
    value = newValue;
    spinlock.unlock();
  }
  // access this only after lock
  uint32_t read() { return value; }
};

#endif
