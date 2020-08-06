#include "cho_util/vis/handler/poly_point_source.hpp"

#include <fmt/printf.h>

#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkUnsignedCharArray.h>

namespace cho {
namespace vis {

vtkStandardNewMacro(ColoredPolyPointSource);
vtkCxxSetObjectMacro(ColoredPolyPointSource, Points, vtkPoints);
vtkCxxSetObjectMacro(ColoredPolyPointSource, Colors, vtkUnsignedCharArray);

ColoredPolyPointSource::ColoredPolyPointSource()
    : Points(nullptr), Colors(nullptr) {
  // Source.
  this->SetNumberOfInputPorts(0);
}

ColoredPolyPointSource::~ColoredPolyPointSource() {
  // Points/Colors are shared pointers: will handle themselves just fine.
}

void ColoredPolyPointSource::SetNumberOfPoints(vtkIdType numPoints) {
  if (!this->Points) {
    Points = vtkNew<vtkPoints>();
  }

  if (this->Points->GetNumberOfPoints() != numPoints) {
    this->Points->SetNumberOfPoints(numPoints);
    this->Modified();
  }
}
vtkIdType ColoredPolyPointSource::GetNumberOfPoints() {
  if (this->Points) {
    return this->Points->GetNumberOfPoints();
  }
  return 0;
}
void ColoredPolyPointSource::SetNumberOfColors(vtkIdType numPoints) {
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

vtkIdType ColoredPolyPointSource::GetNumberOfColors() {
  if (this->Colors) {
    return this->Colors->GetNumberOfTuples();
  }
  return 0;
}
vtkMTimeType ColoredPolyPointSource::GetMTime() {
  vtkMTimeType mTime = this->Superclass::GetMTime();

  if (this->Points != nullptr) {
    const vtkMTimeType time = this->Points->GetMTime();
    mTime = (time > mTime ? time : mTime);
  }

  if (this->Colors != nullptr) {
    const vtkMTimeType time = this->Colors->GetMTime();
    mTime = (time > mTime ? time : mTime);
  }

  return mTime;
}

// void ColoredPolyPointSource::Resize(vtkIdType numPoints) {
//  // vtkPolyPoint::Resize(...);
//  SetNumberOfPoints(numPoints);
//  // Consider if this will produce consistent behavior.
//  SetNumberOfColors(numPoints);
//}

int ColoredPolyPointSource::RequestData(
    vtkInformation* vtkNotUsed(request),
    vtkInformationVector** vtkNotUsed(inputVector),
    vtkInformationVector* outputVector) {
  // get the info object
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the output
  vtkPolyData* output =
      vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdType numPoints = this->GetNumberOfPoints();
  vtkSmartPointer<vtkIdList> pointIds = vtkSmartPointer<vtkIdList>::New();
  pointIds->SetNumberOfIds(numPoints);
  for (vtkIdType i = 0; i < numPoints; ++i) {
    pointIds->SetId(i, i);
  }

  vtkSmartPointer<vtkCellArray> PolyPoint =
      vtkSmartPointer<vtkCellArray>::New();
  PolyPoint->InsertNextCell(pointIds);

  output->SetPoints(this->Points);
  output->SetVerts(PolyPoint);

  // On top of the original code, also add color.
  if (this->GetNumberOfColors() == numPoints) {
    output->GetPointData()->SetScalars(this->Colors);
    // output->GetCellData()->SetScalars(this->Colors);
  }

  return 1;
}

void ColoredPolyPointSource::PrintSelf(ostream& os, vtkIndent indent) {
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Points: " << this->Points << "\n";
  os << indent << "Colors: " << this->Colors << "\n";
}

}  // namespace vis
}  // namespace cho
