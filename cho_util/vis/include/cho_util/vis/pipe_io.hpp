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

namespace cho_util {
namespace vis {

/**
 * Pipe listener
 */
template <typename T>
struct FdListener {
 public:
  FdListener(int fd, std::function<bool(T&&)>& on_data)
      : fd(fd), on_data(on_data) {}

  void Start() {
    t = std::async(std::launch::async, [this] {
      // Convert file descriptor to stream archive format.
      boost::iostreams::file_descriptor_source fds{
          fd, boost::iostreams::file_descriptor_flags::never_close_handle};
      boost::iostreams::stream<boost::iostreams::file_descriptor_source> fin(
          fds);
      boost::archive::binary_iarchive ar(fin);

      T data;
      while (!quit && fin) {
        ar >> data;
        quit |= on_data(std::move(data));
      }
    });
  }

  inline bool IsThreadRunning() const {
    if (t.valid()) {
      auto status = t.wait_for(std::chrono::seconds(0));
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
    t = std::future<void>();
  }
  ~FdListener() { Stop(); }

 private:
  int fd{0};
  bool quit{false};
  std::future<void> t;
  std::function<bool(T&&)> on_data;
};

/**
 * Lightweight wrapper to handle comm.
 */
struct FdWriter {
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
