#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

#include <Eigen/Core>

#include "cho_util/core/random.hpp"

namespace cho {
namespace core {

void GenerateConvexPolygon(
    const int n, std::vector<Eigen::Vector2f>* const points,
    cho::core::RNG& rng = cho::core::RNG::GetInstance()) {
  assert(n > 2);

  std::vector<float> xv, yv;
  for (auto& v_out : {&xv, &yv}) {
    float v0{1}, v1{1};
    v_out->clear();
    v_out->reserve(n);
    for (int i = 0; i < n - 2; ++i) {
      const bool sel = rng.Uniform(0, 1) < 0.5f;
      const float v = rng.Uniform(0, 1);
      if (sel) {
        v_out->emplace_back(v - v0);
        v0 = v;
      } else {
        v_out->emplace_back(v1 - v);
        v1 = v;
      }
    }
    v_out->emplace_back(v1 - 0);
    v_out->emplace_back(0 - v0);
  }
  std::shuffle(yv.begin(), yv.end(), rng.generator);

  // Compute angles of each cumulative vector.
  std::vector<float> hv(n);
  for (int i = 0; i < n; ++i) {
    hv[i] = std::atan2(yv[i], xv[i]);
  }

  // Argsort by angle.
  std::vector<int> indices(n);
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(indices.begin(), indices.end(),
            [&hv](const int i0, const int i1) { return hv[i0] < hv[i1]; });

  // Accumulative vector sum into output.
  Eigen::Vector2f point{0, 0};
  points->clear();
  points->reserve(n);
  for (int i = 0; i < xv.size(); ++i) {
    point.x() += xv[indices[i]];
    point.y() += yv[indices[i]];
    points->emplace_back(point);
  }
}

}  // namespace core
}  // namespace cho
