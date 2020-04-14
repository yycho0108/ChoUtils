#include <cho_util/core/random.hpp>
#include <cho_util/util/timer.hpp>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <chrono>
#include <thread>

#include "cho_util/vis/embedded_viewer.hpp"
#include "cho_util/vis/render_data.hpp"

auto& rng = cho_util::core::RNG::GetInstance();

int main(int, char* []) {
    cho_util::vis::EmbeddedViewer viewer{true};
  if (!viewer.IsClient()) {
    throw std::runtime_error("Must be client");
  }

  cho_util::vis::RenderData data;
  for (int i = 0; i < 128; ++i) {
    data.tag = "cloud";
    // data.tag = fmt::format("cloud{}", i);
    data.render_type = cho_util::vis::RenderData::RenderType::kPoints;
    data.data.resize(128 * 3);
    if (i == 0) {
      std::generate(data.data.begin(), data.data.end(),
                    []() { return rng.Randn(); });
    } else {
      std::transform(data.data.begin(), data.data.end(), data.data.begin(),
                     [](float x) { return x + 0.2; });
    }
    data.quit = false;
    viewer.Render(data);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  // Sphere?
  data.tag = "sphere";
  // data.render_type = rtype;

  // Plane.
  for (int i = 0; i < 64; ++i) {
    fmt::print("PLANE\n");
    data.tag = "plane";
    // data.render_type = RenderData::RenderType::kPlane;
    data.render_type = cho_util::vis::RenderData::RenderType::kUser;
    data.data.resize(6);
    if (i == 0) {
      std::fill(data.data.begin(), data.data.begin() + 3, 0);
      std::fill(data.data.begin() + 3, data.data.begin() + 6, 0);
      data.data[5] = 1;
    } else {
      data.data[2] += 0.1;
    }
    data.quit = false;
    viewer.Render(data);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  // bbox
  for (int i = 0; i < 64; ++i) {
    data.tag = "box";
    data.render_type = RenderData::RenderType::kBox;
    data.data.resize(6);
    if (i == 0) {
      std::fill(data.data.begin(), data.data.begin() + 3, 0);
      std::fill(data.data.begin() + 3, data.data.begin() + 6, 1);
    } else {
      std::transform(data.data.begin(), data.data.end(), data.data.begin(),
                     [](float x) { return x + 0.1; });
    }
    data.quit = false;
    viewer.Render(data);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  // Plane.
  // fmt::print("PLANE\n");
  // data.tag = "plane";
  // data.render_type = RenderData::RenderType::kPlane;
  // data.data.resize(6);
  // std::fill(data.data.begin(), data.data.begin() + 3, 4);
  // std::fill(data.data.begin() + 3, data.data.begin() + 6, 0);
  // data.data[5] = 1;
  // data.quit = false;
  // rc.Send(data);

  // End.
  fmt::print("END\n");
  data.tag = "end";
  data.render_type = RenderData::RenderType::kNone;
  data.data.clear();
  data.quit = true;
  viewer.Render(data);
  fmt::print("waiting...\n");

  std::this_thread::sleep_for(std::chrono::seconds(5));
  return EXIT_SUCCESS;
}
