#pragma once

#include "cho_util/vis/pipe_io_fwd.hpp"

#include <chrono>
#include <future>
#include <iostream>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>

#include "cho_util/vis/io.hpp"

namespace cho_util {
namespace vis {

/**
 * Pipe listener
 */
template <typename DataType>
class FdListener : public Listener<DataType> {
 public:
  using typename Listener<DataType>::Callback;
  FdListener(int fd) : fd(fd) {}

  FdListener(int fd, std::function<bool(DataType&&)>& on_data)
      : fd(fd), on_data(on_data) {}

  void SetCallback(const Callback& on_data) { this->on_data = on_data; }

  void Start() {
    worker = std::async(std::launch::async, [this] {
      // Convert file descriptor to stream archive format.
      boost::iostreams::file_descriptor_source fds{
          fd, boost::iostreams::file_descriptor_flags::never_close_handle};
      boost::iostreams::stream<boost::iostreams::file_descriptor_source> fin(
          fds);
      boost::archive::binary_iarchive ar(fin);

      DataType data;
      while (!quit && fin) {
        ar >> data;
        quit |= OnData(std::move(data));
      }
    });
  }

  inline bool OnData(DataType&& data) const { return on_data(std::move(data)); }
  inline bool IsRunning() const { return IsThreadRunning(); }

  inline bool IsThreadRunning() const {
    if (worker.valid()) {
      auto status = worker.wait_for(std::chrono::seconds(0));
      if (status == std::future_status::ready) {
        return false;
      }
      return true;
    }
    return false;
  }

  void Stop() {
    quit = true;
    while (IsThreadRunning()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    worker = std::future<void>();
  }
  ~FdListener() { Stop(); }

 private:
  int fd{0};
  bool quit{false};
  std::future<void> worker;
  std::function<bool(DataType&&)> on_data;
};

/**
 * Lightweight wrapper to handle comm.
 */

class FdWriter : public Writer {
 public:
  inline FdWriter(int fd)
      : fd(fd),
        fds{fd, boost::iostreams::file_descriptor_flags::never_close_handle},
        fout{fds},
        ar{fout} {};

  template <typename T>
  bool Send(const T& data, const bool flush = true) {
    ar << data;
    if (flush) {
      std::flush(fout);
    }
  }

 private:
  int fd;
  boost::iostreams::file_descriptor_sink fds;
  boost::iostreams::stream<boost::iostreams::file_descriptor_sink> fout;
  boost::archive::binary_oarchive ar;
};

}  // namespace vis
}  // namespace cho_util
