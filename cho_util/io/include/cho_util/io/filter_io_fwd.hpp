#pragma once

#include <memory>

namespace cho {
namespace io {

template <typename T, typename SourceType>
class FilteredListener;

template <typename T, typename SourceType>
using FilteredListenerPtr = std::shared_ptr<FilteredListener<T, SourceType>>;

}  // namespace io
}  // namespace cho
