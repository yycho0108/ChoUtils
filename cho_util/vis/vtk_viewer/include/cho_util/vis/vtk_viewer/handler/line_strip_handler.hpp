#pragma once

#include <vtkPolyDataMapper.h>
#include <vtkPolyLineSource.h>

#include "cho_util/core/geometry/line.hpp"
#include "cho_util/vis/vtk_viewer/handler/handler_base.hpp"
#include "cho_util/vis/vtk_viewer/handler/point_pair_line_filter.hpp"

namespace cho {
namespace vis {

#if 0
class LineStripHandler : public HandlerBase<LineStripHandler> {
 public:
  explicit LineStripHandler(const RenderData& rd) {
    const auto& geom = std::get<core::Linestrip<float, 3>>(rd.geometry);

    // Point Source
    source_ = vtkNew<vtkPolyLineSource>();
    source_->GetPointIds()->SetNumberOfIds(geom.GetNumPoints());
    for (int i = 0; i < geom.GetNumPoints(); ++i) {
      source_->GetPointIds()->SetId(i, i);
    }
    points_->SetNumberOfPoints(geom.GetNumPoints());
    for (int i = 0; i < geom.GetNumPoints(); ++i) {
      const auto& point = geom.GetPoint(i);
      points_->SetPoint(i, point.x(), point.y(), point.z());
    }

    // Mapper
    mapper_ = vtkNew<vtkPolyDataMapper>();
    mapper_->SetInputConnection(source_->GetOutputPort());

    // Actor
    actor_ = vtkNew<vtkActor>();
    actor_->SetMapper(mapper_);
    if (!rd.color.empty()) {
      constexpr const float iscale = (1.0 / 255.0f);
      actor_->GetProperty()->SetColor(
          iscale * rd.color[0], iscale * rd.color[1], iscale * rd.color[2]);
    }

    // switch (rd.representation) {
    //  case RenderData::Representation::kPoints: {
    //    actor_->GetProperty()->SetRepresentationToPoints();
    //    break;
    //  }
    //  case RenderData::Representation::kWireframe: {
    //    actor_->GetProperty()->SetRepresentationToWireframe();
    //    break;
    //  }
    //  case RenderData::Representation::kSurface: {
    //    actor_->GetProperty()->SetRepresentationToSurface();
    //    break;
    //  }
    //  default: { break; }
    //}
    // actor_->GetProperty()->SetLineWidth(2);
    // actor_->GetProperty()->LightingOff();
    // actor_->GetProperty()->SetOpacity(0.125);
  }
  void Update(const RenderData& data) {
    const auto& geom = std::get<core::Lines<float, 3>>(data.geometry);
    const int num_lines = geom.GetNumLines();
    const int num_points = num_lines * 2;

    // Set.
    points_->Resize(num_points);
    points_->SetNumberOfPoints(num_points);
    for (int i = 0; i < num_points; ++i) {
      const auto& p = geom.GetPoint(i);
      points_->SetPoint(i, p[0], p[1], p[2]);
    }
    points_->GetPoints()->Modified();
    points_->Modified();
  }

  vtkSmartPointer<vtkActor> GetActor() { return actor_; }
  vtkSmartPointer<const vtkActor> GetActor() const { return actor_; }

 private:
  vtkSmartPointer<vtkPolyLineSource> source_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor> actor_;
};
#endif

}  // namespace vis
}  // namespace cho
