#pragma once

#include "cho_util/io/filter_io_fwd.hpp"

#include "cho_util/io/io.hpp"

namespace cho {
namespace io {

template <typename DataType, typename SourceType>
class FilteredListener : public Listener<DataType> {
 public:
  using typename Listener<DataType>::Callback;

  FilteredListener(const ListenerPtr<SourceType>& source) : source_(source) {}

  void SetCallback(const Callback& on_data) { this->on_data_ = on_data; }
  void Start() {
    if (!source_->IsRunning()) {
      source_->Start();
    }
  }
  inline bool OnData(DataType&& data) const {
    if (stopped_) {
      return false;
    }
    return on_data_(std::move(data));
  }
  inline bool IsRunning() const {
    return (source_ != nullptr) && source_->IsRunning();
  }
  void Stop() { stopped_ = true; }

 private:
  ListenerPtr<SourceType> source_;
  std::function<bool(DataType&&)> on_data_;
  bool stopped_{false};
};

}  // namespace io
}  // namespace cho
