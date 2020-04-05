#pragma once

#include <vector>

namespace cho_util {
namespace core {

void Linspace(const float min, const float max, const int size,
              std::vector<float>* out);

std::vector<float> Linspace(const float min, const float max, const int size);

}  // namespace core
}  // namespace cho_util
