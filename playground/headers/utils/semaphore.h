#include <condition_variable>
#include <mutex>

// implementation from https://zhu45.org/posts/2019/Oct/20/semaphore/
class Semaphore {
private:
  size_t avail;
  std::mutex m;
  std::condition_variable cv;

public:
  // only one thread can call this; by default, we construct a binary semaphore
  explicit Semaphore(int avail_ = 1) : avail(avail_) {}

  void wait() {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [] { return avail > 0; });

    avail--;
  }

  void post() {
    std::unique_lock<std::mutex> lk(m);
    avail++;
    cv.notify_one();
  }

  size_t available() const { return avail; }
};
