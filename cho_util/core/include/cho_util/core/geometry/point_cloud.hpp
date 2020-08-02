#pragma once

#include "cho_util/core/geometry/geometry_base.hpp"

#include <Eigen/Core>

namespace cho {
namespace core {

template <typename _Scalar, int N>
class PointCloud : public GeometryBase<PointCloud<_Scalar, N>> {
  using Scalar = _Scalar;

 public:
  explicit constexpr PointCloud() {}
  PointCloud(std::initializer_list<Scalar> args) : data_(args) {}

  const Scalar* GetPtr() const { return data_.data(); }
  Scalar* GetPtr() { return data_.data(); }
  int GetSize() const { return N * data_.cols(); }

  auto& GetData() { return data_; }
  const auto& GetData() const { return data_; }

  int GetNumPoints() const { return data_.cols(); }
  auto GetPoint(const int i) { return data_.col(i); }
  const auto GetPoint(const int i) const { return data_.col(i); }

  void SetNumPoints(const int n) { data_.resize(3, n); }

 private:
  // layout: row=ndim, col=npoints
  Eigen::Matrix<Scalar, N, Eigen::Dynamic> data_;

 public:
  friend class GeometryBase<PointCloud>;
  // EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <typename _Scalar, int N>
struct GeometryTraits<PointCloud<_Scalar, N>> {
  using Scalar = _Scalar;
};

}  // namespace core
}  // namespace cho
