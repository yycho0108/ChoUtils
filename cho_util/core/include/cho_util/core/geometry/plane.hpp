#pragma once

#include "cho_util/core/geometry/geometry_base.hpp"

#include <Eigen/Core>

namespace cho {
namespace core {

template <typename _Scalar, int N>
class Plane : public GeometryBase<Plane<_Scalar, N>> {
  using Scalar = _Scalar;

 public:
  explicit constexpr Plane() {}
  Plane(std::initializer_list<Scalar> args) : data_(args) {}

  const Scalar* GetPtr() const { return data_.data(); }
  Scalar* GetPtr() { return data_.data(); }
  static constexpr int GetSize() { return N * 2; }

  auto& GetData() { return data_; }
  const auto& GetData() const { return data_; }

  auto GetCenter() { return data_.col(0); }
  auto GetNormal() { return data_.col(1); }
  const auto GetCenter() const { return data_.col(0); }
  const auto GetNormal() const { return data_.col(1); }

 private:
  Eigen::Matrix<Scalar, N, 2> data_;

 public:
  friend class GeometryBase<Plane>;
  // EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <typename _Scalar, int N>
struct GeometryTraits<Plane<_Scalar, N>> {
  using Scalar = _Scalar;
};

}  // namespace core
}  // namespace cho
