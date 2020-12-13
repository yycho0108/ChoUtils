#include "cho_util/vis/subprocess_viewer.hpp"

#include "cho_util/core/geometry.hpp"
#include "cho_util/core/serialize_eigen.hpp"
#include "cho_util/io/filter.hpp"
#include "cho_util/io/filter_io_fwd.hpp"
#include "cho_util/io/pipe_io.hpp"
#include "cho_util/util/mp_utils.hpp"
#include "cho_util/vis/render_data.hpp"
#include "cho_util/vis/subprocess.hpp"
#include "cho_util/vis/vtk_viewer/vtk_viewer.hpp"

namespace cho {
namespace vis {

struct CommandData {
 public:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar& camera_pose;
  }

 public:
  Eigen::Matrix4f camera_pose;
};

using ClientDataVariant = std::variant<RenderData, CommandData>;

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
    data_writer_ = std::make_shared<io::FdWriter>(proc.GetWriteFd());
  }
}

void SubprocessViewer::StartServer() {
  io::FdWriterPtr writer = std::make_shared<io::FdWriter>(proc.GetWriteFd());

  // Main listener
  io::FdListenerPtr<ClientDataVariant> listener =
      std::make_shared<io::FdListener<ClientDataVariant>>(proc.GetReadFd());

  // Slice
  io::FilteredListenerPtr<RenderData, ClientDataVariant> render_listener =
      std::make_shared<io::FilteredListener<RenderData, ClientDataVariant>>(
          listener);

  // Split to event / render-data.
  listener->SetCallback([this, render_listener](ClientDataVariant&& v) -> bool {
    return std::visit(
        cho::util::overloaded{[this](const CommandData& cmd) -> bool {
                                // Handle commands.
                                if (viewer_ == nullptr) {
                                  fmt::print("CANNOT SET\n");
                                  return false;
                                }
                                viewer_->SetCameraPose(
                                    Eigen::Isometry3f{cmd.camera_pose});
                                return false;
                              },
                              [render_listener](const RenderData& rd) -> bool {
                                RenderData cpy{rd};
                                // Handle renders.
                                render_listener->OnData(std::move(cpy));
                                return false;
                              }},
        v);
  });

  viewer_ = std::make_shared<VtkViewer>(render_listener, writer, false, false);
  viewer_->Start(true);
};

void SubprocessViewer::Render(const RenderData& data) {
  if (!IsClient()) {
    throw std::logic_error("Cannot directly invoke render on server.");
  }
  // NOTE: Client-side.
  if (!data_writer_) {
    fmt::print("NO WRITER\n");
    return;
  }
  data_writer_->Send<ClientDataVariant>(ClientDataVariant{data});
}

void SubprocessViewer::SetCameraPose(const Eigen::Isometry3f& pose) {
  if (!IsClient()) {
    throw std::logic_error("Cannot directly invoke SetCameraPose() on server.");
  }
  data_writer_->Send<ClientDataVariant>(
      ClientDataVariant{CommandData{pose.matrix()}});
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
