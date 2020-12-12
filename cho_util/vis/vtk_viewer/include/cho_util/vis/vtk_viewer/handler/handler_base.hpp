#pragma once

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkUnsignedCharArray.h>
#include <vtkLookupTable.h>

#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace vis {

template <typename Derived>
class HandlerBase {
 private:
  HandlerBase() = default;
  HandlerBase(const HandlerBase&) = default;
  friend Derived;

 public:
  void Update(const RenderData& data) {
    return static_cast<Derived*>(this)->Update(data);
  }
  vtkSmartPointer<vtkActor> GetActor() {
    return static_cast<Derived*>(this)->GetActor();
  }
  vtkSmartPointer<const vtkActor> GetActor() const {
    return static_cast<Derived*>(this)->GetActor();
  }
};

}  // namespace vis
}  // namespace cho
