#include "rever_thread_pool.h"

ThreadPool::ThreadPool(int size) : stop(false) {
  for (int i = 0; i < size; ++i) {
    threads.emplace_back(std::thread([this, i]() {
      while (true) {
        std::function<void()> task;
        {
          // unique lock 出作用域自动释放
          std::unique_lock<std::mutex> lock(mtx);
          // 进入wait，自动解锁，阻塞，等待唤醒
          cv.wait(lock, [this]() { return stop || !tasks.empty(); });
          // wait返回后又自动加锁
          if (stop && tasks.empty())
            return;
          task = tasks.front();
          tasks.pop();
        }
        task();
      }
    }));
  }
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(mtx);
    // 互斥修改
    stop = true;
  }
  cv.notify_all();
  for (std::thread &th : threads) {
    if (th.joinable())
      th.join();
  }
}

void ThreadPool::add(std::function<void()> func) {
  {
    std::unique_lock<std::mutex> lock(mtx);
    if (stop)
      throw std::runtime_error("threadpool error!\n");
    tasks.emplace(func);
  }
  cv.notify_one();
}