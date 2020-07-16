#include "cho_util/vis/remote_viewer.hpp"
#include "cho_util/vis/remote_viewer_server.hpp"

int main(const int argc, const char* argv[]) {
  cho::vis::RemoteViewerServer server{true};
  return EXIT_SUCCESS;
}
