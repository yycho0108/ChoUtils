#include "cho_util/vis/vtk_viewer/handler/point_pair_line_filter.hpp"

#include <fmt/printf.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

namespace cho {
namespace vis {

vtkStandardNewMacro(PointPairLineFilter);

PointPairLineFilter::PointPairLineFilter() { this->SetNumberOfInputPorts(1); }
PointPairLineFilter::~PointPairLineFilter() {}

int PointPairLineFilter::RequestData(vtkInformation*,
                                     vtkInformationVector** inputVector,
                                     vtkInformationVector* outputVector) {
  vtkPolyData* input = vtkPolyData::GetData(inputVector[0], 0);
  vtkPolyData* output = vtkPolyData::GetData(outputVector, 0);

  // Points stay the same.
  output->SetPoints(input->GetPoints());

  // Pairwise lines are added.
  const int n = input->GetNumberOfPoints() / 2;
  vtkNew<vtkCellArray> lines;
  for (int i = 0; i < n; ++i) {
    vtkNew<vtkLine> line;
    line->GetPointIds()->SetId(0, 2 * i);
    line->GetPointIds()->SetId(1, 2 * i + 1);
    lines->InsertNextCell(line);
  }

  // Commit lines.
  output->SetLines(lines);
  return 1;
}
}  // namespace vis
}  // namespace cho
