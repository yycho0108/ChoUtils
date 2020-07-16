#pragma once

#include "cho_util/vis/render_data_fwd.hpp"

#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>

namespace cho {
namespace vis {

struct RenderData {
  enum RenderType : std::int16_t {
    kNone,
    kPoints,
    kLines,
    kPlane,
    kSphere,
    kBox,
    kUser
  };
  enum class Representation : std::int16_t {
    kNone,
    kPoints,
    kWireframe,
    kSurface
  };

  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    auto& rep = reinterpret_cast<std::underlying_type<Representation>::type&>(
        representation);
    ar& render_type;
    ar& tag;
    ar& data;
    ar& color;
    ar& rep;
    ar& quit;
  }

  int render_type;
  std::string tag;
  std::vector<float> data;
  std::vector<std::uint8_t> color;
  Representation representation;
  bool quit;
};
}  // namespace vis
}  // namespace cho
