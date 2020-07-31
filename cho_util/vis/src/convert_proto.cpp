#include "cho_util/vis/convert_proto.hpp"

#include <memory>

#include <fmt/format.h>
#include <Eigen/Core>

#include "cho_util/core/geometry.hpp"
#include "cho_util/core/geometry/geometry_base.hpp"
#include "cho_util/proto/geometry.pb.h"
#include "cho_util/proto/render.grpc.pb.h"
#include "cho_util/proto/render.pb.h"
#include "cho_util/type/convert.hpp"
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

CHO_DEFINE_CONVERT(proto::vis::render::RenderType, vis::RenderType) {
  // Save typing.
#define ADD_CASE(name)                      \
  case ::cho::proto::vis::render::name: {   \
    *target = ::cho::vis::RenderType::name; \
    break;                                  \
  }
  switch (source) {
    ADD_CASE(kNone)
    ADD_CASE(kPoints)
    ADD_CASE(kLines)
    ADD_CASE(kLinestrip)
    ADD_CASE(kLineloop)
    ADD_CASE(kTriangles)
    ADD_CASE(kPlane)
    ADD_CASE(kSphere)
    ADD_CASE(kCuboid)
    default: {
      const std::string& msg = fmt::format(
          "Unsupported render type : {}",
          static_cast<
              const std::underlying_type<proto::vis::render::RenderType>::type>(
              source));
      throw std::out_of_range(msg);
      break;
    }
  }
#undef ADD_CASE
}

CHO_DEFINE_CONVERT(vis::RenderType, proto::vis::render::RenderType) {
#define ADD_CASE(name)                  \
  case vis::RenderType::name: {         \
    *target = proto::vis::render::name; \
    break;                              \
  }
  switch (source) {
    ADD_CASE(kNone)
    ADD_CASE(kPoints)
    ADD_CASE(kLines)
    ADD_CASE(kLinestrip)
    ADD_CASE(kLineloop)
    ADD_CASE(kTriangles)
    ADD_CASE(kPlane)
    ADD_CASE(kSphere)
    ADD_CASE(kCuboid)
    default: {
      const std::string& msg = fmt::format(
          "Unsupported render type : {}",
          static_cast<
              const std::underlying_type<proto::vis::render::RenderType>::type>(
              source));
      throw std::out_of_range(msg);
      break;
    }
  }
#undef ADD_CASE
}

void ::cho::type::
    Converter<vis::RenderData, proto::vis::render::RenderRequest>::ConvertTo(
        const vis::RenderData& rd,
        proto::vis::render::RenderRequest* const req) {
  using RenderType = vis::RenderType;
  using RenderTypeProto = proto::vis::render::RenderType;

  // fmt::print("<set-name>\n");
  req->set_name(rd.tag);
  // fmt::print("</set-name>\n");

  //proto::vis::render::RenderType rtype;
  //Convert(rd.render_type, &rtype);
  //fmt::print("<set-type>\n");
  //fmt::print("type = {}\n", rd.render_type);
  req->set_type(Convert<RenderTypeProto>(rd.render_type));
  // fmt::print("</set-type>\n");
  req->set_representation(
      Convert<proto::vis::render::RepresentationType>(rd.representation));

  // Opacity defaults to 1.0 for now.
  req->set_opacity(1.0f);
  fmt::print("<case>\n");

#define ADD_CASE(NAME, TARGET)                                          \
  case vis::RenderType::NAME: {                                         \
    auto geo = std::dynamic_pointer_cast<                               \
        const cho::vis::GeometryMap<cho::vis::RenderType::NAME>::type>( \
        rd.geometry);                                                   \
    cho::type::Convert(*geo, TARGET);                                   \
    break;                                                              \
  }

  switch (rd.render_type) {
    case vis::RenderType::kNone: {
      break;
    }
      ADD_CASE(kPoints, req->mutable_cloud())
      ADD_CASE(kLines, req->mutable_lines())
      ADD_CASE(kPlane, req->mutable_plane())
      ADD_CASE(kSphere, req->mutable_sphere())
      ADD_CASE(kCuboid, req->mutable_cuboid())
    case vis::RenderType::kUser: {
      throw std::out_of_range("Unsupported REnder Rtype");
      break;
    }
    default: {
      throw std::out_of_range("Unsupported REnder Rtype");
      break;
    }
  }
#undef ADD_CASE
  fmt::print("</case>\n");

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
  fmt::print("cvt done\n");
}

void ::cho::type::Converter<
    proto::vis::render::RenderRequest,
    vis::RenderData>::ConvertTo(const proto::vis::render::RenderRequest& req,
                                vis::RenderData* const rd) {
  rd->tag = req.name();
  Convert(req.type(), &rd->render_type);
  Convert(req.representation(), &rd->representation);
  rd->quit = false;

#define ADD_CASE(NAME, SOURCE)                                      \
  case vis::RenderType::NAME: {                                     \
    auto geo = std::make_shared<                                    \
        cho::vis::GeometryMap<cho::vis::RenderType::NAME>::type>(); \
    cho::type::Convert(SOURCE, geo.get());                          \
    rd->geometry = geo;                                             \
    break;                                                          \
  }

  // Set Points.
  switch (rd->render_type) {
    case vis::RenderType::kNone: {
      break;
    }
      ADD_CASE(kPoints, req.cloud())
      ADD_CASE(kLines, req.lines())
      ADD_CASE(kPlane, req.plane())
      ADD_CASE(kSphere, req.sphere())
      ADD_CASE(kCuboid, req.cuboid())
    default: { break; }
  }
#undef ADD_CASE

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
