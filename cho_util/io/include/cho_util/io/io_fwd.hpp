#pragma once

#include <memory>
namespace cho {
namespace io {

struct Writer;
using WriterPtr = std::shared_ptr<Writer>;

template <typename DataType>
struct Listener;

template <typename DataType>
using ListenerPtr = std::shared_ptr<Listener<DataType>>;

}  // namespace io
}  // namespace cho
