#pragma once

#include <fmt/format.h>
#include <fmt/printf.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include "cho_util/vis/render_data_fwd.hpp"
#include "cho_util/vis/subprocess.hpp"
#include "cho_util/vis/vtk_viewer_fwd.hpp"

namespace cho {
namespace vis {

template <typename DataType>
class RenderServer;

template <typename DataType>
using RenderServerPtr = std::shared_ptr<RenderServer<DataType>>;

class RenderClient;
using RenderClientPtr = std::shared_ptr<RenderClient>;

/**
 * GrpcViewer?
 */
class RemoteViewerServer {
 public:
  inline explicit RemoteViewerServer(
      const bool start, const std::string& address = "0.0.0.0:50051")
      : started_(false) {
    if (start) {
      Start(address);
    }
  }
  // Status queries
  inline bool IsRunning() const { return started_; }

  // App runners
  void Start(const std::string& address = "0.0.0.0:50051");

 private:
  VtkViewerPtr viewer_;
  RenderServerPtr<RenderData> server_;
  bool started_;
};

class RemoteViewerClient {
 public:
  explicit RemoteViewerClient(const std::string& channel = "localhost:50051");
  void Render(const RenderData& data);
  void Spin();

 private:
  RenderClientPtr client_;
};

}  // namespace vis
}  // namespace cho
