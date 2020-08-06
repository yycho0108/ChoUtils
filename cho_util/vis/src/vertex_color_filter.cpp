#include "cho_util/vis/handler/point_pair_line_filter.hpp"

#include <fmt/printf.h>

namespace cho {
namespace vis {

vtkStandardNewMacro(VertexColorFilter);

VertexColorFilter::VertexColorFilter() { this->SetNumberOfInputPorts(1); }
VertexColorFilter::~VertexColorFilter() {}

int VertexColorFilter::RequestData(vtkInformation*,
                                   vtkInformationVector** inputVector,
                                   vtkInformationVector* outputVector) {
  vtkPolyData* input = vtkPolyData::GetData(inputVector[0], 0);
  vtkPolyData* output = vtkPolyData::GetData(outputVector, 0);

  // points stay the same
  output->SetPoints(input->GetPoints());

  // pairwise lines are added.
  const int n = input->GetNumberOfPoints() / 2;
  fmt::print("#lines = {}\n", n);
  vtkNew<vtkCellArray> lines;
  for (int i = 0; i < n; ++i) {
    vtkNew<vtkLine> line;
    line->GetPointIds()->SetId(0, 2 * i);
    line->GetPointIds()->SetId(1, 2 * i + 1);
    lines->InsertNextCell(line);
  }

  // commit lines.
  output->SetLines(lines);

  return 1;
}
}  // namespace vis
}  // namespace cho
