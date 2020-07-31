#include <cho_util/core/geometry.hpp>
#include <cho_util/vis/remote_viewer_client.hpp>
#include <cho_util/vis/render_data.hpp>
#include "cho_util/proto/render.pb.h"
#include "cho_util/type/convert.hpp"

#include <fmt/printf.h>

int main() {
  fmt::print("A\n");
  fmt::print("B\n");

  auto geom = std::make_shared<cho::core::Cuboid<float, 3>>();
  geom->SetMin(Eigen::Vector3f{0, 0, 0});
  geom->SetMax(Eigen::Vector3f{0.5, 0.5, 0.5});
  fmt::print("{}\n", geom->GetMin().transpose());
  fmt::print("{}\n", geom->GetMax().transpose());
  cho::vis::RenderData cube{
      .render_type = cho::vis::RenderType::kCuboid,
      .tag = "cube",
      .geometry = geom,
      .color = {255, 255, 255},
      .representation = cho::vis::RenderData::Representation::kWireframe,
      .quit = false};
  // auto req = cho::type::Convert<cho::proto::vis::render::RenderRequest>(cube);
  fmt::print("C\n");
  cho::vis::RenderClient viewer;
  viewer.Render(cube);
  fmt::print("D\n");
  viewer.Spin();
  return EXIT_SUCCESS;
}
