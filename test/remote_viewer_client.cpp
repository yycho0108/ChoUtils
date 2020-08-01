#include <variant>

#include "cho_util/core/geometry.hpp"
#include "cho_util/core/geometry/line.hpp"
#include "cho_util/core/geometry/point_cloud.hpp"
#include "cho_util/core/geometry/sphere.hpp"
#include "cho_util/type/convert.hpp"
#include "cho_util/vis/remote_viewer.hpp"
#include "cho_util/vis/render_data.hpp"

#include <fmt/printf.h>
#include <unistd.h>

int main() {
  cho::vis::RemoteViewerClient viewer;

  cho::vis::RenderData cube{
      .tag = "cube",
      .geometry = cho::core::Cuboid<float, 3>{{0, 0, 0, 0.5, 0.5, 1.0}},
      .color = {255, 255, 255},
      .representation = cho::vis::RenderData::Representation::kWireframe,
      .quit = false};
  viewer.Render(cube);

  cho::vis::RenderData sphere{
      .tag = "sphere",
      .geometry = cho::core::Sphere<float, 3>{{0, 0, 0, 0.4}},
      .color = {255, 0, 0},
      .representation = cho::vis::RenderData::Representation::kSurface,
      .quit = false};
  viewer.Render(sphere);

  cho::core::PointCloud<float, 3> cloud_geom;
  auto& ps = cloud_geom.GetData();
  ps.resize(3, 128);
  ps.setRandom(3, 128);

  for (int i = 0; i < 1; ++i) {
    cho::vis::RenderData cloud{
        .tag = "cloud",
        .geometry = cloud_geom,
        .color = {255, 255, 0},
        .representation = cho::vis::RenderData::Representation::kPoints,
        .quit = false};
    viewer.Render(cloud);
    // usleep(10000);
  }

  cho::core::Lines<float, 3> lines_geom;
  lines_geom.SetNumLines(7);
  fmt::print("{}\n", lines_geom.GetNumLines());
  auto& ls = lines_geom.GetData();
  ls.setRandom();
  fmt::print("{}x{}\n", ls.rows(), ls.cols());

  cho::vis::RenderData lines{
      .tag = "lines",
      .geometry = lines_geom,
      .color = {255, 255, 0},
      .representation = cho::vis::RenderData::Representation::kWireframe,
      .quit = false};

  viewer.Render(lines);

  viewer.Spin();
  return EXIT_SUCCESS;
}
