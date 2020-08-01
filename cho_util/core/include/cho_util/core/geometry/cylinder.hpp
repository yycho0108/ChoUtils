#pragma once

#include "cho_util/core/geometry/geometry_base.hpp"

#include <Eigen/Core>

namespace cho {
namespace core {

template <typename _Scalar>
class Cylinder : public GeometryBase<Cylinder<_Scalar>> {
  using Scalar = _Scalar;

 public:
  template <typename... Args>
  Cylinder(Args&&... args) : data_(args...) {}

  const Scalar* GetPtrImpl() const { return data_.data(); }
  Scalar* GetPtrImpl() { return data_.data(); }
  static constexpr int GetSizeImpl() { return 7; }

  auto& GetData() { return data_; }
  const auto& GetData() const { return data_; }

  auto GetCenter() { return data_.template head<3>(); }
  const auto GetCenter() const { return data_.template head<3>(); }

  auto GetHeight() { return data_(3); }
  const auto GetHeight() const { return data_(3); }

  auto GetRadius() { return data_(4); }
  const auto GetRadius() const { return data_(4); }

 private:
  // [Center, Radius]
  Eigen::Matrix<Scalar, 7, 1> data_;

 public:
  friend class GeometryBase<Cylinder>;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <typename _Scalar>
struct GeometryTraits<Cylinder<_Scalar>> {
  using Scalar = _Scalar;
};

}  // namespace core
}  // namespace cho
