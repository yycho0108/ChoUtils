#pragma once

#include <variant>

namespace boost {
namespace serialization {

template <class Archive, typename... Ts>
void save(Archive& ar, std::variant<Ts...> const& v, unsigned int /*version*/
) {
  // NOTE(yycho0108): It is important to explicitly set the datatype of `which`
  // here. Carelessly assigning it to something like an `int` will result in
  // incorrect serialization due to inconsistency between load/save.
  const std::size_t which = v.index();
  ar << which;
  std::visit([&ar](const auto& v) { ar << v; }, v);
}

namespace detail {
template <class Archive, typename Variant, std::size_t... Is>
void load_variant_impl(Archive& ar, Variant* const v, const std::size_t which,
                       std::index_sequence<Is...>) {
  // Default construct the variant and deserialize into concrete type.
  std::initializer_list<int>(
      {(which == Is ? ((void)(*v = std::variant_alternative_t<Is, Variant>()),
                       (void)(ar >> std::get<Is>(*v)), 0)
                    : 0)...});
}
}  // namespace detail

template <class Archive, typename... Ts>
void load(Archive& ar, std::variant<Ts...>& v, const unsigned int /*version*/) {
  std::size_t which;
  ar >> which;
  return detail::load_variant_impl<Archive, std::variant<Ts...>>(
      ar, &v, which, std::make_index_sequence<sizeof...(Ts)>{});
}

template <class Archive, typename... Ts>
inline void serialize(Archive& ar, std::variant<Ts...>& v,
                      const unsigned int file_version) {
  split_free(ar, v, file_version);
}

}  // namespace serialization
}  // namespace boost
