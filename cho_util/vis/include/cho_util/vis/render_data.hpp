#pragma once

#include <memory>
#include <variant>

#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>

#include "cho_util/core/geometry.hpp"
#include "cho_util/vis/render_data_fwd.hpp"

namespace cho {
namespace vis {

using GeometryVariant =
    std::variant<core::Point<float, 3>, core::PointCloud<float, 3>,
                 core::Line<float, 3>, core::Lines<float, 3>,
                 core::Plane<float, 3>, core::Sphere<float, 3>,
                 core::Cylinder<float>, core::Cuboid<float, 3>>;
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

enum RenderType : std::int16_t {
  kNone,
  kPoints,
  kLines,
  kLinestrip,
  kLineloop,
  kTriangles,
  kPlane,
  kSphere,
  kCylinder,
  kBox,
  kUser,
  kCuboid = kBox
};

template <RenderType RT>
struct GeometryMap;

template <>
struct GeometryMap<RenderType::kPoints> {
  using type = core::PointCloud<float, 3>;
};
template <>
struct GeometryMap<RenderType::kLines> {
  using type = core::Lines<float, 3>;
};
template <>
struct GeometryMap<RenderType::kPlane> {
  using type = core::Plane<float, 3>;
};
template <>
struct GeometryMap<RenderType::kSphere> {
  using type = core::Sphere<float, 3>;
};
template <>
struct GeometryMap<RenderType::kCylinder> {
  using type = core::Cylinder<float>;
};
template <>
struct GeometryMap<RenderType::kCuboid> {
  using type = core::Cuboid<float, 3>;
};

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
    ar& render_type;
    ar& tag;
    std::visit([&ar](auto& geom) { ar& geom; }, geometry);
  }

  //#define ADD_CASE(name)                                                       \
  //case RenderType::name: {                                                   \
  //  auto geo = std::dynamic_pointer_cast<GeometryMap<name>::type>(geometry); \
  //  ar&geo;                                                               \
  //  break;                                                                   \
  //}
  //    switch (render_type) {
  //      ADD_CASE(kPoints);
  //      ADD_CASE(kLines);
  //      ADD_CASE(kPlane);
  //      ADD_CASE(kSphere);
  //      ADD_CASE(kCylinder);
  //      ADD_CASE(kCuboid);
  //      default: {
  //        throw std::out_of_range("Unsupported render type");
  //        break;
  //      }
  //    }
  //#undef ADD_CASE
  //    ar& color;
  //    ar& rep;
  //    ar& quit;
  //  }

  RenderType render_type;
  std::string tag;
  // std::shared_ptr<cho::core::AbstractGeometry<float>> geometry;
  GeometryVariant geometry;
  std::vector<std::uint8_t> color;
  Representation representation;
  bool quit;
};

}  // namespace vis
}  // namespace cho
