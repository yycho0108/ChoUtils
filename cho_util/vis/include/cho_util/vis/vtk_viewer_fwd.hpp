#pragma once

#include <memory>

namespace cho_util {
namespace vis {

template <typename ListenerPtr, typename WriterPtr>
class VtkViewer;

template <typename ListenerPtr, typename WriterPtr>
using VtkViewerPtr = std::shared_ptr<VtkViewer<ListenerPtr, WriterPtr>>;

}  // namespace vis
}  // namespace cho_util
