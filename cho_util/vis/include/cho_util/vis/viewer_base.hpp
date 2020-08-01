#pragma once

#include <utility>

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
  bool Render(RenderData&& data) {
    return static_cast<Derived*>(this)->Render(std::forward<RenderData>(data));
  }
};

}  // namespace vis
}  // namespace cho
