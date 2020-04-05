#pragma once

#include <random>

namespace cho_util {
namespace core {

struct RNG {
  float Randu();
  float Randn();

  float Uniform(float max);
  float Uniform(float min, float max);
  float Normal(float scale);
  float Normal(float mean, float scale);

  void SetSeed(const std::size_t seed);

  std::default_random_engine generator;
  std::normal_distribution<float> ndist;
  std::uniform_real_distribution<float> udist;

  inline static RNG& GetInstance() {
    static RNG rng;
    return rng;
  }
};

}  // namespace core
}  // namespace cho_util
