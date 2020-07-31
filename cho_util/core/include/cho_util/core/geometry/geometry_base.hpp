#pragma once

#include <memory>
#include <vector>

#include "cho_util/core/serialize_eigen.hpp"

namespace cho {
namespace core {

template <typename Scalar>
class AbstractGeometry {
 public:
  using Ptr = std::shared_ptr<AbstractGeometry>;

 public:
  virtual ~AbstractGeometry() = default;
  virtual const Scalar* GetPtr() const = 0;
  virtual Scalar* GetPtr() = 0;
  virtual int GetSize() const = 0;
};

template <typename Derived>
struct GeometryTraits {};

template <typename Derived>
class GeometryBase
    : public AbstractGeometry<typename GeometryTraits<Derived>::Scalar> {
 public:
  using Scalar = typename GeometryTraits<Derived>::Scalar;
  using Ptr = std::shared_ptr<GeometryBase<Derived>>;

 private:
  GeometryBase() = default;
  GeometryBase(const GeometryBase&) = default;
  friend Derived;

 public:
  virtual const Scalar* GetPtr() const final override {
    return static_cast<const Derived*>(this)->GetPtrImpl();
  }
  virtual Scalar* GetPtr() final override {
    return static_cast<Derived*>(this)->GetPtrImpl();
  }
  virtual int GetSize() const final override {
    return static_cast<const Derived*>(this)->GetSizeImpl();
  }

  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    auto& data = static_cast<Derived*>(this)->data_;
    boost::serialization::serialize_eigen(ar, data, version);
  }
};

}  // namespace core
}  // namespace cho
