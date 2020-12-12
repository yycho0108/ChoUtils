#pragma once

#include "cho_util/io/io_fwd.hpp"

#include <functional>

namespace cho {
namespace io {

template <typename DataType>
struct Listener {
  using Callback = std::function<bool(DataType&&)>;
  Listener() {}
  virtual ~Listener() {}

  virtual void SetCallback(const Callback& on_data) = 0;
  virtual bool OnData(DataType&& data) const = 0;

  virtual bool IsRunning() const = 0;
  virtual void Start() = 0;
  virtual void Stop() = 0;
};

struct Writer {
  Writer() {}
  virtual ~Writer() {}
  template <typename DataType>
  bool Send(const DataType& data, const bool flush = true);
};
}  // namespace io
}  // namespace cho
