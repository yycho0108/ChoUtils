#pragma once

#include <memory>
namespace cho_util {
namespace vis {

template <typename Derived>
struct Writer;

template <typename Derived>
using WriterPtr = std::shared_ptr<Writer<Derived>>;

template <typename Derived>
struct Listener;

template <typename Derived>
using ListenerPtr = std::shared_ptr<Listener<Derived>>;

}  // namespace vis
}  // namespace cho_util
