#pragma once

#include <variant>

#include "cho_util/vis/vtk_viewer/handler/handler_base.hpp"

#include "cho_util/vis/vtk_viewer/handler/cloud_handler.hpp"
#include "cho_util/vis/vtk_viewer/handler/cuboid_handler.hpp"
#include "cho_util/vis/vtk_viewer/handler/cylinder_handler.hpp"
#include "cho_util/vis/vtk_viewer/handler/line_handler.hpp"
#include "cho_util/vis/vtk_viewer/handler/plane_handler.hpp"
// #include "cho_util/vis/handler/point_handler.hpp"
#include "cho_util/vis/vtk_viewer/handler/sphere_handler.hpp"

namespace cho {
namespace vis {

using HandlerVariant =
    std::variant<CloudHandler, CuboidHandler, CylinderHandler, LineHandler,
                 PlaneHandler, SphereHandler>;

template <typename Geometry>
struct HandlerMap {};
template <>
struct HandlerMap<core::Cuboid<float, 3>> {
  using type = CuboidHandler;
};
template <>
struct HandlerMap<core::Cylinder<float>> {
  using type = CylinderHandler;
};
template <>
struct HandlerMap<core::Sphere<float, 3>> {
  using type = SphereHandler;
};
template <>
struct HandlerMap<core::Point<float, 3>> {
  // FIXME(yycho0108): invalid
  using type = CloudHandler;
};
template <>
struct HandlerMap<core::PointCloud<float, 3>> {
  using type = CloudHandler;
};
template <>
struct HandlerMap<core::Plane<float, 3>> {
  using type = PlaneHandler;
};
template <>
struct HandlerMap<core::Line<float, 3>> {
  // FIXME(yycho0108): invalid
  using type = LineHandler;
};
template <>
struct HandlerMap<core::Lines<float, 3>> {
  using type = LineHandler;
};
template <typename Geometry>
using HandlerType = typename HandlerMap<Geometry>::type;

}  // namespace vis
}  // namespace cho
