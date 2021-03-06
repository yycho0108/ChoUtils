#pragma once

#include <fmt/format.h>
#include <fmt/printf.h>

#include <unordered_map>
#include <vector>

#include "cho_util/io/pipe_io_fwd.hpp"
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
class SubprocessViewer : public ViewerBase<SubprocessViewer> {
 public:
  inline SubprocessViewer(const bool start) : started_(false) {
    if (start) {
      Start();
    }
  }

  // Status queries
  inline bool IsRunning() const { return started_; }
  inline bool IsServer() const { return IsRunning() && !proc.IsParent(); }
  inline bool IsClient() const { return IsRunning() && proc.IsParent(); }

  // Data handlers
  // std::vector<int> GetAvailableHandlers(const int size) const;
  // int RegisterHandler(Handler&& handler, const int rtype = -1);
  // int RegisterHandler(const Handler& handler, const int rtype = -1);
  // void RegisterDefaultHandlers();

  // App runners
  void Start();
  void StartServer();
  void Render(const RenderData& data);
  void SetCameraPose(const Eigen::Isometry3f& pose);
  void Spin();

 private:
  VtkViewerPtr viewer_;
  Subprocess proc;
  io::FdWriterPtr data_writer_;
  // FdListenerPtr<EventData> event_listener_;

  bool started_;
};

}  // namespace vis
}  // namespace cho
