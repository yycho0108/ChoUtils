#pragma once

#include "cho_util/vis/handler_fwd.hpp"

#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkBoundingBox.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>

#include <vtkPlaneSource.h>
#include <vtkPolyPointSource.h>
#include <vtkSphereSource.h>

#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <string>

#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace vis {

template <typename Derived>
class BaseHandler : public AbstractHandler {
 private:
  BaseHandler() = default;
  BaseHandler(const BaseHandler&) = default;
  friend Derived;

 public:
  virtual void Create(const RenderData& data) {
    return static_cast<Derived*>(this)->CreateImpl(data);
  }
  virtual void Update(const RenderData& data) {
    return static_cast<Derived*>(this)->UpdateImpl(data);
  }
};

class CylinderHandler : public BaseHandler<CylinderHandler> {
 public:
  using Ptr = std::shared_ptr<CylinderHandler>;

  void CreateImpl(const RenderData& data) {
    auto colors = vtkSmartPointer<vtkNamedColors>::New();
    source_ = vtkSmartPointer<vtkCylinderSource>::New();
    mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
    actor_ = vtkSmartPointer<vtkActor>::New();

    mapper_->SetInputConnection(source_->GetOutputPort());
    actor_->SetMapper(mapper_);
    actor_->GetProperty()->SetColor(colors->GetColor4d("Tomato").GetData());

    // NOTE(yycho0108): Params.
    source_->SetResolution(8);
  }

  void UpdateImpl(const RenderData& data) {
    // using GeometryType = std::decay_t<decltype(data.geometry)>;
    // if(!std::is_same<GeometryType, core::Cylinder<float>>()){
    //    throw ...
    //}
    // if(constexpr std::is_same<GeometryType, core::Cylinder<float>>){

    //}

    std::visit(
        [this](const auto& geom) {
          using T = std::decay_t<decltype(geom)>;
          if constexpr (std::is_same_v<T, core::Cylinder<float>>) {
            const auto& center = geom.GetCenter();
            source_->SetCenter(center.x(), center.y(), center.z());
          }
        },
        data.geometry);
    // source_->SetCenter(src.);
    // source_->SetHeight(data.data[3]);
    // source_->SetRadius(data.data[4]);
    // source_->Update();
  }

 private:
  vtkSmartPointer<vtkCylinderSource> source_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor> actor_;
};

/**
 * Handler for actor creation + update.
 */
struct Handler {
  std::function<vtkSmartPointer<vtkActor>(const RenderData&)> create;
  std::function<void(const vtkSmartPointer<vtkActor>&, const RenderData&)>
      update;
};

/**
 * Get a cloud actor for {size x 3} row-major data.
 */
template <typename Scalar>
vtkSmartPointer<vtkActor> GetCloudActor(const Scalar* const data, int size) {
  // points -> polydata -> vertex -> mapper -> actor

#if 0
  // Map array, no-copy
  // vtkNew<vtkFloatArray> arr;
  vtkNew<vtkAOSDataArrayTemplate<Scalar>> arr;
  arr->SetArray(const_cast<Scalar*>(data), size * 3, 1);
  arr->SetNumberOfComponents(3);
  arr->SetNumberOfTuples(size);

  // points
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->SetData(arr);

  // points polydata
  vtkSmartPointer<vtkPolyData> points_poly =
      vtkSmartPointer<vtkPolyData>::New();
  points_poly->SetPoints(points);

  // points -> vertex
  vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter =
      vtkSmartPointer<vtkVertexGlyphFilter>::New();
  vertexFilter->SetInputData(points_poly);
  {
    // Required to set colors.
    vertexFilter->Update();

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
      colorLut->GetColor(data[i * 3], color);
      for (auto& c : color) {
        c *= 255.0f;
      }
      colors->SetTuple(i, color);
    }

    // Set colors
    vertexFilter->GetOutput()->GetCellData()->SetScalars(colors);
  }

  // mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(vertexFilter->GetOutputPort());
#else
  vtkSmartPointer<vtkPolyPointSource> src =
      vtkSmartPointer<vtkPolyPointSource>::New();
  src->Resize(size);
  src->SetNumberOfPoints(size);
  for (int i = 0; i < size; ++i) {
    auto p = &data[i * 3];
    src->SetPoint(i, p[0], p[1], p[2]);
  }
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(src->GetOutputPort());
#endif

  // actor
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(2);
  return actor;
}

template <typename Scalar>
vtkSmartPointer<vtkActor> GetPlaneActor(
    const vtkSmartPointer<vtkNamedColors>& colors, const Scalar* const center,
    const Scalar* const normal) {
  fmt::print("GetPlane {} {} {} {} {} {}\n", center[0], center[1], center[2],
             normal[0], normal[1], normal[2]);

  // Create a plane
  vtkSmartPointer<vtkPlaneSource> planeSource =
      vtkSmartPointer<vtkPlaneSource>::New();
  planeSource->SetCenter(center[0], center[1], center[2]);
  planeSource->SetNormal(normal[0], normal[1], normal[2]);

  // Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(planeSource->GetOutputPort());

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(colors->GetColor3d("Cyan").GetData());

  return actor;
}

template <typename Scalar>
vtkSmartPointer<vtkActor> GetBboxActor(
    vtkSmartPointer<vtkNamedColors>& colors, const Scalar* const min,
    const Scalar* const max, const std::uint8_t* const color = nullptr,
    const RenderData::Representation rep =
        RenderData::Representation::kWireframe) {
  // Create a plane
  vtkNew<vtkCubeSource> bbox;
  bbox->SetBounds(min[0], max[0], min[1], max[1], min[2], max[2]);

  // Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(bbox->GetOutputPort());

  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  if (color) {
    constexpr const float iscale = (1.0 / 255.0f);
    actor->GetProperty()->SetColor(iscale * color[0], iscale * color[1],
                                   iscale * color[2]);
  }
  switch (rep) {
    case RenderData::Representation::kPoints: {
      actor->GetProperty()->SetRepresentationToPoints();
      break;
    }
    case RenderData::Representation::kWireframe: {
      actor->GetProperty()->SetRepresentationToWireframe();
      break;
    }
    case RenderData::Representation::kSurface: {
      actor->GetProperty()->SetRepresentationToSurface();
      break;
    }
    default: { break; }
  }
  actor->GetProperty()->SetLineWidth(2);
  actor->GetProperty()->LightingOff();
  actor->GetProperty()->SetOpacity(0.125);
  return actor;
}

vtkSmartPointer<vtkActor> GetCylinderActor(
    vtkSmartPointer<vtkNamedColors>& colors);

vtkSmartPointer<vtkActor> Render(vtkSmartPointer<vtkNamedColors>& colors,
                                 const RenderData& data);

void Update(const vtkSmartPointer<vtkActor>& actor, const RenderData& data);
}  // namespace vis
}  // namespace cho
