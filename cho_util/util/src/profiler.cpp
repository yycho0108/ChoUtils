#include "cho_util/util/profiler.hpp"

#include <fstream>
#include <iostream>

namespace cho_util {
namespace util {

void Profiler::Start(const std::string& tag) {
  auto it = timers_.find(tag);
  if (it == timers_.end()) {
  } else {
    it->second.Start();
  }
}

void Profiler::Stop(const std::string& tag) {
  stats_[tag].emplace_back(timers_[tag].StopAndGetElapsedTime());
}

void Profiler::Save(const std::string& filename) const {
  std::ofstream fout(filename);
  for (const auto& entry : stats_) {
    fout << entry.first << std::endl;
    for (const auto& t : entry.second) {
      fout << t << ' ';
    }
    fout << std::endl;
  }
}

}  // namespace util
}  // namespace cho_util
