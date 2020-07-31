#include "cho_util/vis/handlers.hpp"

#include <memory>
#include <string>

#include <fmt/format.h>
#include <fmt/printf.h>

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
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyPointSource.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>

#include "cho_util/core/geometry/plane.hpp"
#include "cho_util/core/geometry/point_cloud.hpp"
#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace vis {

vtkSmartPointer<vtkActor> GetCylinderActor(
    vtkSmartPointer<vtkNamedColors>& colors) {
  // This creates a polygonal cylinder model with eight circumferential facets
  // (i.e, in practice an octagonal prism).
  vtkSmartPointer<vtkCylinderSource> cylinder =
      vtkSmartPointer<vtkCylinderSource>::New();
  cylinder->SetResolution(8);

  // Filter -> polydata

  // The mapper is responsible for pushing the geometry into the graphics
  // library. It may also do color mapping, if scalars or other attributes are
  // defined.
  vtkSmartPointer<vtkPolyDataMapper> cylinderMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

  // The actor is a grouping mechanism: besides the geometry (mapper), it
  // also has a property, transformation matrix, and/or texture map.
  // Here we set its color and rotate it around the X and Y axes.
  vtkSmartPointer<vtkActor> cylinderActor = vtkSmartPointer<vtkActor>::New();
  cylinderActor->SetMapper(cylinderMapper);
  cylinderActor->GetProperty()->SetColor(
      colors->GetColor4d("Tomato").GetData());
  return cylinderActor;
}

vtkSmartPointer<vtkActor> Render(vtkSmartPointer<vtkNamedColors>& colors,
                                 const RenderData& data) {
  if (!data.geometry) {
    return nullptr;
  }

  switch (data.render_type) {
    case RenderType::kNone: {
      return nullptr;
      break;
    }
    case RenderType::kPoints: {
      return GetCloudActor(data.geometry->GetPtr(),
                           data.geometry->GetSize() / 3);
      break;
    }
    case RenderType::kPlane: {
      return GetPlaneActor(colors, data.geometry->GetPtr(),
                           data.geometry->GetPtr() + 3);
      break;
    }
    case RenderType::kBox: {
      fmt::print("BOX\n");
      // return GetBboxActor(colors, data.geometry->GetPtr(),
      auto geo = std::dynamic_pointer_cast<const core::Cuboid<float, 3> >(
          data.geometry);
      const Eigen::Vector3f bmin = geo->GetMin();
      const Eigen::Vector3f bmax = geo->GetMax();
      return GetBboxActor(colors, &bmin(0), &bmax(0),
                          data.color.empty() ? nullptr : data.color.data(),
                          data.representation);
      break;
    }
    default: {
      return nullptr;
      break;
    }
  }
  return nullptr;
}

void Update(const vtkSmartPointer<vtkActor>& actor, const RenderData& data) {
  switch (data.render_type) {
    case RenderType::kNone: {
      break;
    }
    case RenderType::kPoints: {
#if 0
      // Update!
      vtkSmartPointer<vtkAlgorithm> alg =
          actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
      vtkSmartPointer<vtkVertexGlyphFilter> vertex =
          vtkVertexGlyphFilter::SafeDownCast(alg);
      vtkSmartPointer<vtkPolyData> poly =
          vtkPolyData::SafeDownCast(vertex->GetInputDataObject(0, 0));
      // vertex->GetOutput()->GetCellData()->SetScalars()

      // Points
      vtkNew<vtkFloatArray> arr;
      arr->SetArray(const_cast<float*>(data.data.data()), data.data.size(), 1);
      arr->SetNumberOfComponents(3);
      arr->SetNumberOfTuples(data.data.size() / 3);

      // points
      vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
      points->SetData(arr);
      poly->SetPoints(points);

      vertex->SetInputData(poly);
      vertex->Update();
#else
      auto geo = std::dynamic_pointer_cast<const core::PointCloud<float, 3> >(
          data.geometry);
      const int size = geo->GetNumPoints();

      vtkSmartPointer<vtkAlgorithm> alg =
          actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
      vtkSmartPointer<vtkPolyPointSource> src =
          vtkPolyPointSource::SafeDownCast(alg);
      // Set.
      src->Resize(size);
      src->SetNumberOfPoints(size);
      for (int i = 0; i < size; ++i) {
        const auto& p = geo->GetData().col(i);
        ;
        src->SetPoint(i, p[0], p[1], p[2]);
      }
      src->GetPoints()->Modified();

      // Update.
      src->Update();
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
          const auto& p = geo->GetData().col(i);
          colorLut->GetColor(0.2 * p.z(), color);
          for (auto& c : color) {
            c *= 255.0f;
          }
          colors->SetTuple(i, color);
        }
        // Set colors
        src->GetOutput()->GetCellData()->SetScalars(colors);
      }
#endif
      break;
    }
    case RenderType::kPlane: {
      auto geo = std::dynamic_pointer_cast<const core::Plane<float, 3> >(
          data.geometry);
      const auto& center = geo->GetCenter();
      const auto& normal = geo->GetNormal();

      vtkSmartPointer<vtkAlgorithm> alg =
          actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
      vtkSmartPointer<vtkPlaneSource> plane = vtkPlaneSource::SafeDownCast(alg);
      plane->SetCenter(center.x(), center.y(), center.z());
      plane->SetNormal(normal.x(), normal.y(), normal.z());
      plane->Update();
      break;
    }
    case RenderType::kCuboid: {
      auto geo = std::dynamic_pointer_cast<const core::Cuboid<float, 3> >(
          data.geometry);

      const auto& bmin = geo->GetMin();
      const auto& bmax = geo->GetMax();
      fmt::print("min {} {} {}\n", bmin.x(), bmin.y(), bmin.z());
      fmt::print("max {} {} {}\n", bmax.x(), bmax.y(), bmax.z());

      vtkSmartPointer<vtkAlgorithm> alg =
          actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
      vtkSmartPointer<vtkCubeSource> bbox = vtkCubeSource::SafeDownCast(alg);
      bbox->SetBounds(bmin.x(), bmax.x(), bmin.y(), bmax.y(), bmin.z(),
                      bmax.z());
      bbox->Update();
      break;
    }
  }
}
}  // namespace vis
}  // namespace cho
