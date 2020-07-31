#pragma once

#include "cho_util/core/geometry/geometry_base.hpp"

#include <Eigen/Core>

namespace cho {
namespace core {

template <typename _Scalar, int N>
class Cuboid : public GeometryBase<Cuboid<_Scalar, N>> {
  using Scalar = _Scalar;

 public:
  template <typename... Args>
  Cuboid(Args&&... args) : data_(args...) {}

  const Scalar* GetPtrImpl() const { return data_.data(); }
  Scalar* GetPtrImpl() { return data_.data(); }
  static constexpr int GetSizeImpl() { return N * 2; }

  auto& GetData() { return data_; }
  const auto& GetData() const { return data_; }

  auto GetMin() { return data_.col(0); }
  auto GetMax() { return data_.col(1); }
  auto GetMin() const { return data_.col(0); }
  auto GetMax() const { return data_.col(1); }

  template <typename Derived>
  auto SetMin(const Eigen::DenseBase<Derived>& d) {
    data_.col(0) = d;
  }
  template <typename Derived>
  auto SetMax(const Eigen::DenseBase<Derived>& d) {
    data_.col(1) = d;
  }

 private:
  // [Center, Radius]
  Eigen::Matrix<Scalar, N, 2> data_;

 public:
  friend class GeometryBase<Cuboid>;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <typename _Scalar, int N>
struct GeometryTraits<Cuboid<_Scalar, N>> {
  using Scalar = _Scalar;
};

}  // namespace core
}  // namespace cho
