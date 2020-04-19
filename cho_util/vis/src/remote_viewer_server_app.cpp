#include "cho_util/vis/remote_viewer.hpp"
#include "cho_util/vis/remote_viewer_server.hpp"

int main(const int argc, const char* argv[]) {
  cho_util::vis::RemoteViewerServer server{true};
  return 0;
}
