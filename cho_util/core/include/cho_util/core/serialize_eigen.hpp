#pragma once

#include <Eigen/Core>

#include <boost/serialization/array.hpp>
#include <boost/serialization/split_free.hpp>

namespace details {
template <template <class...> class Z, class, class... Ts>
struct can_apply : std::false_type {};
template <template <class...> class Z, class... Ts>
struct can_apply<Z, std::void_t<Z<Ts...>>, Ts...> : std::true_type {};
}  // namespace details
template <template <class...> class Z, class... Ts>
using can_apply = details::can_apply<Z, void, Ts...>;

template <class Derived>
constexpr std::true_type is_eigen_f(Eigen::EigenBase<Derived> const&) {
  return {};
}

template <class T>
using is_eigen_r = decltype(is_eigen_f(std::declval<T const&>()));

template <class T>
using is_eigen = can_apply<is_eigen_r, T>;

namespace boost {
namespace serialization {

template <class Archive, typename Matrix,
          typename = std::enable_if_t<is_eigen<Matrix>{}>>
inline void save(Archive& ar, const Matrix& g, const unsigned int version) {
  int rows = g.rows();
  int cols = g.cols();
  ar& rows;
  ar& cols;
  ar& boost::serialization::make_array(g.data(), rows * cols);
}

template <class Archive, typename Matrix,
          typename = std::enable_if_t<is_eigen<Matrix>{}>>
inline void load(Archive& ar, Matrix& g, const unsigned int version) {
  int rows, cols;
  ar& rows;
  ar& cols;
  g.resize(rows, cols);
  ar& boost::serialization::make_array(g.data(), rows * cols);
}

template <class Archive, typename Matrix,
          typename = std::enable_if_t<is_eigen<Matrix>{}>>
inline void serialize_eigen(Archive& ar, Matrix& g,
                            const unsigned int version) {
  boost::serialization::split_free(ar, g, version);
}

}  // namespace serialization
}  // namespace boost
