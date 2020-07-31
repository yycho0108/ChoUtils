#pragma once

#include "cho_util/core/geometry/geometry_base.hpp"

#include <Eigen/Core>

namespace cho {
namespace core {

template <typename _Scalar, int N>
class Point : public GeometryBase<Point<_Scalar, N>> {
  using Scalar = _Scalar;

 public:
  template <typename... Args>
  Point(Args&&... args) : data_(args...) {}

  const Scalar* GetPtrImpl() const { return data_.data(); }
  Scalar* GetPtrImpl() { return data_.data(); }
  static constexpr int GetSize() { return N; }

  auto& GetData() { return data_; }
  const auto& GetData() const { return data_; }

 private:
  Eigen::Matrix<Scalar, N, 1> data_;

 public:
  friend class GeometryBase<Point>;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

}  // namespace core
}  // namespace cho
