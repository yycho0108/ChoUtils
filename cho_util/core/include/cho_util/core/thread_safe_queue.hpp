#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

template <class T>
struct thread_safe_queue {
 private:
  std::queue<std::shared_ptr<T>> q;
  mutable std::mutex mtx_;
  mutable std::condition_variable has_element_;
  mutable std::condition_variable is_empty_;

 public:
  thread_safe_queue() {}

  // Special accessor!
  void wait_until_empty() {
    std::unique_lock<std::mutex> lock(mtx_);
    is_empty_.wait(lock, [this]() { return q.empty(); });
  }

  // Modifiers
  void push(const T& element) {
    std::unique_lock<std::mutex> lock(mtx_);
    q.push(std::make_shared<T>(element));
    has_element_.notify_all();
  }
  void emplace(const T& element) {
    std::unique_lock<std::mutex> lock(mtx_);
    q.emplace(std::make_shared<T>(element));
    has_element_.notify_all();
  }
  void emplace(T&& element) {
    std::unique_lock<std::mutex> lock(mtx_);
    q.emplace(std::make_shared<T>(element));
    has_element_.notify_all();
  }
  void pop(const bool wait = false) {
    std::unique_lock<std::mutex> lock(mtx_);
    if (wait) {
      has_element_.wait(lock, [this]() { return !q.empty(); });
    }
    q.pop();
    if (q.empty()) {
      is_empty_.notify_all();
    }
  }

  // Accessors
  T& front(const bool wait = false) {
    std::unique_lock<std::mutex> lock(mtx_);
    if (wait) {
      has_element_.wait(lock, [this]() { return !q.empty(); });
    }
    return *q.front();
  }
  const T& front(const bool wait = false) const {
    std::unique_lock<std::mutex> lock(mtx_);
    if (wait) {
      has_element_.wait(lock, [this]() { return !q.empty(); });
    }
    return *q.front();
  }
  T& back() {
    std::unique_lock<std::mutex> lock(mtx_);
    return *q.back();
  }
  const T& back() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return *q.back();
  }
  std::size_t size() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return q.size();
  }
  bool empty() const {
    std::unique_lock<std::mutex> lock(mtx_);
    return q.empty();
  }
};
