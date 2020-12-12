#pragma once

#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>

#include "cho_util/core/geometry/plane.hpp"
#include "cho_util/vis/vtk_viewer/handler/handler_base.hpp"

namespace cho {
namespace vis {

class PlaneHandler : public HandlerBase<PlaneHandler> {
 public:
  PlaneHandler(const RenderData& data) {
    const auto& geom = std::get<core::Plane<float, 3>>(data.geometry);
    const auto& center = geom.GetCenter();
    const auto& normal = geom.GetNormal();

    // Create a plane
    source_ = vtkNew<vtkPlaneSource>();
    source_->SetCenter(center[0], center[1], center[2]);
    source_->SetNormal(normal[0], normal[1], normal[2]);

    // Create a mapper and actor
    mapper_ = vtkNew<vtkPolyDataMapper>();
    mapper_->SetInputConnection(source_->GetOutputPort());

    actor_ = vtkNew<vtkActor>();
    actor_->SetMapper(mapper_);
    actor_->GetProperty()->SetColor(0.0, 1.0, 1.0);
  }

  void Update(const RenderData& data) {
    const auto& geom = std::get<core::Plane<float, 3>>(data.geometry);
    const auto& center = geom.GetCenter();
    const auto& normal = geom.GetNormal();
    source_->SetCenter(center.x(), center.y(), center.z());
    source_->SetNormal(normal.x(), normal.y(), normal.z());
    source_->Update();
  }
  vtkSmartPointer<vtkActor> GetActor() { return actor_; }
  vtkSmartPointer<const vtkActor> GetActor() const { return actor_; }

 private:
  vtkSmartPointer<vtkPlaneSource> source_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor> actor_;
};
}  // namespace vis
}  // namespace cho
