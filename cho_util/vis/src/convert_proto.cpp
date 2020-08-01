#include "cho_util/vis/convert_proto.hpp"

#include <fmt/format.h>
#include <Eigen/Core>

#include "cho_util/core/geometry.hpp"
#include "cho_util/proto/geometry.pb.h"
#include "cho_util/proto/render.grpc.pb.h"
#include "cho_util/proto/render.pb.h"
#include "cho_util/type/convert.hpp"
#include "cho_util/util/mp_utils.hpp"
#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace type {

CHO_DEFINE_CONVERT(proto::vis::render::RepresentationType,
                   vis::RenderData::Representation) {
#define ADD_CASE(name, name2)                                 \
  case ::cho::proto::vis::render::RepresentationType::name: { \
    *target = ::cho::vis::RenderData::Representation::name2;  \
    break;                                                    \
  }

  switch (source) {
    ADD_CASE(kAsNone, kNone);
    ADD_CASE(kAsPoints, kPoints);
    ADD_CASE(kAsWireframe, kWireframe);
    ADD_CASE(kAsSurface, kSurface);
    default: {
      throw std::out_of_range("undefined representation type");
      break;
    }
  }
#undef ADD_CASE
}
CHO_DEFINE_CONVERT(vis::RenderData::Representation,
                   proto::vis::render::RepresentationType) {
#define ADD_CASE(name, name2)                                       \
  case ::cho::vis::RenderData::Representation::name: {              \
    *target = ::cho::proto::vis::render::RepresentationType::name2; \
    break;                                                          \
  }
  switch (source) {
    ADD_CASE(kNone, kAsNone);
    ADD_CASE(kPoints, kAsPoints);
    ADD_CASE(kWireframe, kAsWireframe);
    ADD_CASE(kSurface, kAsSurface);
    default: {
      throw std::out_of_range("undefined representation type");
      break;
    }
  }
#undef ADD_CASE
}

void ::cho::type::
    Converter<vis::RenderData, proto::vis::render::RenderRequest>::ConvertTo(
        const vis::RenderData& rd,
        proto::vis::render::RenderRequest* const req) {
  req->set_name(rd.tag);
  req->set_representation(
      Convert<proto::vis::render::RepresentationType>(rd.representation));

  // Opacity defaults to 1.0 for now.
  req->set_opacity(1.0f);
  std::visit(
      cho::util::overloaded{[&req](const core::PointCloud<float, 3>& geom) {
                              cho::type::Convert(geom, req->mutable_cloud());
                            },
                            [&req](const core::Lines<float, 3>& geom) {
                              cho::type::Convert(geom, req->mutable_lines());
                            },
                            [&req](const core::Plane<float, 3>& geom) {
                              cho::type::Convert(geom, req->mutable_plane());
                            },
                            [&req](const core::Sphere<float, 3>& geom) {
                              cho::type::Convert(geom, req->mutable_sphere());
                            },
                            [&req](const core::Cuboid<float, 3>& geom) {
                              cho::type::Convert(geom, req->mutable_cuboid());
                            },
                            [](const auto& arg) {
                              throw std::out_of_range("Unsupported overload");
                            }},
      rd.geometry);

  // Set Color.
  if (rd.color.size() == 3) {
    auto color = req->mutable_uniform();
    color->set_r(rd.color[0] / 255.0f);
    color->set_g(rd.color[1] / 255.0f);
    color->set_b(rd.color[2] / 255.0f);
  }
  if (rd.color.size() > 3) {
    auto colors = req->mutable_each();
    colors->mutable_colors()->Reserve(rd.color.size());
    for (int i = 0; i < rd.color.size(); i += 3) {
      auto color = colors->mutable_colors()->Add();
      color->set_r(rd.color[i] / 255.0f);
      color->set_g(rd.color[i + 1] / 255.0f);
      color->set_b(rd.color[i + 2] / 255.0f);
    }
    req->set_allocated_each(colors);
  }
}

void ::cho::type::Converter<
    proto::vis::render::RenderRequest,
    vis::RenderData>::ConvertTo(const proto::vis::render::RenderRequest& req,
                                vis::RenderData* const rd) {
  rd->tag = req.name();
  Convert(req.representation(), &rd->representation);
  rd->quit = false;

  switch (req.data_case()) {
    case proto::vis::render::RenderRequest::DataCase::kCloud: {
      rd->geometry = Convert<core::PointCloud<float, 3>>(req.cloud());
      break;
    }
    case proto::vis::render::RenderRequest::DataCase::kCuboid: {
      rd->geometry = Convert<core::Cuboid<float, 3>>(req.cuboid());
      break;
    }
    case proto::vis::render::RenderRequest::DataCase::kLines: {
      rd->geometry = Convert<core::Lines<float, 3>>(req.lines());
      break;
    }
    case proto::vis::render::RenderRequest::DataCase::kPlane: {
      rd->geometry = Convert<core::Plane<float, 3>>(req.plane());
      break;
    }
    case proto::vis::render::RenderRequest::DataCase::kSphere: {
      rd->geometry = Convert<core::Sphere<float, 3>>(req.sphere());
      break;
    }
    default: {
      fmt::print("DC={}\n", req.data_case());
      throw std::out_of_range("Unsupported data case !!!");
      break;
    }
  }

  // Set Color.
  rd->color.clear();
  switch (req.color_case()) {
    case proto::vis::render::RenderRequest::kUniform: {
      rd->color.reserve(3);
      rd->color.emplace_back(req.uniform().r() * 255);
      rd->color.emplace_back(req.uniform().g() * 255);
      rd->color.emplace_back(req.uniform().b() * 255);
      break;
    }
    case proto::vis::render::RenderRequest::kEach: {
      rd->color.reserve(req.each().colors_size() * 3);
      for (int i = 0; i < req.each().colors_size(); ++i) {
        rd->color.emplace_back(req.each().colors(i).r() * 255);
        rd->color.emplace_back(req.each().colors(i).g() * 255);
        rd->color.emplace_back(req.each().colors(i).b() * 255);
      }
      break;
    }
    default:
      break;
  }
}

}  // namespace type
}  // namespace cho
