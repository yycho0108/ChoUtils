#pragma once

#include <memory>

namespace cho_util {
namespace vis {

template <typename T>
struct FdListener;

template <typename T>
using FdListenerPtr = std::shared_ptr<FdListener<T>>;

struct FdWriter;
using FdWriterPtr = std::shared_ptr<FdWriter>;

}  // namespace vis
}  // namespace cho_util
