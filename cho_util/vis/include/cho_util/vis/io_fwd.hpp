#pragma once

#include <memory>
namespace cho_util {
namespace vis {

struct Writer;
using WriterPtr = std::shared_ptr<Writer>;

template <typename DataType>
struct Listener;

template <typename DataType>
using ListenerPtr = std::shared_ptr<Listener<DataType>>;

}  // namespace vis
}  // namespace cho_util
