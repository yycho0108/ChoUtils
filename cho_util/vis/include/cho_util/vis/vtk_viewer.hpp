#pragma once

#include "cho_util/vis/vtk_viewer_fwd.hpp"

#include <memory>

#include "cho_util/vis/event_data.hpp"
#include "cho_util/vis/io.hpp"
#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace vis {

class VtkViewer {
 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  using ListenerPtr = cho::vis::ListenerPtr<RenderData>;
  using WriterPtr = cho::vis::WriterPtr;

 public:
  VtkViewer(const ListenerPtr listener, const WriterPtr writer,
            const bool start, const bool block = true);
  ~VtkViewer();

  // inline std::vector<int> GetAvailableHandlers(const int size) const;
  // inline int RegisterHandler(Handler&& handler, const int rtype);
  // inline int RegisterHandler(const Handler& handler, const int rtype);
  // inline void RegisterDefaultHandlers();

  bool OnData(RenderData&& data);

  void Start(const bool block = true);

  void Step();

  void Spin();
};

}  // namespace vis
}  // namespace cho
