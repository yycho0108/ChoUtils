#pragma once

#include "cho_util/core/geometry/geometry_base.hpp"

#include <Eigen/Core>

namespace cho {
namespace core {

template <typename _Scalar, int N>
class Line : public GeometryBase<Line<_Scalar, N>> {
  using Scalar = _Scalar;

 public:
  template <typename... Args>
  Line(Args&&... args) : data_(args...) {}

  const Scalar* GetPtrImpl() const { return data_.data(); }
  Scalar* GetPtrImpl() { return data_.data(); }
  static constexpr int GetSizeImpl() { return N * 2; }

  auto& GetData() { return data_; }
  const auto& GetData() const { return data_; }

 private:
  Eigen::Matrix<Scalar, N, 2> data_;

 public:
  friend class GeometryBase<Line>;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <typename _Scalar, int N>
struct GeometryTraits<Line<_Scalar, N>> {
  using Scalar = _Scalar;
};

template <typename _Scalar, int N>
class Lines : public GeometryBase<Lines<_Scalar, N>> {
  using Scalar = _Scalar;

 public:
  template <typename... Args>
  Lines(Args&&... args) : data_(args...) {}

  const Scalar* GetPtrImpl() const { return data_.data(); }
  Scalar* GetPtrImpl() { return data_.data(); }
  int GetSizeImpl() const { return data_.rows() * 2; }

  auto& GetData() { return data_; }
  const auto& GetData() const { return data_; }

 private:
  // layout = (num lines * ndim), 2
  Eigen::Matrix<Scalar, Eigen::Dynamic, 2> data_;

 public:
  friend class GeometryBase<Lines>;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <typename _Scalar, int N>
struct GeometryTraits<Lines<_Scalar, N>> {
  using Scalar = _Scalar;
};

}  // namespace core
}  // namespace cho
