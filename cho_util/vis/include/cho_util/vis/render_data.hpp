#pragma once

#include <memory>
#include <utility>
#include <variant>

#include <boost/serialization/array.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

#include "cho_util/core/geometry.hpp"
#include "cho_util/core/serialize_std_variant.hpp"
#include "cho_util/vis/render_data_fwd.hpp"

namespace cho {
namespace vis {

struct RenderData {
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
    ar& tag;
    ar& geometry;
    ar& color;
    ar& rep;
    ar& quit;
  }

  std::string tag;
  core::GeometryVariant geometry;
  std::vector<std::uint8_t> color;
  Representation representation;
  bool quit;
};

}  // namespace vis
}  // namespace cho
