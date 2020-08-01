#pragma once

#include "cho_util/vis/vtk_viewer_fwd.hpp"

#include <memory>

#include "cho_util/vis/event_data.hpp"
#include "cho_util/vis/io.hpp"
#include "cho_util/vis/render_data.hpp"
#include "cho_util/vis/viewer_base.hpp"

namespace cho {
namespace vis {

class VtkViewer : ViewerBase<VtkViewer> {
 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  using ListenerPtr = cho::vis::ListenerPtr<RenderData>;
  using WriterPtr = cho::vis::WriterPtr;

 public:
  VtkViewer(const ListenerPtr listener, const WriterPtr writer,
            const bool start, const bool block = true);
  ~VtkViewer();

  void Start(const bool block = true);
  void Step();
  void Spin();
  bool Render(RenderData&& data);
};

}  // namespace vis
}  // namespace cho
