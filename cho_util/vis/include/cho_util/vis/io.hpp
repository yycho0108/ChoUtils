#pragma once

#include "cho_util/vis/io_fwd.hpp"

#include <functional>

namespace cho {
namespace vis {

template <typename DataType>
struct Listener {
  using Callback = std::function<bool(DataType&&)>;
  Listener(){}
  virtual ~Listener() {}

  virtual void SetCallback(const Callback& on_data) = 0;
  bool OnData(DataType&& data);

  virtual bool IsRunning() const;
  virtual void Start();
  virtual void Stop();
};

struct Writer {
  Writer(){}
  virtual ~Writer(){}
  template <typename DataType>
  bool Send(const DataType& data, const bool flush = true);
};
}  // namespace vis
}  // namespace cho
