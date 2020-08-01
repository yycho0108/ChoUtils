#pragma once

#include <variant>

#include "cho_util/core/geometry/geometry_base.hpp"

#include "cho_util/core/geometry/cuboid.hpp"
#include "cho_util/core/geometry/cylinder.hpp"
#include "cho_util/core/geometry/line.hpp"
#include "cho_util/core/geometry/plane.hpp"
#include "cho_util/core/geometry/point.hpp"
#include "cho_util/core/geometry/point_cloud.hpp"
#include "cho_util/core/geometry/sphere.hpp"

namespace cho {
namespace core {
using GeometryVariant =
    std::variant<core::Point<float, 3>, core::PointCloud<float, 3>,
                 core::Line<float, 3>, core::Lines<float, 3>,
                 core::Plane<float, 3>, core::Sphere<float, 3>,
                 core::Cylinder<float>, core::Cuboid<float, 3>>;
}
}  // namespace cho
