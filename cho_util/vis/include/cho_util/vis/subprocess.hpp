#pragma once

#include <memory>

namespace cho_util {
namespace vis {

/**
 * Lightweight wrapper that manages subprocess scope.
 */
class Subprocess {
 public:
  // No copy
  Subprocess(const Subprocess&) = delete;
  Subprocess& operator=(const Subprocess&) = delete;

  inline Subprocess(const bool start = false) {
    if (start) {
      Start();
    }
  }
  inline virtual ~Subprocess() { Stop(); }
  inline static std::shared_ptr<Subprocess> Create(const bool start = false) {
    return std::make_shared<Subprocess>(start);
  }

  void Start();
  void Stop();

  inline int GetReadFd() const { return IsParent() ? fd_c2p_[0] : fd_p2c_[0]; }
  inline int GetWriteFd() const { return IsParent() ? fd_p2c_[1] : fd_c2p_[1]; }
  inline bool IsParent() const { return (pid_ > 0); }

 private:
  int fd_p2c_[2], fd_c2p_[2];
  int pid_{-1};
};

}  // namespace vis
}  // namespace cho_util
