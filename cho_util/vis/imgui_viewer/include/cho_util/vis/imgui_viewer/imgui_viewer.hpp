#pragma once

#include <memory>

#include "cho_util/io/io.hpp"
#include "cho_util/vis/event_data.hpp"
#include "cho_util/vis/render_data.hpp"
#include "cho_util/vis/viewer_base.hpp"

namespace cho {
namespace vis {

class ImguiViewer : ViewerBase<ImguiViewer> {
 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  using ListenerPtr = cho::io::ListenerPtr<RenderData>;
  using WriterPtr = cho::io::WriterPtr;

 public:
  ImguiViewer(const ListenerPtr listener, const WriterPtr writer,
              const bool start, const bool block = true);
  ~ImguiViewer();

  void Start(const bool block = true);
  void Step();
  void Spin();
  bool Render(RenderData&& data);
};

}  // namespace vis
}  // namespace cho
