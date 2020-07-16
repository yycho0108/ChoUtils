#include "cho_util/vis/handlers.hpp"

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
  if (data.data.empty()) {
    return nullptr;
  }

  switch (data.render_type) {
    case RenderData::RenderType::kNone: {
      return nullptr;
      break;
    }
    case RenderData::RenderType::kPoints: {
      return GetCloudActor(&data.data[0], data.data.size() / 3);
      break;
    }
    case RenderData::RenderType::kPlane: {
      return GetPlaneActor(colors, &data.data[0], &data.data[3]);
      break;
    }
    case RenderData::RenderType::kBox: {
      return GetBboxActor(colors, &data.data[0], &data.data[3],
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
    case RenderData::RenderType::kNone: {
      break;
    }
    case RenderData::RenderType::kPoints: {
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
      const int size = (data.data.size() / 3);

      vtkSmartPointer<vtkAlgorithm> alg =
          actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
      vtkSmartPointer<vtkPolyPointSource> src =
          vtkPolyPointSource::SafeDownCast(alg);
      // Set.
      src->Resize(size);
      src->SetNumberOfPoints(size);
      for (int i = 0; i < size; ++i) {
        auto p = &data.data[i * 3];
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
          colorLut->GetColor(0.2 * data.data[i * 3], color);
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
    case RenderData::RenderType::kPlane: {
      vtkSmartPointer<vtkAlgorithm> alg =
          actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
      vtkSmartPointer<vtkPlaneSource> plane = vtkPlaneSource::SafeDownCast(alg);
      plane->SetCenter(data.data[0], data.data[1], data.data[2]);
      plane->SetNormal(data.data[3], data.data[4], data.data[5]);
      plane->Update();
      break;
    }
    case RenderData::RenderType::kBox: {
      vtkSmartPointer<vtkAlgorithm> alg =
          actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
      vtkSmartPointer<vtkCubeSource> bbox = vtkCubeSource::SafeDownCast(alg);
      bbox->SetBounds(data.data[0], data.data[3], data.data[1], data.data[4],
                      data.data[2], data.data[5]);
      bbox->Update();
      break;
    }
  }
}
}  // namespace vis
}  // namespace cho
