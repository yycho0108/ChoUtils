#pragma once

#include <memory>
#include <vector>

#include "cho_util/core/serialize_eigen.hpp"

namespace cho {
namespace core {

template <typename Derived>
struct GeometryTraits {};

template <typename Derived>
class GeometryBase {
 public:
  using Scalar = typename GeometryTraits<Derived>::Scalar;
  using Ptr = std::shared_ptr<GeometryBase<Derived>>;

 private:
  explicit constexpr GeometryBase() = default;
  GeometryBase(const GeometryBase&) = default;
  friend Derived;

 public:
  const Scalar* GetPtr() const {
    return static_cast<const Derived*>(this)->GetPtr();
  }
  Scalar* GetPtr() { return static_cast<Derived*>(this)->GetPtr(); }
  int GetSize() const { return static_cast<const Derived*>(this)->GetSize(); }

  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    auto& data = static_cast<Derived*>(this)->data_;
    ar& data;
  }
};

}  // namespace core
}  // namespace cho
