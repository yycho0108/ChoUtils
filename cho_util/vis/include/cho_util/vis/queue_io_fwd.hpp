#pragma once

#include <memory>

namespace cho_util {
namespace vis {

template <typename T>
struct QueueListener;

template <typename T>
using QueueListenerPtr = std::shared_ptr<QueueListener<T>>;

template <typename T>
struct QueueWriter;

template <typename T>
using QueueWriterPtr = std::shared_ptr<QueueWriter<T>>;

}  // namespace vis
}  // namespace cho_util
