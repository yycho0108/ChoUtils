#pragma once

#include <vtkCellData.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyPointSource.h>

#include <fmt/printf.h>

#include "cho_util/core/geometry/point_cloud.hpp"
#include "cho_util/vis/handler/handler_base.hpp"

namespace cho {
namespace vis {

class CloudHandler : public HandlerBase<CloudHandler> {
 public:
  using Ptr = std::shared_ptr<CloudHandler>;

  explicit CloudHandler(const RenderData& data) {
    const auto& geom = std::get<core::PointCloud<float, 3>>(data.geometry);
    fmt::print("{},{}\n", geom.GetData().rows(), geom.GetData().cols());
    fmt::print("#={}\n", geom.GetNumPoints());

    source_ = vtkNew<vtkPolyPointSource>();
    source_->Resize(geom.GetNumPoints());
    source_->SetNumberOfPoints(geom.GetNumPoints());
    for (int i = 0; i < geom.GetNumPoints(); ++i) {
      const auto& point = geom.GetPoint(i);
      source_->SetPoint(i, point.x(), point.y(), point.z());
    }
    mapper_ = vtkNew<vtkPolyDataMapper>();
    mapper_->SetInputConnection(source_->GetOutputPort());

    // actor
    actor_ = vtkNew<vtkActor>();
    actor_->SetMapper(mapper_);
    // TODO(yycho0108): magic constant
    actor_->GetProperty()->SetPointSize(2);
  }

  void Update(const RenderData& data) {
    const auto& geom = std::get<core::PointCloud<float, 3>>(data.geometry);
    const int size = geom.GetNumPoints();

    // Set.
    source_->Resize(size);
    source_->SetNumberOfPoints(size);
    for (int i = 0; i < size; ++i) {
      const auto& p = geom.GetPoint(i);
      source_->SetPoint(i, p[0], p[1], p[2]);
    }
    source_->GetPoints()->Modified();

    if (1) {
      // Colormap
      vtkNew<vtkLookupTable> colorLut;
      colorLut->SetTableRange(0, 1);
      colorLut->Build();

      // Compute colors via colormap
      vtkNew<vtkUnsignedCharArray> colors;
      colors->SetNumberOfComponents(3);
      colors->SetNumberOfTuples(size);
      double color[3];
      for (int i = 0; i < size; ++i) {
        const auto& p = geom.GetPoint(i);
        colorLut->GetColor(0.2 * p.z(), color);
        for (auto& c : color) {
          c *= 255.0f;
        }
        colors->SetTuple(i, color);
      }
      // Set colors
      source_->GetOutput()->GetCellData()->SetScalars(colors);
    }

    // NOTE(yycho0108): Update is required here
    // source_->Modified();
  }

  vtkSmartPointer<vtkActor> GetActor() { return actor_; }
  vtkSmartPointer<const vtkActor> GetActor() const { return actor_; }

 private:
  vtkSmartPointer<vtkPolyPointSource> source_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor> actor_;
};

}  // namespace vis
}  // namespace cho
