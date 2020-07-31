#pragma once

#include "cho_util/core/geometry/geometry_base.hpp"

#include <Eigen/Core>

namespace cho {
namespace core {

template <typename _Scalar, int N>
class PointCloud : public GeometryBase<PointCloud<_Scalar, N>> {
  using Scalar = _Scalar;

 public:
  template <typename... Args>
  PointCloud(Args&&... args) : data_(args...) {}

  const Scalar* GetPtrImpl() const { return data_.data(); }
  Scalar* GetPtrImpl() { return data_.data(); }
  int GetSizeImpl() const { return N * data_.cols(); }

  auto& GetData() { return data_; }
  const auto& GetData() const { return data_; }

  int GetNumPoints() const { return data_.cols(); }

 private:
  // layout: row=ndim, col=npoints
  Eigen::Matrix<Scalar, N, Eigen::Dynamic> data_;

 public:
  friend class GeometryBase<PointCloud>;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <typename _Scalar, int N>
struct GeometryTraits<PointCloud<_Scalar, N>> {
  using Scalar = _Scalar;
};

}  // namespace core
}  // namespace cho
