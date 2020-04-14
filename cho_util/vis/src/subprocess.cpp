#include "cho_util/vis/subprocess.hpp"

#include <unistd.h>
#include <csignal>

namespace cho_util {
namespace vis {

void Subprocess::Start() {
  if (pid_ >= 0) {
    throw std::runtime_error("Already started");
  }
  // Create pipes
  if (pipe(fd_c2p_) == -1) {
    throw std::runtime_error("pipe failed");
  }
  if (pipe(fd_p2c_) == -1) {
    throw std::runtime_error("pipe failed");
  }

  // Create subprocess
  pid_ = fork();
  if (pid_ < 0) {
    throw std::runtime_error("fork failed");
  }
}

void Subprocess::Stop() {
  // Cleanup subprocess
  if (IsParent()) {
    kill(pid_, SIGTERM);
    pid_ = -1;
  }

  // Close all pipes
  close(fd_c2p_[0]);
  close(fd_c2p_[1]);
  close(fd_p2c_[0]);
  close(fd_p2c_[1]);
}

}  // namespace vis
}  // namespace cho_util
