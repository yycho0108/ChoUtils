#pragma once

#include <memory>

namespace cho {
namespace io {

template <typename T>
class FdListener;

template <typename T>
using FdListenerPtr = std::shared_ptr<FdListener<T>>;

class FdWriter;
using FdWriterPtr = std::shared_ptr<FdWriter>;

}  // namespace io
}  // namespace cho
