#pragma once

#include <fmt/format.h>
#include <fmt/printf.h>

#include <unordered_map>
#include <vector>

#include "cho_util/core/thread_safe_queue.hpp"

#include "cho_util/io/queue_io_fwd.hpp"
#include "cho_util/vis/render_data_fwd.hpp"
#include "cho_util/vis/subprocess.hpp"
#include "cho_util/vis/viewer_base.hpp"
#include "cho_util/vis/vtk_viewer/vtk_viewer_fwd.hpp"

namespace cho {
namespace vis {

/**
 *
 *
 */
class DirectViewer : public ViewerBase<DirectViewer> {
 public:
  DirectViewer(const bool start = false);

  // Status queries
  inline bool IsRunning() const { return started_; }
  inline bool IsServer() const { return true; }
  inline bool IsClient() const { return true; }

  // App runners
  void Start();
  void Step();
  void Spin();
  void Render(const RenderData& data);

  void StartServer();

 private:
  std::shared_ptr<thread_safe_queue<RenderData>> data_queue_;
  std::shared_ptr<thread_safe_queue<RenderData>> event_queue_;

  VtkViewerPtr viewer_;
  io::QueueWriterPtr<RenderData> data_writer_;
  // QueueListenerPtr<EventData> event_listener_;

  bool started_;
};

}  // namespace vis
}  // namespace cho
