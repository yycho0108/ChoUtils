#pragma once

#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataMapper.h>

#include "cho_util/vis/render_data_fwd.hpp"

namespace cho {
namespace vis {

class ColorFilter : public vtkPolyDataAlgorithm {
 public:
  vtkTypeMacro(ColorFilter, vtkPolyDataAlgorithm);
  static ColorFilter* New();

  void SetNumberOfColors(vtkIdType numPoints);
  vtkIdType GetNumberOfColors();
  void SetColors(vtkUnsignedCharArray* colors);
  vtkGetObjectMacro(Colors, vtkUnsignedCharArray);

  void UpdateWithRenderData(const RenderData& rd);

 protected:
  ColorFilter();
  ~ColorFilter();
  int RequestData(vtkInformation*, vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector) override;

 private:
  ColorFilter(const ColorFilter&) = delete;
  void operator=(const ColorFilter&) = delete;
  vtkSmartPointer<vtkUnsignedCharArray> Colors;
};

}  // namespace vis
}  // namespace cho
