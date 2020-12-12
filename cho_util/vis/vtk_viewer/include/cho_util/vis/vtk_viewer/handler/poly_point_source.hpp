#pragma once

#include <vtkPolyDataAlgorithm.h>

class vtkPoints;
class vtkUnsignedCharArray;

namespace cho {
namespace vis {

/**
 * @brief Adaptation of vtkPolyPointSource that also enables vertex-wise color
 * assignment.
 */
class ColoredPolyPointSource : public vtkPolyDataAlgorithm {
 public:
  vtkTypeMacro(ColoredPolyPointSource, vtkPolyDataAlgorithm);
  static ColoredPolyPointSource* New();
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void SetNumberOfPoints(vtkIdType numPoints);
  vtkIdType GetNumberOfPoints();

  void SetNumberOfColors(vtkIdType numPoints);
  vtkIdType GetNumberOfColors();

  // Since resize is ambiguous (what should the color be?),
  // just explicitly call SetNumberOf{Points|Colors}.
  // void Resize(vtkIdType numPoints);

  vtkMTimeType GetMTime() override;

  void SetPoints(vtkPoints* points);
  vtkGetObjectMacro(Points, vtkPoints);

  void SetColors(vtkUnsignedCharArray* colors);
  vtkGetObjectMacro(Colors, vtkUnsignedCharArray);

 protected:
  ColoredPolyPointSource();
  ~ColoredPolyPointSource() override;

  int RequestData(vtkInformation*, vtkInformationVector**,
                  vtkInformationVector*) override;

  vtkSmartPointer<vtkUnsignedCharArray> Colors;
  vtkSmartPointer<vtkPoints> Points;

 private:
  ColoredPolyPointSource(const ColoredPolyPointSource&) = delete;
  void operator=(const ColoredPolyPointSource&) = delete;
};

}  // namespace vis
}  // namespace cho
