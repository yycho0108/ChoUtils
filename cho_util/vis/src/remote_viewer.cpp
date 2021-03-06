#include "cho_util/vis/remote_viewer.hpp"

#include <future>

#include "cho_util/io/queue_io.hpp"
#include "cho_util/vis/remote_viewer_client.hpp"
#include "cho_util/vis/remote_viewer_server.hpp"
#include "cho_util/vis/render_data.hpp"
#include "cho_util/vis/vtk_viewer/vtk_viewer.hpp"

namespace cho {
namespace vis {

// App runners
void RemoteViewerServer::Start(const std::string& address) {
  // Start listening to server stuff asynchronously.
  server_ = std::make_shared<RenderServer<RenderData>>(address);
  std::thread server_thread([this]() { server_->Start(); });

  // FIXME(yycho0108): Make event writer do something
  auto event_queue_ = std::make_shared<thread_safe_queue<RenderData>>();
  io::QueueWriterPtr<RenderData> event_writer =
      std::make_shared<io::QueueWriter<RenderData>>(event_queue_);

  // Start immediately + block
  viewer_ = std::make_shared<VtkViewer>(
      std::dynamic_pointer_cast<io::Listener<RenderData>>(server_),
      event_writer, true, true);

  // Cleanup server thread.
  if (server_thread.joinable()) {
    server_thread.join();
  }
}

RemoteViewerClient::RemoteViewerClient(const std::string& channel) {
  client_ = std::make_shared<RenderClient>(channel);
}
void RemoteViewerClient::Render(const RenderData& data) {
  client_->Render(data);
}
void RemoteViewerClient::Spin() { std::promise<void>().get_future().wait(); }
}  // namespace vis
}  // namespace cho
