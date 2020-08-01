#pragma once

namespace cho {
namespace util {

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

}  // namespace util
}  // namespace cho
