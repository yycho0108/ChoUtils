#include "cho_util/vis/direct_viewer.hpp"

#include "cho_util/io/queue_io.hpp"
#include "cho_util/vis/render_data.hpp"
#include "cho_util/vis/vtk_viewer/vtk_viewer.hpp"

namespace cho {
namespace vis {

DirectViewer::DirectViewer(const bool start)
    : started_(false),
      data_queue_(std::make_shared<thread_safe_queue<RenderData>>()),
      event_queue_(std::make_shared<thread_safe_queue<RenderData>>()) {
  if (start) {
    Start();
  }
}

void DirectViewer::Start() {
  if (started_) {
    throw std::runtime_error("Already started");
    return;
  }
  started_ = true;
  StartServer();
  data_writer_ = std::make_shared<io::QueueWriter<RenderData>>(data_queue_);
}

void DirectViewer::StartServer() {
  // FIXME(yycho0108): should not be RenderData
  io::QueueWriterPtr<RenderData> event_writer =
      std::make_shared<io::QueueWriter<RenderData>>(event_queue_);
  io::QueueListenerPtr<RenderData> data_listener =
      std::make_shared<io::QueueListener<RenderData>>(data_queue_);
  viewer_ =
      std::make_shared<VtkViewer>(data_listener, event_writer, true, false);
}

void DirectViewer::Render(const RenderData& data) {
  // NOTE: Client-side.
  if (!data_writer_) {
    return;
  }
  // TODO(yycho0108): Consider exposing flush() option.
  data_writer_->Send<RenderData>(data, true);
  viewer_->Step();
}

void DirectViewer::Step() { viewer_->Step(); }
void DirectViewer::Spin() { viewer_->Spin(); }

}  // namespace vis
}  // namespace cho
