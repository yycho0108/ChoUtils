#pragma once

#include <utility>

#include <Eigen/Geometry>

#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace vis {

template <typename Derived>
class ViewerBase {
 private:
  friend Derived;

 public:
  void Start() { return static_cast<Derived*>(this)->Start(); }
  void Step() { return static_cast<Derived*>(this)->Step(); }
  void Spin() { return static_cast<Derived*>(this)->Spin(); }
  void SetCameraPose(const Eigen::Isometry3f& pose) {
    return static_cast<Derived*>(this)->SetCameraPose();
  }
  bool Render(RenderData&& data) {
    return static_cast<Derived*>(this)->Render(std::forward<RenderData>(data));
  }
};

}  // namespace vis
}  // namespace cho
