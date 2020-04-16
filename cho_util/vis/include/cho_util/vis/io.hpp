#pragma once

#include "cho_util/vis/io_fwd.hpp"

namespace cho_util {
namespace vis {

template <typename Derived>
struct Listener {
  Listener() {}
  ~Listener() {}

  template <typename Callback>
  inline void SetCallback(const Callback& on_data) {
    static_cast<Derived*>(this)->SetCallback(on_data);
  }

  template <typename DataType>
  inline bool OnData(DataType&& data) const {
    return static_cast<Derived*>(this)->OnData(data);
  }

  inline bool IsRunning() const {
    return static_cast<const Derived*>(this)->IsRunning();
  }

  inline void Start() { static_cast<Derived*>(this)->Start(); }
  inline void Stop() { static_cast<Derived*>(this)->Stop(); }
};

template <typename Derived>
struct Writer {
  Writer() {}
  virtual ~Writer() {}

  template <typename T>
  inline bool Send(const T& data, const bool flush = true) {
    static_cast<Derived*>(this)->Send(data, flush);
  }
};
}  // namespace vis
}  // namespace cho_util
