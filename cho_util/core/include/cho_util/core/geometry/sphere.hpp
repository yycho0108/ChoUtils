#pragma once

#include "cho_util/core/geometry/geometry_base.hpp"

#include <Eigen/Core>

namespace cho {
namespace core {

template <typename _Scalar, int N>
class Sphere : public GeometryBase<Sphere<_Scalar, N>> {
  using Scalar = _Scalar;

 public:
  explicit constexpr Sphere() {}
  Sphere(std::initializer_list<Scalar> args) {
    std::copy(args.begin(), args.end(), data_.data());
  }

  const Scalar* GetPtr() const { return data_.data(); }
  Scalar* GetPtr() { return data_.data(); }
  static constexpr int GetSize() { return N + 1; }

  auto& GetData() { return data_; }
  const auto& GetData() const { return data_; }

  auto GetCenter() { return data_.template head<N>(); }
  auto& GetRadius() { return data_(N); }
  const auto GetCenter() const { return data_.template head<N>(); }
  const auto& GetRadius() const { return data_(N); }

  template <typename Derived>
  void SetCenter(const Eigen::DenseBase<Derived>& center) {
    data_.template head<N>() = center;
  }

  void SetRadius(const Scalar radius) { data_(N) = radius; }

 private:
  // [Center, Radius]
  Eigen::Matrix<Scalar, N + 1, 1> data_;

 public:
  friend class GeometryBase<Sphere>;
  // EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <typename _Scalar, int N>
struct GeometryTraits<Sphere<_Scalar, N>> {
  using Scalar = _Scalar;
};

}  // namespace core
}  // namespace cho
