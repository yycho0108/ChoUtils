#pragma once

#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataMapper.h>

#include "cho_util/core/geometry/line.hpp"
#include "cho_util/vis/handler/handler_base.hpp"

namespace cho {
namespace vis {
    
class PointPairLineFilter : public vtkPolyDataAlgorithm {
 public:
  vtkTypeMacro(PointPairLineFilter, vtkPolyDataAlgorithm);
  static PointPairLineFilter* New();

 protected:
  PointPairLineFilter();
  ~PointPairLineFilter();

  // int FillInputPortInformation(int port, vtkInformation* info) override {
  //   if (port == 0) {
  //     info->Set(vtkDataObject::DATA_TYPE_NAME, "");
  //     return 1;
  //   }
  //   return 0;
  // }
  int RequestData(vtkInformation*, vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector) override;

 private:
  PointPairLineFilter(const PointPairLineFilter&) = delete;
  void operator=(const PointPairLineFilter&) = delete;
};

}  // namespace vis
}  // namespace cho
