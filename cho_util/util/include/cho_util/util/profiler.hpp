#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

#include "cho_util/util/timer.hpp"

namespace cho {
namespace util {

class Profiler {
 private:
  std::unordered_map<std::string, UTimer> timers_;
  std::unordered_map<std::string, std::vector<float>> stats_;

 public:
  static Profiler& GetInstance() {
    static Profiler instance;
    return instance;
  }

  void Start(const std::string& tag);
  void Stop(const std::string& tag);
  void Save(const std::string& filename = "/tmp/fout.txt") const;
};

}  // namespace util
}  // namespace cho
