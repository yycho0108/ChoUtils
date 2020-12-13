#pragma once

#include <memory>

#include "cho_util/io/io.hpp"
#include "cho_util/vis/event_data.hpp"
#include "cho_util/vis/render_data.hpp"
#include "cho_util/vis/viewer_base.hpp"
#include "cho_util/vis/vtk_viewer/vtk_viewer_fwd.hpp"

namespace cho {
namespace vis {

class VtkViewer : ViewerBase<VtkViewer> {
 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  using ListenerPtr = cho::io::ListenerPtr<RenderData>;
  using WriterPtr = cho::io::WriterPtr;

 public:
  VtkViewer(const ListenerPtr listener, const WriterPtr writer,
            const bool start, const bool block = true);
  ~VtkViewer();

  void Start(const bool block = true);
  void Step();
  void Spin();
  void SetCameraPose(const Eigen::Isometry3f& pose);
  bool Render(RenderData&& data);
};

}  // namespace vis
}  // namespace cho
