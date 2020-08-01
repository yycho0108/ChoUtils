#pragma once

#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>

#include "cho_util/core/geometry/cylinder.hpp"
#include "cho_util/vis/handler/handler_base.hpp"

namespace cho {
namespace vis {

class CylinderHandler : public HandlerBase<CylinderHandler> {
 public:
  using Ptr = std::shared_ptr<CylinderHandler>;
  explicit CylinderHandler(const RenderData& data) {
    auto colors = vtkSmartPointer<vtkNamedColors>::New();
    source_ = vtkSmartPointer<vtkCylinderSource>::New();
    mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
    actor_ = vtkSmartPointer<vtkActor>::New();

    mapper_->SetInputConnection(source_->GetOutputPort());
    actor_->SetMapper(mapper_);
    actor_->GetProperty()->SetColor(colors->GetColor4d("Tomato").GetData());

    // FIXME(yycho0108): magic params.
    source_->SetResolution(8);
  }

  void Update(const RenderData& data) {
    // NOTE(yycho0108): may throw if invalid data has been passed.
    const auto& geom = std::get<core::Cylinder<float>>(data.geometry);
    const auto& center = geom.GetCenter();
    source_->SetCenter(center.x(), center.y(), center.z());
    source_->SetHeight(geom.GetHeight());
    source_->SetRadius(geom.GetRadius());

    // source_->Update();
    source_->Modified();
  }

 private:
  vtkSmartPointer<vtkCylinderSource> source_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor> actor_;
};
}  // namespace vis
}  // namespace cho
