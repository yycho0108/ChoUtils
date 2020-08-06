#pragma once

#include <vtkCellData.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>

#include <fmt/printf.h>

#include "cho_util/core/geometry/point_cloud.hpp"
#include "cho_util/vis/handler/handler_base.hpp"
#include "cho_util/vis/handler/poly_point_source.hpp"

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
  }
  void SetColors(const RenderData& data) {
    const auto& geom = std::get<core::PointCloud<float, 3>>(data.geometry);
    const int size = geom.GetNumPoints();

    if (data.color.empty()) {
      fmt::print("EMPTY\n");
      // source_->SetUseVertexColors(false);
      source_->SetNumberOfColors(0);
    } else if (data.color.size() == 3) {
      fmt::print("UNI\n");
      // source_->SetUseVertexColors(false);
      source_->SetNumberOfColors(0);
      actor_->GetProperty()->SetColor(data.color[0], data.color[1],
                                      data.color[2]);
    } else if (data.color.size() == 3 * size) {
      // source_->SetUseVertexColors(true);
      fmt::print("EACH\n");
      source_->SetNumberOfColors(size);
      auto* colors = source_->GetColors();
      double color[3];
      for (int i = 0; i < size; ++i) {
        fmt::print("{} {} {}\n", data.color[i * 3], data.color[i * 3 + 1],
                   data.color[i * 3 + 2]);

        std::copy(&data.color[i * 3], &data.color[(i + 1) * 3], color);
        colors->SetTuple(i, color);
      }
      // source_->SetColors(colors);
    }
  }

 public:
  using Ptr = std::shared_ptr<CloudHandler>;

  explicit CloudHandler(const RenderData& data) {
    const auto& geom = std::get<core::PointCloud<float, 3>>(data.geometry);
    source_ = vtkNew<ColoredPolyPointSource>();
    fmt::print("{},{}\n", geom.GetData().rows(), geom.GetData().cols());
    fmt::print("#={}\n", geom.GetNumPoints());

    mapper_ = vtkNew<vtkPolyDataMapper>();
    mapper_->SetInputConnection(source_->GetOutputPort());

    // actor
    actor_ = vtkNew<vtkActor>();
    actor_->SetMapper(mapper_);
    // TODO(yycho0108): magic constant
    actor_->GetProperty()->SetPointSize(2);

    SetPoints(data);
    SetColors(data);
    source_->Modified();
  }

  void Update(const RenderData& data) {
    const auto& geom = std::get<core::PointCloud<float, 3>>(data.geometry);
    const int size = geom.GetNumPoints();

    // Set.
    SetPoints(data);
    SetColors(data);
    source_->Modified();

    // NOTE(yycho0108): Update is required here
    // source_->Modified();
  }

  vtkSmartPointer<vtkActor> GetActor() { return actor_; }
  vtkSmartPointer<const vtkActor> GetActor() const { return actor_; }

 private:
  vtkSmartPointer<ColoredPolyPointSource> source_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor> actor_;
};

}  // namespace vis
}  // namespace cho
