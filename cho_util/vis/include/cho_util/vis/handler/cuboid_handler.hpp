#pragma once

#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>

#include "cho_util/core/geometry/cuboid.hpp"
#include "cho_util/vis/handler/handler_base.hpp"

namespace cho {
namespace vis {
class CuboidHandler : public HandlerBase<CuboidHandler> {
 public:
  explicit CuboidHandler(const RenderData& rd) {
    const auto& geom = std::get<core::Cuboid<float, 3>>(rd.geometry);
    const auto& bmin = geom.GetMin();
    const auto& bmax = geom.GetMax();

    // Create a plane
    source_ = vtkNew<vtkCubeSource>();
    source_->SetBounds(bmin[0], bmax[0], bmin[1], bmax[1], bmin[2], bmax[2]);

    // Create a mapper and actor
    mapper_ = vtkNew<vtkPolyDataMapper>();  // polydata -> primitives
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
    const auto& geom = std::get<core::Cuboid<float, 3>>(data.geometry);
    const auto& bmin = geom.GetMin();
    const auto& bmax = geom.GetMax();
    source_->SetBounds(bmin.x(), bmax.x(), bmin.y(), bmax.y(), bmin.z(),
                       bmax.z());
    source_->Update();
    // ??
    // source_->Modified();
  }

  vtkSmartPointer<vtkActor> GetActor() { return actor_; }
  vtkSmartPointer<const vtkActor> GetActor() const { return actor_; }

 private:
  vtkSmartPointer<vtkCubeSource> source_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor> actor_;
};

}  // namespace vis
}  // namespace cho
