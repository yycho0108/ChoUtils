#pragma once

#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>

#include "cho_util/core/geometry/sphere.hpp"
#include "cho_util/vis/vtk_viewer/handler/handler_base.hpp"

namespace cho {
namespace vis {
class SphereHandler : public HandlerBase<SphereHandler> {
 public:
  explicit SphereHandler(const RenderData& rd) {
    const auto& geom = std::get<core::Sphere<float, 3>>(rd.geometry);
    const auto& center = geom.GetCenter();
    const auto& radius = geom.GetRadius();
    // Create a plane
    source_ = vtkNew<vtkSphereSource>();
    source_->SetCenter(center.x(), center.y(), center.z());
    source_->SetRadius(radius);

    // Create a mapper and actor
    mapper_ = vtkNew<vtkPolyDataMapper>();
    mapper_->SetInputConnection(source_->GetOutputPort());

    actor_ = vtkNew<vtkActor>();
    actor_->SetMapper(mapper_);
    if (!rd.color.empty()) {
      constexpr const float iscale = (1.0 / 255.0f);
      actor_->GetProperty()->SetColor(
          iscale * rd.color[0], iscale * rd.color[1], iscale * rd.color[2]);
    }
    switch (rd.representation) {
      case RenderData::Representation::kPoints: {
        actor_->GetProperty()->SetRepresentationToPoints();
        break;
      }
      case RenderData::Representation::kWireframe: {
        actor_->GetProperty()->SetRepresentationToWireframe();
        break;
      }
      case RenderData::Representation::kSurface: {
        actor_->GetProperty()->SetRepresentationToSurface();
        break;
      }
      default: { break; }
    }
    actor_->GetProperty()->SetLineWidth(2);
    actor_->GetProperty()->LightingOff();
    actor_->GetProperty()->SetOpacity(0.125);
  }
  void Update(const RenderData& data) {
    const auto& geom = std::get<core::Sphere<float, 3>>(data.geometry);
    const auto& center = geom.GetCenter();
    const auto& radius = geom.GetRadius();
    // Create a plane
    source_->SetCenter(center.x(), center.y(), center.z());
    source_->SetRadius(radius);
    source_->Modified();
  }

  vtkSmartPointer<vtkActor> GetActor() { return actor_; }
  vtkSmartPointer<const vtkActor> GetActor() const { return actor_; }

 private:
  vtkSmartPointer<vtkSphereSource> source_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor> actor_;
};

}  // namespace vis
}  // namespace cho
