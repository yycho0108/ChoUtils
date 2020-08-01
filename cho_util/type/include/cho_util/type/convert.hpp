#pragma once

#include <type_traits>
#include <utility>

namespace cho {
namespace type {

template <typename Source, typename Target, typename = void>
struct Converter {
  // All specializations must conform to the following signature:
  // static void Convert(const Source& source, Target* const target);
};

// Passthrough specialization.
template <typename Source, typename Target>
struct Converter<Source, Target,
                 std::enable_if_t<std::is_same<Source, Target>::value>> {
  static void ConvertTo(Source&& source, Target* const target) {
    *target = std::forward<Source>(source);
  }
};

// Helper class to determine if conversion is defined.
template <typename Source, typename Target>
struct has_converter {
 private:
  // TODO(yycho0108): verify if Source&& below is valid.
  template <typename T>
  static constexpr auto check(T*) -> typename std::is_same<
      decltype(std::declval<T>().ConvertTo(std::declval<Source&&>(),
                                           std::declval<Target* const>())),
      void>::type;

  template <typename>
  static constexpr std::false_type check(...);

  typedef decltype(
      check<Converter<std::decay_t<Source>, std::decay_t<Target>>>(0)) type;

 public:
  static constexpr bool value = type::value;
};
template <typename Source, typename Target>
static constexpr const bool has_converter_v =
    has_converter<Source, Target>::value;

// Functional version.
template <typename Source, typename Target,
          typename = std::enable_if_t<has_converter_v<Source, Target>>>
void Convert(Source&& source, Target* const target) {
  Converter<std::decay_t<Source>, std::decay_t<Target>>::ConvertTo(
      std::forward<Source>(source), target);
}

namespace impl {
class require_deduction_helper {
 protected:
  constexpr require_deduction_helper() {}
};
}  // namespace impl
using require_deduction = impl::require_deduction_helper&;

// Return-value version.
// https://stackoverflow.com/a/58771661
template <
    typename Target, require_deduction...,
    typename = std::enable_if_t<std::is_default_constructible<Target>::value>,
    typename Source>
Target Convert(Source&& source) {
  Target target;
  Convert(std::forward<Source>(source), &target);
  return target;
}

#define CHO_DECLARE_CONVERT(SOURCE, TARGET)                            \
  struct Converter<SOURCE, TARGET> {                                   \
    static void ConvertTo(const SOURCE& source, TARGET* const target); \
  };

#define CHO_DEFINE_CONVERT(SOURCE, TARGET)                                     \
  void ::cho::type::Converter<SOURCE, TARGET>::ConvertTo(const SOURCE& source, \
                                                         TARGET* const target)

#define CHO_DEFINE_CONVERT_ENABLE(SOURCE, TARGET, ENABLE)         \
  void ::cho::type::Converter<SOURCE, TARGET, ENABLE>::ConvertTo( \
      const SOURCE& source, TARGET* const target)

}  // namespace type
}  // namespace cho
