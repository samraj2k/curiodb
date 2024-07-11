#ifndef COMMONDEF_H
#define COMMONDEF_H

#include "./semaphore.h"
#include <cstdint>

// common identifier for all objects
// same as postgres, but defined it 64 bit unsigned integer
typedef uint64_t Oid;

template <typename T> class AtomicVariable {
private:
  Semaphore semaphore;
  // take spin lock and modify the value
  T value;

public:
  void modify(T newValue) {
    semaphore.wait();
    value = newValue;
    semaphore.post();
  }
  // access this only after lock
  T read() { return value; }
};

#endif
