#include "cho_util/core/random.hpp"

namespace cho_util {
namespace core {

float RNG::Randn() { return ndist(generator); }

float RNG::Randu() { return udist(generator); }

float RNG::Uniform(float max) { return max * Randu(); }
float RNG::Uniform(float min, float max) { return min + (max - min) * Randu(); }

float RNG::Normal(float scale) { return scale * Randn(); }
float RNG::Normal(float mean, float scale) { return mean + scale * Randn(); }

void RNG::SetSeed(const std::size_t seed) { generator.seed(seed); }

}  // namespace core
}  // namespace cho_util
