#pragma once

#include <memory>
#include <string>

#include "cho_util/proto/render.grpc.pb.h"
#include "cho_util/vis/render_data_fwd.hpp"

namespace cho {
namespace vis {

class RenderClient {
 public:
  explicit RenderClient(const std::string& channel = "localhost:50051");
  void Render(const RenderData& data);
  void Spin();

 private:
  // Out of the passed in Channel comes the stub, stored here, our view of the
  // server's exposed services.
  std::unique_ptr<proto::vis::render::Renderer::Stub> stub_;
};
}  // namespace vis
}  // namespace cho
