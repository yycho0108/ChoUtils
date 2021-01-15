#pragma once

#include <vtkCellData.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyPointSource.h>

#include <fmt/printf.h>

#include "cho_util/core/geometry/point_cloud.hpp"
#include "cho_util/vis/vtk_viewer/handler/color_filter.hpp"
#include "cho_util/vis/vtk_viewer/handler/handler_base.hpp"

namespace cho {
namespace vis {

class CloudHandler : public HandlerBase<CloudHandler> {
 private:
  void SetPoints(const RenderData& data) {
    const auto& geom = std::get<core::PointCloud<float, 3>>(data.geometry);
    source_->SetNumberOfPoints(geom.GetNumPoints());
    auto* points = source_->GetPoints();
    for (int i = 0; i < geom.GetNumPoints(); ++i) {
      const auto& point = geom.GetPoint(i);
      // TODO(yycho0108): Consider forwarding call within source class.
      points->SetPoint(i, point.x(), point.y(), point.z());
    }
    points->Modified();
  }

 public:
  using Ptr = std::shared_ptr<CloudHandler>;

  explicit CloudHandler(const RenderData& data) {
    const auto& geom = std::get<core::PointCloud<float, 3>>(data.geometry);
    source_ = vtkNew<vtkPolyPointSource>();

    color_filter_ = vtkNew<ColorFilter>();
    color_filter_->SetInputConnection(source_->GetOutputPort());

    mapper_ = vtkNew<vtkPolyDataMapper>();
    mapper_->SetInputConnection(color_filter_->GetOutputPort());

    // actor
    actor_ = vtkNew<vtkActor>();
    actor_->SetMapper(mapper_);
    // TODO(yycho0108): magic constant
    actor_->GetProperty()->SetPointSize(2);

    SetPoints(data);
    color_filter_->UpdateWithRenderData(data);
    // SetColors(data);
    source_->Modified();
  }

  void Update(const RenderData& data) {
    const auto& geom = std::get<core::PointCloud<float, 3>>(data.geometry);
    const int size = geom.GetNumPoints();

    // Set.
    SetPoints(data);
    color_filter_->UpdateWithRenderData(data);
    // SetColors(data);
    mapper_->Update();
    // source_->Update();
    // source_->Modified();
    // NOTE(yycho0108): Update is required here
    // source_->Modified();
  }

  vtkSmartPointer<vtkActor> GetActor() { return actor_; }
  vtkSmartPointer<const vtkActor> GetActor() const { return actor_; }

 private:
  vtkSmartPointer<vtkPolyPointSource> source_;
  vtkSmartPointer<ColorFilter> color_filter_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor> actor_;
};

}  // namespace vis
}  // namespace cho
