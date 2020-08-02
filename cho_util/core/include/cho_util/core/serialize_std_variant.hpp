#pragma once

#include <variant>

namespace boost {
namespace serialization {

template <class Archive, typename... Ts>
void save(Archive& ar, std::variant<Ts...> const& v, unsigned int /*version*/
) {
  const std::size_t which = v.index();
  ar << which;
  std::visit([&ar](const auto& v) { ar << v; }, v);
}

namespace detail {
template <class Archive, typename Variant, std::size_t... Is>
void load_variant_at_index(Archive& ar, Variant* const v,
                           const std::size_t which,
                           std::index_sequence<Is...>) {
  // 1) default construct variant.
  std::initializer_list<int>(
      {(which == Is ? (*v = std::variant_alternative_t<Is, Variant>()),
        0 : 0)...});
  // 2) load from archive.
  std::visit(
      [&ar](auto& v) {
        using T = std::decay_t<decltype(v)>;
        ar >> v;
      },
      *v);
}
}  // namespace detail

template <class Archive, typename... Ts>
void load(Archive& ar, std::variant<Ts...>& v, const unsigned int version) {
  std::size_t which;
  ar >> which;
  return detail::load_variant_at_index<Archive, std::variant<Ts...>>(
      ar, &v, which, std::make_index_sequence<sizeof...(Ts)>{});
}

template <class Archive, typename... Ts>
inline void serialize(Archive& ar, std::variant<Ts...>& v,
                      const unsigned int file_version) {
  split_free(ar, v, file_version);
}

}  // namespace serialization
}  // namespace boost
