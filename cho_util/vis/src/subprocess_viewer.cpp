#include "cho_util/vis/subprocess_viewer.hpp"

#include "cho_util/core/geometry.hpp"
#include "cho_util/vis/pipe_io.hpp"
#include "cho_util/vis/render_data.hpp"
#include "cho_util/vis/subprocess.hpp"
#include "cho_util/vis/vtk_viewer.hpp"

namespace cho {
namespace vis {

void SubprocessViewer::Start() {
  if (started_) {
    throw std::runtime_error("Already started");
    return;
  }
  started_ = true;
  proc.Start();
  if (IsServer()) {
    StartServer();
  } else {
    data_writer_ = std::make_shared<FdWriter>(proc.GetWriteFd());
  }
}

void SubprocessViewer::StartServer() {
  FdWriterPtr writer = std::make_shared<FdWriter>(proc.GetWriteFd());
  FdListenerPtr<RenderData> listener =
      std::make_shared<FdListener<RenderData>>(proc.GetReadFd());
  viewer_ = std::make_shared<VtkViewer>(listener, writer, true);
}

void SubprocessViewer::Render(const RenderData& data) {
  if (!IsClient()) {
    throw std::logic_error("Cannot directly invoke render on server.");
  }
  // NOTE: Client-side.
  if (!data_writer_) {
    fmt::print("NO WRITER\n");
    return;
  }
  data_writer_->Send<RenderData>(data);
}

void SubprocessViewer::Spin() {
  // As the client, we do not have access to the viewer in the server.
  // the reasonable thing to do is just wait forever.
  // https://stackoverflow.com/a/42644441
  if (IsClient()) {
    std::promise<void>().get_future().wait();
  }

  // The below block should generally never be reached, but implementing a valid
  // alternative nonetheless.
  if (IsServer()) {
    viewer_->Spin();
  }
}

}  // namespace vis
}  // namespace cho
