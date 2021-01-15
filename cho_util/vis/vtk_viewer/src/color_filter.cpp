
#include "cho_util/vis/vtk_viewer/handler/color_filter.hpp"

#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>

#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace vis {

vtkStandardNewMacro(ColorFilter);
vtkCxxSetObjectMacro(ColorFilter, Colors, vtkUnsignedCharArray);

ColorFilter::ColorFilter() { this->SetNumberOfInputPorts(1); }
ColorFilter::~ColorFilter() {}

void ColorFilter::SetNumberOfColors(vtkIdType numPoints) {
  if (!this->Colors) {
    Colors = vtkNew<vtkUnsignedCharArray>();
    Colors->SetNumberOfComponents(3);
    Colors->SetNumberOfTuples(numPoints);
  }

  if (this->Colors->GetNumberOfTuples() != numPoints) {
    this->Colors->SetNumberOfTuples(numPoints);
    this->Modified();
  }
}

void ColorFilter::UpdateWithRenderData(const RenderData& rd) {
  const int num_colors = rd.color.size() / 3;

  // Allocate output.
  SetNumberOfColors(num_colors);

  // Populate output.
  double color[3];
  for (int i = 0; i < num_colors; ++i) {
    std::copy(&rd.color[i * 3], &rd.color[(i + 1) * 3], color);
    Colors->SetTuple(i, color);
  }
}

vtkIdType ColorFilter::GetNumberOfColors() {
  if (this->Colors) {
    return this->Colors->GetNumberOfTuples();
  }
  return 0;
}

int ColorFilter::RequestData(vtkInformation*,
                             vtkInformationVector** inputVector,
                             vtkInformationVector* outputVector) {
  vtkPolyData* input = vtkPolyData::GetData(inputVector[0], 0);
  vtkPolyData* output = vtkPolyData::GetData(outputVector, 0);

  // Copy input data.
  output->ShallowCopy(input);

  // Fetch the domain over which to apply the colors ...
  const int num_colors = this->GetNumberOfColors();
  const int num_points = input->GetNumberOfPoints();
  const int num_cells = input->GetNumberOfCells();

  // Set Colors.
  if (num_colors == num_cells) {
    output->GetCellData()->SetScalars(this->Colors);
  } else if (num_colors == num_points) {
    output->GetPointData()->SetScalars(Colors);
  }
  return 1;
}

}  // namespace vis
}  // namespace cho
