#pragma once

#include "cho_util/vis/queue_io_fwd.hpp"

#include <chrono>
#include <future>

#include "cho_util/core/thread_safe_queue.hpp"
#include "cho_util/vis/io.hpp"

namespace cho_util {
namespace vis {

/**
 * Pipe listener
 */
template <typename DataType>
struct QueueListener : Listener<QueueListener<DataType>> {
 private:
  std::shared_ptr<thread_safe_queue<DataType>> queue;
  bool quit{false};
  std::future<void> worker;
  std::function<bool(DataType&&)> on_data;

 public:
  QueueListener(const std::shared_ptr<thread_safe_queue<DataType>>& queue)
      : queue(queue) {}

  QueueListener(const std::shared_ptr<thread_safe_queue<DataType>>& queue,
                std::function<bool(DataType&&)>& on_data)
      : queue(queue), on_data(on_data) {}

  template <typename Callback>
  void SetCallback(const Callback& on_data) {
    this->on_data = on_data;
  }

  void Start() {
    worker = std::async(std::launch::async, [this] {
      DataType data;
      while (!quit) {
        quit |= OnData(std::move(queue->front(true)));
        queue->pop();
      }
    });
  }

  inline bool OnData(DataType&& data) const { return on_data(std::move(data)); }
  inline bool IsRunning() const { return IsThreadRunning(); }

  inline bool IsThreadRunning() const {
    if (worker.valid()) {
      auto status = worker.wait_for(std::chrono::seconds(0));
      if (status == std::future_status::ready) {
        return false;
      }
      return true;
    }
    return false;
  }

  void Stop() {
    quit = true;
    while (IsThreadRunning()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    worker = std::future<void>();
  }
  ~QueueListener() { Stop(); }
};

/**
 * Lightweight wrapper to handle comm.
 */
template <typename DataType>
struct QueueWriter : Writer<QueueWriter<DataType>> {
 private:
  std::shared_ptr<thread_safe_queue<DataType>> queue;

 public:
  inline QueueWriter(const std::shared_ptr<thread_safe_queue<DataType>>& queue)
      : queue(queue){};

  template <typename T>
  bool Send(const T& data, const bool flush = true) {
    queue->emplace(data);
    if (flush) {
      queue->wait_until_empty();
    }
  }
};

}  // namespace vis
}  // namespace cho_util
