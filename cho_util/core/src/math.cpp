#include "cho_util/core/math.hpp"

namespace cho_util {
namespace core {

void Linspace(const float min, const float max, const int size,
              std::vector<float>* out) {
  if (size <= 0) {
    return;
  }
  out->resize(size);
  const float step = (max - min) / size;
  out->at(0) = min;
  for (int i = 1; i < size; ++i) {
    out->at(i) = out->at(i - 1) + step;
  }
}
std::vector<float> Linspace(const float min, const float max, const int size) {
  std::vector<float> out;
  Linspace(min, max, size, &out);
  return out;
}
}  // namespace core
}  // namespace cho_util
