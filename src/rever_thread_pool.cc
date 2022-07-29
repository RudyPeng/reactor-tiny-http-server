#include "rever_thread_pool.h"

ThreadPool::ThreadPool(int size) : stop_(false) {
  for (int i = 0; i < size; ++i) {
    threads_.emplace_back(std::thread([this, i]() {
      while (true) {
        std::function<void()> task;
        {
          // unique lock 出作用域自动释放
          std::unique_lock<std::mutex> lock(mtx_);
          // 进入wait，自动解锁，阻塞，等待唤醒
          cv_.wait(lock, [this]() { return stop_ || !tasks_.empty(); });
          // wait返回后又自动加锁
          if (stop_ && tasks_.empty())
            return;
          task = tasks_.front();
          tasks_.pop();
        }
        task();
      }
    }));
  }
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(mtx_);
    // 互斥修改
    stop_ = true;
  }
  cv_.notify_all();
  for (std::thread &th : threads_) {
    if (th.joinable())
      th.join();
  }
}

void ThreadPool::Add(std::function<void()> func) {
  {
    std::unique_lock<std::mutex> lock(mtx_);
    if (stop_)
      throw std::runtime_error("threadpool error!\n");
    tasks_.emplace(func);
  }
  cv_.notify_one();
}