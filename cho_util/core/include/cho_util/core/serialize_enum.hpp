#pragma once

#include <utility>

namespace boost {
namespace serialization {

template <class Archive, typename Enum,
          typename std::enable_if_t<std::is_enum_v<Enum>>* = nullptr>
void save(Archive& ar, Enum& v, unsigned int /*version*/
) {
  auto& vv = reinterpret_cast<typename std::underlying_type<Enum>::type&>(v);
  ar << vv;
}

template <class Archive, typename Enum,
          typename std::enable_if_t<std::is_enum_v<Enum>>* = nullptr>
void load(Archive& ar, Enum& v, const unsigned int /*version*/) {
  auto& vv = reinterpret_cast<typename std::underlying_type<Enum>::type&>(v);
  ar >> vv;
}

template <class Archive, typename Enum,
          typename std::enable_if_t<std::is_enum_v<Enum>>* = nullptr>
inline void serialize(Archive& ar, Enum& v, const unsigned int file_version) {
  split_free(ar, v, file_version);
}

}  // namespace serialization
}  // namespace boost
