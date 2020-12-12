#pragma once

#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyPointSource.h>

#include "cho_util/core/geometry/line.hpp"
#include "cho_util/vis/vtk_viewer/handler/handler_base.hpp"
#include "cho_util/vis/vtk_viewer/handler/point_pair_line_filter.hpp"

namespace cho {
namespace vis {

class LineHandler : public HandlerBase<LineHandler> {
 public:
  explicit LineHandler(const RenderData& rd) {
    const auto& geom = std::get<core::Lines<float, 3>>(rd.geometry);

    // Point Source
    points_ = vtkNew<vtkPolyPointSource>();
    points_->Resize(geom.GetNumPoints());
    points_->SetNumberOfPoints(geom.GetNumPoints());
    for (int i = 0; i < geom.GetNumPoints(); ++i) {
      const auto& point = geom.GetPoint(i);
      points_->SetPoint(i, point.x(), point.y(), point.z());
    }

    // Filter to pairwise line segments
    filter_ = vtkNew<PointPairLineFilter>();
    filter_->SetInputConnection(points_->GetOutputPort());

    // Mapper
    mapper_ = vtkNew<vtkPolyDataMapper>();
    mapper_->SetInputConnection(filter_->GetOutputPort());

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
  vtkSmartPointer<vtkPolyPointSource> points_;
  vtkSmartPointer<PointPairLineFilter> filter_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor> actor_;
};

}  // namespace vis
}  // namespace cho
