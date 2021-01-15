#pragma once

#include <utility>

namespace cho {
namespace core {
struct EnumHash {
  template <typename T, std::enable_if_t<std::is_enum_v<T>>* = nullptr>
  std::size_t operator()(const T& t) const {
    return static_cast<std::size_t>(t);
  }
};
}  // namespace core
}  // namespace cho
