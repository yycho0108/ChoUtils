#pragma once

#include <fmt/format.h>
#include <fmt/printf.h>

#include <unordered_map>
#include <vector>

#include "cho_util/core/thread_safe_queue.hpp"

#include "cho_util/vis/handler_fwd.hpp"
#include "cho_util/vis/queue_io_fwd.hpp"
#include "cho_util/vis/render_data_fwd.hpp"
#include "cho_util/vis/subprocess.hpp"
#include "cho_util/vis/vtk_viewer_fwd.hpp"

namespace cho {
namespace vis {

/**
 *
 *
 */
class DirectViewer {
 public:
  DirectViewer(const bool start = false);

  // Status queries
  inline bool IsRunning() const { return started_; }
  inline bool IsServer() const { return true; }
  inline bool IsClient() const { return true; }

  // Data handlers
  // std::vector<int> GetAvailableHandlers(const int size) const;
  // int RegisterHandler(Handler&& handler, const int rtype = -1);
  // int RegisterHandler(const Handler& handler, const int rtype = -1);
  // void RegisterDefaultHandlers();

  // App runners
  void Start();
  void StartServer();
  void Render(const RenderData& data);
  void Spin();

 private:
  std::shared_ptr<thread_safe_queue<RenderData>> data_queue_;
  std::shared_ptr<thread_safe_queue<RenderData>> event_queue_;

  VtkViewerPtr viewer_;
  QueueWriterPtr<RenderData> data_writer_;
  // QueueListenerPtr<EventData> event_listener_;

  bool started_;
};

}  // namespace vis
}  // namespace cho
