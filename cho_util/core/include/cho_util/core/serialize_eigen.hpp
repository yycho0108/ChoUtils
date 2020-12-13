#pragma once

#include <Eigen/Core>

#include <boost/serialization/array.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost {
namespace serialization {

template <class Archive, typename Scalar, int... Options>
inline void save(Archive& ar, const Eigen::Matrix<Scalar, Options...>& g,
                 const unsigned int version) {
  int rows = g.rows();
  int cols = g.cols();
  ar& rows;
  ar& cols;
  ar& boost::serialization::make_array(g.data(), rows * cols);
}

template <class Archive, typename Scalar, int... Options>
inline void load(Archive& ar, Eigen::Matrix<Scalar, Options...>& g,
                 const unsigned int version) {
  int rows, cols;
  ar& rows;
  ar& cols;
  g.resize(rows, cols);
  ar& boost::serialization::make_array(g.data(), rows * cols);
}

template <class Archive, typename Scalar, int... Options>
inline void serialize(Archive& ar, Eigen::Matrix<Scalar, Options...>& g,
                      const unsigned int version) {
  boost::serialization::split_free(ar, g, version);
}

}  // namespace serialization
}  // namespace boost
