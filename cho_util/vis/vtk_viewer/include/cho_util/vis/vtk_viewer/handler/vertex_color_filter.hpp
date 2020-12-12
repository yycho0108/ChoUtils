#pragma once

#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataMapper.h>

#include "cho_util/core/geometry/line.hpp"
#include "cho_util/vis/vtk_viewer/handler/handler_base.hpp"

namespace cho {
namespace vis {

class VertexColorFilter : public vtkPolyDataAlgorithm {
 public:
  vtkTypeMacro(VertexColorFilter, vtkPolyDataAlgorithm);
  static VertexColorFilter* New();

 protected:
  VertexColorFilter();
  ~VertexColorFilter();
  int RequestData(vtkInformation*, vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector) override;

 private:
  VertexColorFilter(const VertexColorFilter&) = delete;
  void operator=(const VertexColorFilter&) = delete;
};

}  // namespace vis
}  // namespace cho
