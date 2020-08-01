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

  const Scalar* GetPtr() const { return data_.data(); }
  Scalar* GetPtr() { return data_.data(); }
  static constexpr int GetSize() { return N * 2; }

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
  Lines() {}
  Lines(std::initializer_list<Scalar> args) : data_(args) {}

  const Scalar* GetPtr() const { return data_.data(); }
  Scalar* GetPtr() { return data_.data(); }
  int GetSize() const { return data_.cols() * N; }

  auto& GetData() { return data_; }
  const auto& GetData() const { return data_; }

  int GetNumLines() const { return data_.cols() / 2; }
  int GetNumPoints() const { return data_.cols(); }

  auto GetLine(const int i) { return data_.template block<N, 2>(0, i * 2); }
  const auto GetLine(const int i) const {
    return data_.template block<N, 2>(0, i * 2);
  }

  auto GetSourcePoint(const int i) { return data_.col(i * 2); }
  const auto GetSourcePoint(const int i) const { return data_.col(i * 2); }
  auto GetTargetPoint(const int i) { return data_.col(i * 2 + 1); }
  const auto GetTargetPoint(const int i) const { return data_.col(i * 2 + 1); }

  auto GetPoint(const int i) { return data_.col(i); }
  const auto GetPoint(const int i) const { return data_.col(i); }

  void SetNumLines(const int n) { data_.resize(N, n * 2); }

 private:
  // layout = NDIM, (NLINES * 2)
  Eigen::Matrix<Scalar, N, Eigen::Dynamic> data_;

 public:
  friend class GeometryBase<Lines>;
  // EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <typename _Scalar, int N>
struct GeometryTraits<Lines<_Scalar, N>> {
  using Scalar = _Scalar;
};

}  // namespace core
}  // namespace cho
