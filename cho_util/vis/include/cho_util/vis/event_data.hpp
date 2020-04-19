#pragma once

#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>

namespace cho_util {
namespace vis {

struct EventData {
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int) {
    ar& dummy;
  }
  int dummy;
};
}  // namespace vis
}  // namespace cho_util
