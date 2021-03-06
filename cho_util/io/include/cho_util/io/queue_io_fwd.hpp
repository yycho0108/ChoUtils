#pragma once

#include <memory>

namespace cho {
namespace io {

template <typename T>
class QueueListener;

template <typename T>
using QueueListenerPtr = std::shared_ptr<QueueListener<T>>;

template <typename T>
class QueueWriter;

template <typename T>
using QueueWriterPtr = std::shared_ptr<QueueWriter<T>>;

}  // namespace io
}  // namespace cho
